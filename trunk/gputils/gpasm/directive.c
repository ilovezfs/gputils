/* Implements directives, pseudo-ops and processor opcodes
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Craig Franklin

This file is part of gputils.

gputils is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

gputils is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with gputils; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include "stdhdr.h"

#include "libgputils.h"
#include "gpasm.h"
#include "directive.h"
#include "evaluate.h"
#include "parse.h"
#include "processor.h"
#include "lst.h"
#include "macro.h"
#include "coff.h"
#include "gperror.h"
#include "special.h"

/* Forward declarations */
void execute_body(struct macro_head *h);

#define ATTRIB_COND 1

/************************************************************************/
/* create flags that control the behavior of the table macros like data,
 * dt,de, and dw */

#define DEFAULT_LIT_MASK 0xffff
#define ENDIAN_SWAP      (1<<31)
#define PACKING_BYTES    (1<<30)
#define SPLIT_PACK       (1<<29)

struct sllist {
  struct pnode *p;
  struct sllist *next;
};

extern struct pnode *mk_constant(int value);

/************************************************************************/
int _16bit_core = 0;         /* The 16bit core is handled differently in
			      * some instances. */
int _17cxx_core = 0;         /* 17cxx is different from 18cxx */

/************************************************************************/

/* Write a word into the memory image at the current location */

static void emit(unsigned int value)
{

  /* only write the program data to memory on the second pass */
  if (state.pass == 2) {

    if ((state.mode == relocatable) && (state.obj.section == NULL)) {
      gperror(GPE_WRONG_SECTION, NULL);
    }

    if (_17cxx_core && (state.org > 0xffff)) {  
      gperror(GPE_ADDROVF, NULL);
    } 

    if(value > state.device.core_size) {
      gpmessage(GPM_RANGE,NULL);
      value &= state.device.core_size;
    }

    if (i_memory_get(state.i_memory, state.org) & MEM_USED_MASK) {
      gperror(GPE_ADDROVR, NULL);
    }

    i_memory_put(state.i_memory, state.org, MEM_USED_MASK | value);

  }

  state.org++;

}

int endian_swap_word(int x)
{
  return (  ( (x>>8) & 0xff) |  ( (x<<8) & 0xff00));
}

/* Write a word to memory unless we're packing successive bytes into a word
 *
 * The static variable 'packed_hi_lo' acts both as a flag and a shift amount.
 * when it's zero, the next byte to be emitted is packed into the low byte
 * of the word. When it is 8, the next byte is packed into high byte of the
 * word.  This variable is used in conjunction with the data( ) routine.
 *
 * The static variable 'packed_byte' is the word into which two bytes are
 * packed. When two bytes are packed, then the word is emitted.
 */

int packed_hi_lo = 0;
static int packed_byte = 0;

static void emit_packed(unsigned int value, unsigned int mode)
{

  if(value > 255) {
    gpwarning(GPW_RANGE,NULL);
    value &= 0xff;
  }

  packed_byte |= (value << packed_hi_lo);

  if(packed_hi_lo) {

    if(mode & ENDIAN_SWAP) 
      packed_byte = endian_swap_word(packed_byte);

    emit( packed_byte);
    packed_hi_lo = packed_byte = 0;

  } else
    packed_hi_lo = 8;

}

static int off_or_on(struct pnode *p)
{
  int had_error = 0, ret = 0;

  if (p->tag != symbol)
    had_error = 1;
  else if (strcasecmp(p->value.symbol, "OFF") == 0)
    ret = 0;
  else if (strcasecmp(p->value.symbol, "ON") == 0)
    ret = 1;
  else
    had_error = 1;

  if (had_error)
     gperror(GPE_EXPECTED, "Expected \"ON\" or \"OFF\".");

  return ret;
}

static void data(struct sllist *L, int flavor, int lit_mask)
{


  if (L) {

    struct sllist *list,*previous;
    int value;

    list = L->next;
    
    while(list) {

      value = reloc_evaluate(list->p, RELOCT_ALL);

      if(lit_mask & PACKING_BYTES)
	emit_packed(value | flavor, lit_mask);
      else {
	if((value > lit_mask) || (value < 0)) {
          gpwarning(GPW_RANGE,NULL);
	}
      
        if(lit_mask & SPLIT_PACK) {
	  emit(value & 0xff);
	  emit((value >> 8) & 0xff);
        } else {
	  emit((value & lit_mask) | flavor);
        }
      }

      previous = list;
      list = list->next;
      free(previous);

    }

  }

  if(packed_hi_lo) 
    emit_packed(flavor, lit_mask);
  
}


/* If we convert to glib, we can use the built in library call g_slist_append */

struct sllist *sllist_append(struct sllist   *list,
			     struct pnode *p)
{
  struct sllist *new;

  new = malloc(sizeof(*new));
  new->p = p;
  new->next  = NULL;
  list->next = new;

  return(new);
}


/* convert an expression list which may consist of strings, constants, labels,
 * etc. into a singly linked list of integers.
 * This is called by the db,dw,dt,de, and data directives. 
 * 
 * pnode *L -        A pointer to a doubly-linked list containing all of the 
 *                   expressions.
 * sllist *list -    A pointer to a singly-linked list into which we will place
 *                   the integer values of the expressions.
 * packing_strings - A flag indicating that char strings should be packed two to
 *                   a word. Note that this applies to only the da, dw, and data 
 *                   directives. The db directive does the byte packing later.
 */

static void simplify_data(struct pnode *L, struct sllist *list, int packing_strings)
{

  if (L) {
    struct pnode *p;
    unsigned int v = 0;
    unsigned int shift;
    int value;

    p = HEAD(L);

    if (p->tag == string) {
      char *pc = p->value.string;

      if(packing_strings == 2) {
        shift = 7;
      } else {
        shift = 8;
      }
      
      while (*pc) {
	pc = convert_escape_chars(pc, &value);

	if(packing_strings) {
	  if(v>= (1<<31) ) {
	    v = (v<<shift) |  value;
            if(_16bit_core) {
              v = endian_swap_word(v);
            }   	    
	    list = sllist_append(list, mk_constant(v));
	    v = 0;
	  } else
	    v = (1<<31) | value;
	  
	} else {
	  list = sllist_append(list, mk_constant(value));
	}
      }

      if(packing_strings && v >= 1<<31 ) {
        v = v <<shift;
        if(_16bit_core) {
          v = endian_swap_word(v);
        }        
	list = sllist_append(list, mk_constant(v));
      }

    } else {
      list = sllist_append(list, p);

    }
    simplify_data(TAIL(L), list, packing_strings);

  }

}

/* Do the work for beginning a conditional assembly block.  Leave it
   disabled by default.  This is used by do_if, do_ifdef and
   do_ifndef. */

static void enter_if(void)
{
  struct amode *new = malloc(sizeof(*new));

  new->mode = in_then;
  new->prev = state.astack;
  if (state.astack == NULL)
    new->prev_enabled = 1;
  else
    new->prev_enabled = state.astack->enabled && state.astack->prev_enabled;
  new->enabled = 0;	/* Only the default */
  state.astack = new;
}

/* Checking that a macro definition's parameters are correct */

static int macro_parms_simple(struct pnode *parms)
{
  if (parms == NULL) {
    return 1;
  } else if (HEAD(parms)->tag != symbol) {
    gperror(GPE_ILLEGAL_ARGU, NULL);
    return 0;
  } else {
    return (macro_parms_simple(TAIL(parms)));
  }
}

static int list_symbol_member(struct pnode *M, struct pnode *L)
{
  if (L == NULL) {
    return 0;
  } else if (STRCMP(M->value.symbol, HEAD(L)->value.symbol) == 0) {
    char buf[BUFSIZ];
    snprintf(buf,
             sizeof(buf), 
             "Duplicate macro parameter (%s).", 
             HEAD(L)->value.symbol);
    gperror(GPE_UNKNOWN, buf);
    return 1;
  } else {
    return list_symbol_member(M, TAIL(L));
  }
}

static int macro_parms_unique(struct pnode *parms)
{
  if (parms == NULL)
    return 1;
  else
    return (!list_symbol_member(HEAD(parms), TAIL(parms)) &&
	    macro_parms_unique(TAIL(parms)));
}

static int macro_parms_ok(struct pnode *parms)
{
  return (macro_parms_simple(parms) && macro_parms_unique(parms));
}

typedef gpasmVal opfunc(gpasmVal r,
			char *name,
			int arity,
			struct pnode *parms);

/************************************************************************/

static gpasmVal do_badram(gpasmVal r,
			  char *name,
			  int arity,
			  struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = dir;
  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);
    if ((p->tag == binop) &&
	(p->value.binop.op == '-')) {
      int start, end;

      if (can_evaluate(p->value.binop.p0) &&
	  can_evaluate(p->value.binop.p1)) {
	start = evaluate(p->value.binop.p0);
	end = evaluate(p->value.binop.p1);

	if ((end < start) ||
	    (start < 0) ||
	    (MAX_RAM <= end)) {
	  gpwarning(GPW_INVALID_RAM, NULL);
	} else {
	  for (; start <= end; start++)
	    state.badram[start] = 1;
	}
      }
    } else {
      if (can_evaluate(p)) {
	int loc;

	loc = evaluate(p);
	if ((loc < 0) ||
	    (MAX_RAM <= loc)) {
	  gpwarning(GPW_INVALID_RAM, NULL);
	 } else
	  state.badram[loc] = 1;
      }
    }
  }

  return r;
}

static gpasmVal do_badrom(gpasmVal r,
			  char *name,
			  int arity,
			  struct pnode *parms)
{

  state.lst.line.linetype = dir;

  /* FIXME: implement this directive */
  gpwarning(GPW_UNKNOWN, "gpasm doesn't support the badrom directive yet"); 

  return r;
}

static void
set_bankisel(int address)
{
  if (state.device.class == PROC_CLASS_PIC14) {
    if (address < 0x100) {
      /* bcf 0x3, 0x7 */
      emit(0x1383);
    } else {
      /* bsf 0x3, 0x7 */
      emit(0x1783);
    }
  } else {
    /* movlb bank */
    emit(0xb800 | gp_processor_check_bank(state.device.class, address));
  }

}

static gpasmVal do_bankisel(gpasmVal r,
		            char *name,
		            int arity,
		            struct pnode *parms)
{
  struct pnode *p;
  int num_reloc;

  if ((state.device.class != PROC_CLASS_PIC14) &&
      (state.device.class != PROC_CLASS_PIC16)) {
    gpmessage(GPM_EXTPAGE, NULL);
    return r;
  }

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (state.mode == absolute) {
      set_bankisel(maybe_evaluate(p));
    } else {
      num_reloc = count_reloc(p);
 
      if (num_reloc == 0) {
        /* it is an absolute address, generate the bankisel but no relocation */
        set_bankisel(maybe_evaluate(p));
      } else if (num_reloc != 1) {
        gperror(GPE_UNRESOLVABLE, NULL);
      } else {
        reloc_evaluate(p, RELOCT_IBANKSEL);
        emit(0);
      }
    }
  }

  return r;
}

static gpasmVal do_banksel(gpasmVal r,
		           char *name,
		           int arity,
		           struct pnode *parms)
{
  struct pnode *p;
  int address;
  int bank;
  int num_reloc;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (state.mode == absolute) {
      address = maybe_evaluate(p);
      bank = gp_processor_check_bank(state.device.class, address);
      state.org += gp_processor_set_bank(state.device.class, 
                                         state.processor_info->num_banks,
                                         bank, 
                                         state.i_memory, 
                                         state.org);
    } else {
      num_reloc = count_reloc(p);

      if (num_reloc == 0) {
        /* it is an absolute address, generate the banksel but no relocation */
        address = maybe_evaluate(p);
        bank = gp_processor_check_bank(state.device.class, address);
        state.org += gp_processor_set_bank(state.device.class, 
                                           state.processor_info->num_banks,
                                           bank, 
                                           state.i_memory, 
                                           state.org);
      } else if (num_reloc != 1) {
        gperror(GPE_UNRESOLVABLE, NULL);
      } else if (state.device.class == PROC_CLASS_PIC16) {
        reloc_evaluate(p, RELOCT_BANKSEL);
        emit(0);
      } else if (state.device.class == PROC_CLASS_PIC16E) {
        reloc_evaluate(p, RELOCT_BANKSEL);
        emit(0);
      } else {
        switch (state.processor_info->num_banks) {
        case 2:
          reloc_evaluate(p, RELOCT_BANKSEL);
          emit(0);
          break;      
        case 4:
          reloc_evaluate(p, RELOCT_BANKSEL);
          emit(0);
          emit(0);
          break;
        }
      }
    }
  }

  return r;
}

static gpasmVal do_code(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = sec;
  state.next_state = state_section;
  
  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      strncpy(state.obj.new_sec_name, ".code", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_TEXT;
      break;
    case 1:
      /* new absolute section */
      p = HEAD(parms);
      strncpy(state.obj.new_sec_name, ".code", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p) >> _16bit_core;
      state.obj.new_sec_flags = STYP_TEXT | STYP_ABS;
      break;
    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal do_constant(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{

  struct pnode *p;
  int value;

  state.lst.line.linetype = dir;

  for (; parms; parms = TAIL(parms)) {
    p = HEAD(parms);
    if ((p->tag == binop) &&
        (p->value.binop.op == '=')) {
      if (enforce_simple(p->value.binop.p0)) {
        char *lhs;
        /* fetch the symbol */
        lhs = p->value.binop.p0->value.symbol;
        /* constants must be assigned a value at declaration */
        value = maybe_evaluate(p->value.binop.p1);
        /* put the symbol and value in the table*/
        set_global(lhs, value, PERMANENT, gvt_constant);
      }
    } else {
      gperror(GPE_ILLEGAL_ARGU, NULL);
    }
  }

  return r;
}

/*-------------------------------------------------------------------------
 *
 * configuration memory
 * 
 * In addition to memory for storing instructions, each pic has memory for
 * storing configuration data (e.g. code protection, wdt enable, etc.). Each
 * family of the pic microcontrollers treats this memory slightly different.
 *
 * do_config( ) - Called by the parser when a __CONFIG assembler directive
 *       is encountered.
 *
 */

static gpasmVal do_config(gpasmVal r,
			  char *name,
			  int arity,
			  struct pnode *parms)
{
  struct pnode *p;
  int ca;
  int value;

  state.lst.line.linetype = config;

  switch(arity) {
  case 1:
    if(_16bit_core) {
      gpwarning(GPW_EXPECTED,"18cxxx devices should specify __CONFIG address");
      ca = CONFIG1L;
    } else {
      ca = state.device.config_address;
    }
    p = HEAD(parms);
    break;

  case 2:
    ca = maybe_evaluate(HEAD(parms));
    p = HEAD(TAIL(parms));
    break;

  default:
    enforce_arity(arity,2);
    return r;
  }

  state.lst.config_address = ca;

  if (state.mode == relocatable) {
    if ((!state.found_devid) && ((ca == DEVID1) || (ca == DEVID2))) {
      coff_new_section(".devid", ca >> _16bit_core, STYP_ABS | STYP_TEXT);
      state.found_devid = true;
    } else if (!state.found_config) {
      coff_new_section(".config", ca >> _16bit_core, STYP_ABS | STYP_TEXT);
      state.found_config = true;
    }
  }

  if ((can_evaluate(p)) && (state.pass == 2)) { 
    value = evaluate(p);

    if(_16bit_core) {
      int curval = i_memory_get(state.c_memory, ca>>1);
      int mask = 0xff <<((ca&1) ? 0 : 8);

      if(value > 0xff) {
	gpwarning(GPW_RANGE,0);
      }
      /* If the config address is even, then this byte goes in LSB position */
      value = (value & 0xff) << ((ca&1) ? 8 : 0)  | MEM_USED_MASK; 

      if(curval & MEM_USED_MASK)
	curval &= mask;
      else
	curval |= mask;

      i_memory_put(state.c_memory, ca>>1,  curval | value);

    } else {
      if(value > state.device.core_size) {
        gpmessage(GPM_RANGE,NULL);
        value &= state.device.core_size;
      }

      if (i_memory_get(state.c_memory, ca) & MEM_USED_MASK) {
        gperror(GPE_ADDROVR, NULL);
      }

      i_memory_put(state.c_memory, ca, MEM_USED_MASK | value);
      /* FIXME: need line_number? this one will be wrong coff_linenum(1) */
    }

    /* force the section to end */
    state.obj.section = NULL;
  }
        
  return r;
}

/*-------------------------------------------------------------------------
 *  do_da - The 'da' directive. Generates a number representing two
 *          7 bit ascii characters. It can be used in place of the DATA
 *          directive for 14 bit cores to pack two characters into one
 *          word. 
 */
static gpasmVal do_da(gpasmVal r,
		      char *name,
		      int arity,
		      struct pnode *parms)
{

  struct sllist list;

  list.next = NULL;

  if(_16bit_core || state.device.core_size == CORE_12BIT_MASK) {
    simplify_data(parms, &list, 1);
  } else {
    simplify_data(parms, &list, 2);
  }

  data(&list, 0, DEFAULT_LIT_MASK);

  return r;
}

/*-------------------------------------------------------------------------
 * do_data - The 'data' directive. Initialize one or more words of program
 *           memory with data. On all families except the pic18cxxx, the 
 *           first character is in the most significant byte of the word.
 */

static gpasmVal do_data(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{

  struct sllist list;

  list.next = NULL;
  simplify_data(parms, &list, 1);

  if ((state.mode == relocatable) &&
      !(SECTION_FLAGS & STYP_TEXT)) {
    /* This is a data memory not program */
    state.lst.line.linetype = res;

    /* data memory is byte sized so split the data */  
    data(&list, 0, SPLIT_PACK);
  } else {
    data(&list, 0, DEFAULT_LIT_MASK);
  } 

  return r;
}
/*-------------------------------------------------------------------------
 * do_db  - Reserve program memory words with packed 8-bit values. On the
 *          18cxxx families, dw and db are the same. For the 12 and 14 bit
 *          cores, the upper bits are masked (e.g. the 14-bit core can only
 *          store 14bits at a given program memory address, so the upper 2
 *          in a db directive are meaningless.
 */


static gpasmVal do_db(gpasmVal r,
		      char *name,
		      int arity,
		      struct pnode *parms)
{

  struct sllist list;

  list.next = NULL;
  simplify_data(parms, &list,0);

  if ((state.mode == relocatable) &&
      !(SECTION_FLAGS & STYP_TEXT)) {
    /* This is a data memory not program */
    state.lst.line.linetype = res;
 
    /* only valid in initialized data sections */
    if (SECTION_FLAGS & STYP_BSS)
      gperror(GPE_WRONG_SECTION, NULL);

    data(&list, 0, 0xff);
  } else {
    if(_16bit_core) {
      data(&list, 0, PACKING_BYTES);
    } else {
      data(&list, 0, ENDIAN_SWAP | PACKING_BYTES);
    }    
  } 

  return r;
}

static gpasmVal do_de(gpasmVal r,
		      char *name,
		      int arity,
		      struct pnode *parms)
{

  struct sllist list;

  list.next = NULL;
  simplify_data(parms, &list,0);
  if(_16bit_core) {
    data(&list, 0, PACKING_BYTES);
  } else {
    data(&list, 0, 0xff);
  }  


  return r;
}

static gpasmVal do_def(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  struct pnode *p;
  char *symbol_name = NULL;
  gp_symbol_type *coff_symbol = NULL;
  int shift = 0;
  int eval;
  int value = 0;
  gp_boolean new_class = false;
  int coff_class = C_NULL;
  gp_boolean new_type = false;
  int coff_type = T_NULL;
  enum gpasmValTypes type = gvt_debug;

  state.lst.line.linetype = dir;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    if (arity < 2) { 
      enforce_arity(arity, 2);
      return r;
    }

    /* the first argument is the symbol name */
    p = HEAD(parms);
    if (enforce_simple(p)) {
      symbol_name = p->value.symbol;
    } else {
      return r;
    }
    parms = TAIL(parms);

    if ((SECTION_FLAGS & STYP_TEXT) && _16bit_core) {
      shift = 1;
    }

    /* update the properties */
    for (; parms; parms = TAIL(parms)) {
      p = HEAD(parms);
      if ((p->tag == binop) &&
	  (p->value.binop.op == '=')) {
        if (enforce_simple(p->value.binop.p0)) {
	  char *lhs;

	  lhs = p->value.binop.p0->value.symbol;
          if (strcasecmp(lhs, "value") == 0) {
            value = maybe_evaluate(p->value.binop.p1);   
    	  } else if (strcasecmp(lhs, "size") == 0) {
            state.org += (maybe_evaluate(p->value.binop.p1) >> shift);   
    	  } else if (strcasecmp(lhs, "type") == 0) {
            eval = maybe_evaluate(p->value.binop.p1);   
            if ((eval < 0) || (eval > 0xffff)) {
              gperror(GPE_RANGE, NULL);
            } else {
              new_type = true;
              coff_type = eval;   
            }
    	  } else if (strcasecmp(lhs, "class") == 0) {
            eval = maybe_evaluate(p->value.binop.p1);   
            if ((eval < -128) || (eval > 127)) {
              gperror(GPE_RANGE, NULL);
            } else {
              new_class = true;
              coff_class = eval;   
            }
          } else {
            gperror(GPE_ILLEGAL_ARGU, NULL);
          }
        }
      } else {
        if (enforce_simple(p)) {
	  if (strcasecmp(p->value.symbol, "absolute") == 0) {
            type = gvt_absolute;
            value = 0;   
  	  } else if (strcasecmp(p->value.symbol, "debug") == 0) {
            type = gvt_debug;
            value = 0;   
	  } else if (strcasecmp(p->value.symbol, "extern") == 0) {
            type = gvt_extern;
            value = 0;   
  	  } else if (strcasecmp(p->value.symbol, "global") == 0) {
            type = gvt_global;
            value = state.org << shift;
  	  } else if (strcasecmp(p->value.symbol, "static") == 0) {
            type = gvt_static;
            value = state.org << shift;
  	  } else {
            gperror(GPE_ILLEGAL_ARGU, NULL);
          }
        }
      }
    }
  }

  set_global(symbol_name, value, PERMANENT, type);

  /* update the symbol with the values */
  if ((state.pass == 2) && (new_class || new_type)) {
    coff_symbol = gp_coffgen_findsymbol(state.obj.object, symbol_name);
    assert(coff_symbol != NULL);
    if (new_class)
      coff_symbol->class = coff_class;
      
    if (new_type)
      coff_symbol->type = coff_type;
  }

  return r;
}

static gpasmVal do_dim(gpasmVal r,
                       char *name,
                       int arity,
                       struct pnode *parms)
{
  struct pnode *p;
  char *symbol_name = NULL;
  gp_symbol_type *coff_symbol = NULL;
  int number_symbols;
  gp_aux_type *aux_list;
  struct sllist first_list;
  struct sllist *list = &first_list;
  struct sllist *previous;
  int i;
  int value;

  state.lst.line.linetype = dir;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    if (arity < 3) { 
      enforce_arity(arity, 1);
      return r;
    }
    
    /* the first argument is the symbol name */
    p = HEAD(parms);
    if (enforce_simple(p)) {
      /* lookup the symbol */
      symbol_name = p->value.symbol;
      coff_symbol = gp_coffgen_findsymbol(state.obj.object, symbol_name);
      if (coff_symbol == NULL) {
        gperror(GPE_NOSYM, NULL);
        return r;
      }
    } else {
      return r;
    }
    parms = TAIL(parms);

    /* the second argument must be the number of aux symbols */
    p = HEAD(parms);
    number_symbols = maybe_evaluate(p);
    if ((number_symbols < 0) || (number_symbols > 127)) {
      gperror(GPE_UNKNOWN, "number of auxiliary symbols must be less then 128 and positive"); 
      return r;
    }
    state.obj.symbol_num += number_symbols;
    parms = TAIL(parms);

    /* create the symbols */
    aux_list = gp_coffgen_blockaux(number_symbols);
    coff_symbol->num_auxsym = number_symbols;
    coff_symbol->aux_list = aux_list;

    /* convert the arguments into a list of values */
    list->next = NULL;
    simplify_data(parms, list, 0);

    /* write the data to the auxiliary symbols */
    list = list->next;
    i = 0;
    while(list) {
      value = maybe_evaluate(list->p);
      if (value & (~0xff)) {
        gperror(GPE_RANGE, NULL); 
        return r;
      }
      
      if (aux_list == NULL) {
        gperror(GPE_UNKNOWN, "insufficent number of auxiliary symbols"); 
        return r;
      }
      
      if (i == SYMBOL_SIZE) {
        i = 0;
        aux_list = aux_list->next;      
      } else {
        aux_list->_aux_symbol.data[i++] = value;
      }

      previous = list;
      list = list->next;
      free(previous);
    }
  }

  return r;
}

static gpasmVal do_direct(gpasmVal r,
		          char *name,
		          int arity,
		          struct pnode *parms)
{

  struct pnode *p;
  int value;
  unsigned char direct_command = 0;
  char *direct_string = NULL;

  state.lst.line.linetype = dir;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else if (enforce_arity(arity, 2)) {
    p = HEAD(parms);
    value = maybe_evaluate(p);
    if ((value < 0) || (value > 255)) {
      gperror(GPE_RANGE, NULL);
    } else {
      direct_command = value;   
    }

    p= HEAD(TAIL(parms));
    if (p->tag == string) {
      if (strlen(p->value.string) < 255) {
        direct_string = p->value.string;
      } else {
        gperror(GPE_UNKNOWN, "string must be less than 255 bytes long");
      }
    } else {
      gperror(GPE_ILLEGAL_ARGU, NULL);
    }  

    if (direct_string == NULL) {
      return r;
    }

    if (SECTION_FLAGS & STYP_TEXT) {
      coff_add_directsym(direct_command, direct_string);
    } else {
      gperror(GPE_WRONG_SECTION, NULL);
    }
  
  }

  return r;
}

static gpasmVal do_dt(gpasmVal r,
		      char *name,
		      int arity,
		      struct pnode *parms)
{
  struct symbol *s;
  struct insn *i;

  struct sllist list;


  s = get_symbol(state.stBuiltin, "RETLW");
  assert(s != NULL); /* Every PIC has a RETLW instruction */
  i = get_symbol_annotation(s);

  list.next = NULL;
  simplify_data(parms, &list, 0);
  data(&list, i->opcode, 0xff);

  return r;
}

/*-------------------------------------------------------------------------
 *  do_dw - The 'dw' directive. On all families except for the p18cxxx, the
 *          dw directive is the same as the 'data' directive. For the p18cxxx
 *          it's the same as the 'db' directive. (That's strange, but it's
 *          also the way mpasm does it).
 */
static gpasmVal do_dw(gpasmVal r,
		      char *name,
		      int arity,
		      struct pnode *parms)
{

  struct sllist list;

  list.next = NULL;
  simplify_data(parms, &list, 1);

  if ((state.mode == relocatable) &&
      !(SECTION_FLAGS & STYP_TEXT)) {
    /* This is a data memory not program */
    state.lst.line.linetype = res;

    /* only valid in initialized data sections */
    if (SECTION_FLAGS & STYP_BSS)
      gperror(GPE_WRONG_SECTION, NULL);

    /* data memory is byte sized so split the data */  
    data(&list, 0, SPLIT_PACK);
  } else {
    data(&list, 0, DEFAULT_LIT_MASK);
  }
  

  return r;
}


static gpasmVal do_else(gpasmVal r,
			char *name,
			int arity,
			struct pnode *parms)
{
  state.lst.line.linetype = dir;
  if (state.astack == NULL)
    gperror(GPE_ILLEGAL_COND, NULL);
  else if ((state.astack->mode != in_then))
    gperror(GPE_ILLEGAL_COND, NULL);
  else
    state.astack->enabled = !state.astack->enabled;

  return r;
}

static gpasmVal do_endif(gpasmVal r,
			 char *name,
			 int arity,
			 struct pnode *parms)
{
  state.lst.line.linetype = dir;
  if (state.astack == NULL) {
    gperror(GPE_ILLEGAL_COND, "Illegal condition (ENDIF).");
  } else if ((state.astack->mode != in_then) &&
	     (state.astack->mode != in_else)) {
    gperror(GPE_ILLEGAL_COND, "Illegal condition (ENDIF).");
  } else {
    struct amode *old;

    old = state.astack;
    state.astack = state.astack->prev;
    free(old);
  }
  return r;
}

static gpasmVal do_endm(gpasmVal r,
			char *name,
			int arity,
			struct pnode *parms)
{
  assert(!state.mac_head);
  state.lst.line.linetype = dir;
  if (state.mac_prev == NULL)
    gperror(GPE_UNMATCHED_ENDM, NULL);
  else
    state.mac_prev = NULL;

  state.mac_body = NULL;

  return r;
}

static gpasmVal do_endw(gpasmVal r,
			char *name,
			int arity,
			struct pnode *parms)
{
  state.lst.line.linetype = dir;

  assert(!state.mac_head);
  if (state.mac_prev == NULL) {
    gperror(GPE_ILLEGAL_COND, "Illegal condition (ENDW).");
  } else if (maybe_evaluate(state.while_head->parms)) {
    state.next_state = state_while;  
    state.next_buffer.macro = state.while_head;
  }

  state.mac_body = NULL;
  state.mac_prev = NULL;
  state.while_head = NULL;

  return r;
}

static gpasmVal do_eof(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{
  state.lst.line.linetype = dir;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else if (enforce_arity(arity, 0)) {
    if (state.debug_info) { 
      coff_add_eofsym();
    } else {
      gpwarning(GPW_UNKNOWN, "directive ignored when debug info is disabled");
    }
  }

  return r;
}

static gpasmVal do_equ(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{
  struct pnode *p;
  state.lst.line.linetype = equ;
  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (can_evaluate(p))
      r = evaluate(p);
  }

  return r;
}

static gpasmVal do_error(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{
  struct pnode *p;
  state.lst.line.linetype = dir;
  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (p->tag == string) {
      gperror(GPE_USER, p->value.string);
    } else {
      gperror(GPE_ILLEGAL_ARGU, NULL);
    }
  }

  return r;
}

/************************************************************************
 * do_errlvl - parse the ERRORLEVEL directive
 *
 * If the file gpasm is assembling contains a ERRORLEVEL directive, then scan
 * the comma delimited list of options in *parms
 *
 * Inputs:
 *   gpasmVal r - not used, but is returned
 *   char *name - not used, but contains the directive name 'list'.
 *   int arity - not used, but should contain '1'
 *   struct pnode *parms - a linked list of the parsed parameters
 *
 *
 *
 ************************************************************************/

static gpasmVal do_errlvl(gpasmVal r,
                          char *name,
                          int arity,
                          struct pnode *parms)
{
  struct pnode *p;
  state.lst.line.linetype = dir;

  if (state.pass == 2) {
    for (; parms; parms = TAIL(parms)) {
      p = HEAD(parms);
      if (p->tag == unop)  {
        gpasmVal value = evaluate(p->value.unop.p0);
       
        if (p->value.unop.op == '-') {
          add_code(-value);
        } else if (p->value.unop.op == '+') {
          add_code(value);	    
        } else {
          gperror(GPE_ILLEGAL_ARGU, "Expected 0, 1, 2, +|-<message number>");
        }
      } else if (p->tag == constant) {
        select_errorlevel(p->value.constant);
      } else {
        gperror(GPE_ILLEGAL_ARGU, "Expected 0, 1, 2, +|-<message number>");
      }
    }
  }

  return r;
}

static gpasmVal do_exitm(gpasmVal r,
			 char *name,
			 int arity,
			 struct pnode *parms)
{
  state.lst.line.linetype = dir;
  if (enforce_arity(arity, 0)) {
    if (state.stGlobal == state.stTop) {
      gperror(GPE_UNKNOWN, "Attempt to use \"exitm\" outside of macro");
    } else {
      state.next_state = state_exitmacro;
    }
  }

  return r;
}

static gpasmVal do_expand(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{
  state.lst.line.linetype = dir;
  if (state.cmd_line.macro_expand) {
    gpmessage(GPM_SUPLIN, NULL);
  } else {
    if (enforce_arity(arity, 0)) {
      state.lst.expand = true;
    }
  }
  return r;
}

static gpasmVal do_extern(gpasmVal r,
		          char *name,
		          int arity,
		          struct pnode *parms)
{
  char *p;
  state.lst.line.linetype = dir;
  
  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    for (; parms; parms = TAIL(parms)) {
      p = maybe_evaluate_concat(HEAD(parms));
      if (p) {
        set_global(p, 0, PERMANENT, gvt_extern);
      }
    }
  }

  return r;
}

static gpasmVal do_file(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{
  struct pnode *p;
  state.lst.line.linetype = dir;
  
  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else if (enforce_arity(arity, 1)) {
    if (state.debug_info) {
      p = HEAD(parms);
      if (p->tag == string) {
        state.obj.debug_file = coff_add_filesym(p->value.string, 0);
      } else {
        gperror(GPE_ILLEGAL_ARGU, NULL);
      }
    } else {
      gpwarning(GPW_UNKNOWN, "directive ignored when debug info is disabled");
    }
  }

  return r;
}

/* Filling constants is handled here. Filling instructions is handled
   in the parser. */ 
   
static gpasmVal do_fill(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{
  struct pnode *h;
  int number;
  int i;

  if (enforce_arity(arity, 2)) {
    h = HEAD(parms);
    number = eval_fill_number(HEAD(TAIL(parms)));

    for (i = 1; i <= number ; i += 1) {
      /* we must evaluate each loop, because some symbols change (i.e. $) */
      emit(maybe_evaluate(h));
    }
  }
  return r;
}

static gpasmVal do_global(gpasmVal r,
		          char *name,
		          int arity,
		          struct pnode *parms)
{
  char *p;
  char buf[BUFSIZ];
  struct symbol *s;
  struct variable *var;

  state.lst.line.linetype = dir;
  
  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    for (; parms; parms = TAIL(parms)) {
      p = maybe_evaluate_concat(HEAD(parms));
      if (p) {
        s = get_symbol(state.stTop, p);
        if (s == NULL) {
          snprintf(buf,
                   sizeof(buf),
                   "Symbol not previously defined (%s).",
                   p);
          gperror(GPE_NOSYM, buf);    
	} else {
          var = get_symbol_annotation(s);
          if (var == NULL) {
            snprintf(buf,
                     sizeof(buf),
                     "Symbol not assigned a value (%s).",
                     p);
            gpwarning(GPW_UNKNOWN, buf);    
          } else {
            if ((var->previous_type == gvt_address) || 
                (var->previous_type == gvt_global)  ||
                (var->previous_type == gvt_static)) {
              /* make the symbol global */
              var->type = gvt_global;
            } else if (var->previous_type == gvt_extern) { 
              gperror(GPE_DUPLAB, NULL);    
            } else {
              snprintf(buf,
                       sizeof(buf),
                       "Operand must be an address label (%s).", 
                       p);
              gperror(GPE_MUST_BE_LABEL, buf);    
            } 
          }         
        }  
      } else {
        gperror(GPE_ILLEGAL_ARGU, NULL);
      }
    }  
  }

  return r;
}

static gpasmVal do_idata(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = sec;
  state.next_state = state_section;
  
  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      strncpy(state.obj.new_sec_name, ".idata", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_DATA;
      break;
    case 1:
      /* new absolute section */
      p = HEAD(parms);
      strncpy(state.obj.new_sec_name, ".idata", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p) >> _16bit_core;
      state.obj.new_sec_flags = STYP_DATA | STYP_ABS;
      break;
    default:
      enforce_arity(arity, 1);
    }  
  }

  return r;
}

static gpasmVal do_ident(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{
  struct pnode *p;
  state.lst.line.linetype = dir;
  
  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (p->tag == string) {
      coff_add_identsym(p->value.string);
    } else {
      gperror(GPE_ILLEGAL_ARGU, NULL);
    }
  }

  return r;
}

static gpasmVal do_idlocs(gpasmVal r,
		          char *name,
		          int arity,
		          struct pnode *parms)
{
  int value;
  int idreg;
  int curvalue;
  int mask;

  if (_17cxx_core) {
    gperror(GPE_ILLEGAL_DIR, NULL);
    return r;
  }

  if (_16bit_core) {
    if (enforce_arity(arity,2)) {
      idreg = maybe_evaluate(HEAD(parms));
      value = maybe_evaluate(HEAD(TAIL(parms)));
    } else {
      gperror(GPW_EXPECTED,"18cxxx devices should specify __IDLOC address");
      return r;
    }
  } else {
    if (enforce_arity(arity,1)) {
      idreg = state.device.id_location;
      value = maybe_evaluate(HEAD(parms));
    } else {
      return r;
    }
  }

  if ((state.mode == relocatable) && (!state.found_idlocs)) {
    coff_new_section(".idlocs", idreg >> _16bit_core, STYP_ABS | STYP_TEXT);
    state.found_idlocs = true;
  }

  state.lst.config_address = idreg;
  state.device.id_location = idreg;

  if (state.pass == 2) {
    if (_16bit_core) {
      state.lst.line.linetype = config;

      if (idreg > IDLOC7 || idreg < IDLOC0) {
        gperror(GPE_RANGE,NULL);
      } else {
        if(value > 0xff) {
          gpwarning(GPW_RANGE, NULL);
        }
        curvalue = i_memory_get(state.c_memory, idreg>>1);
        mask = 0xff <<((idreg&1) ? 0 : 8);
	
        /* If the address is even, then this byte goes in LSB position */
        value = (value & 0xff) << ((idreg&1) ? 8 : 0)  | MEM_USED_MASK; 

        if(curvalue & MEM_USED_MASK)
          curvalue &= mask;
        else
          curvalue |= mask;

        i_memory_put(state.c_memory, idreg>>1,  curvalue | value);
      }

    } else {
      state.lst.line.linetype = idlocs;

      if (value > 0xffff) {
        gpmessage(GPM_IDLOC, NULL);
        value &= 0xffff;
      }     
      
      if (i_memory_get(state.c_memory, idreg) & MEM_USED_MASK) {
        gperror(GPE_ADDROVR, NULL);
      }

      i_memory_put(state.c_memory, idreg,     
                   ((value & 0xf000) >> 12) | MEM_USED_MASK);
      i_memory_put(state.c_memory, idreg + 1,
                   ((value & 0x0f00) >> 8) | MEM_USED_MASK);
      i_memory_put(state.c_memory, idreg + 2,
                   ((value & 0x00f0) >> 4) | MEM_USED_MASK);
      i_memory_put(state.c_memory, idreg + 3,
                   (value & 0x000f) | MEM_USED_MASK);

    }
  }
  
  return r;
}

static gpasmVal do_if(gpasmVal r,
		      char *name,
		      int arity,
		      struct pnode *parms)
{
  struct pnode *p;
  state.lst.line.linetype = dir;
  enter_if();

  /* Only evaluate the conditional if it matters... */
  if (state.astack->prev_enabled) {
    if (enforce_arity(arity, 1)) {
      p = HEAD(parms);
      state.astack->enabled = maybe_evaluate(p);
    }
  }

  return r;
}

static gpasmVal do_ifdef(gpasmVal r,
			 char *name,
			 int arity,
			 struct pnode *parms)
{
  struct pnode *p;
  state.lst.line.linetype = dir;
  enter_if();

  /* Only evaluate the conditional if it matters... */
  if (state.astack->prev_enabled) {
    if (enforce_arity(arity, 1)) {
      p = HEAD(parms);
      if (p->tag != symbol) {
	gperror(GPE_ILLEGAL_LABEL, NULL);
      } else {
	if ((get_symbol(state.stDefines, p->value.symbol)) ||
            (get_symbol(state.stTop, p->value.symbol)))
	  state.astack->enabled = 1;
      }
    }
  }

  return r;
}

static gpasmVal do_ifndef(gpasmVal r,
			  char *name,
			  int arity,
			  struct pnode *parms)
{
  struct pnode *p;
  state.lst.line.linetype = dir;
  enter_if();

  /* Only evaluate the conditional if it matters... */
  if (state.astack->prev_enabled) {
    if (enforce_arity(arity, 1)) {
      p = HEAD(parms);
      if (p->tag != symbol) {
	gperror(GPE_ILLEGAL_LABEL, NULL);
      } else {
	if ((!get_symbol(state.stDefines, p->value.symbol)) &&
            (!get_symbol(state.stTop, p->value.symbol)))
	  state.astack->enabled = 1;
      }
    }
  }

  return r;
}

static gpasmVal do_include(gpasmVal r,
		           char *name,
		           int arity,
		           struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = dir;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (p->tag == string) {
      state.next_state = state_include;  
      state.next_buffer.file = strdup(p->value.string);
    } else {
      gperror(GPE_ILLEGAL_ARGU, NULL);
    }
  }

  return r;
}

static gpasmVal do_line(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = dir;

  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else if (enforce_arity(arity, 1)) {
    if (state.debug_info) {
      p = HEAD(parms);
      state.obj.debug_line = maybe_evaluate(p);
    } else {
      gpwarning(GPW_UNKNOWN, "directive ignored when debug info is disabled");
    }
  }

  return r;
}

/************************************************************************
 * do_list - parse the LIST directive
 *
 * If the file gpasm is assembling contains a LIST directive, then scan
 * and parse will call do_list and pass the comma delimited list of LIST
 * options in *parms
 *
 * Inputs:
 *   gpasmVal r - not used, but is returned
 *   char *name - not used, but contains the directive name 'list'.
 *   int arity - not used, but should contain '1'
 *   struct pnode *parms - a linked list of the parsed parameters
 *
 *
 *
 ************************************************************************/

static gpasmVal do_list(gpasmVal r,
			char *name,
			int arity,
			struct pnode *parms)
{
  struct pnode *p;

  state.lst.enabled = true;
  state.lst.line.linetype = dir;

  for (; parms; parms = TAIL(parms)) {
    p = HEAD(parms);
    if ((p->tag == binop) &&
	(p->value.binop.op == '=')) {
      if (enforce_simple(p->value.binop.p0)) {
	char *lhs;

	lhs = p->value.binop.p0->value.symbol;
	if (strcasecmp(lhs, "b") == 0) {
	  int b;

	  b = maybe_evaluate(p->value.binop.p1);
	  if (b != 0)
	    state.lst.tabstop = b;
	} else if (strcasecmp(lhs, "c") == 0) {
	  ; /* Ignore this for now: column width not used */
	} else if (strcasecmp(lhs, "f") == 0) { 
	  if (enforce_simple(p->value.binop.p1))
            select_hexformat(p->value.binop.p1->value.symbol);
	} else if (strcasecmp(lhs, "l") == 0) {
	  ; /* Ignore this for now: page length */
	} else if (strcasecmp(lhs, "mm") == 0) {
	  state.lst.memorymap = off_or_on(p->value.binop.p1);
	} else if (strcasecmp(lhs, "n") == 0) {
          if (can_evaluate(p->value.binop.p1)) {
            int number = evaluate(p->value.binop.p1);

            if ((number > 6) || (number == 0)) {
              state.lst.linesperpage = number;
            } else {
              gperror(GPE_RANGE, NULL);
            } 
          }
	} else if (strcasecmp(lhs, "p") == 0) {
	  if (enforce_simple(p->value.binop.p1))
	    select_processor(p->value.binop.p1->value.symbol);
	} else if (strcasecmp(lhs, "pe") == 0) {
          state.extended_pic16e = true;
	  if (enforce_simple(p->value.binop.p1))
	    select_processor(p->value.binop.p1->value.symbol);
	} else if (strcasecmp(lhs, "r") == 0) {
	  if (enforce_simple(p->value.binop.p1))
	    select_radix(p->value.binop.p1->value.symbol);
	} else if (strcasecmp(lhs, "st") == 0) {
	  state.lst.symboltable = off_or_on(p->value.binop.p1);
	} else if (strcasecmp(lhs, "t") == 0) {
	  ; /* Ignore this for now: always wrap long list lines */
	} else if (strcasecmp(lhs, "w") == 0) {
          select_errorlevel(maybe_evaluate(p->value.binop.p1));
	} else if (strcasecmp(lhs, "x") == 0) {
	  if (enforce_simple(p->value.binop.p1))
            select_expand(p->value.binop.p1->value.symbol);
        } else {
          gperror(GPE_ILLEGAL_ARGU, NULL);
        }
      }
    } else {
      if (enforce_simple(p)) {
	if (strcasecmp(p->value.symbol, "free") == 0) {
	  ; /* Ignore this directive */
	} else if (strcasecmp(p->value.symbol, "fixed") == 0) {
	  ; /* Ignore this directive */
	} else if (strcasecmp(p->value.symbol, "nowrap") == 0) {
	  ; /* Ignore this directive */
	} else if (strcasecmp(p->value.symbol, "wrap") == 0) {
	  ; /* Ignore this directive */
	} else {
          gperror(GPE_ILLEGAL_ARGU, NULL);
        }
      }
    }
  }

  /* The .list symbol is only added to the COFF file if its only action is to turn on
     the listing */
  if (arity == 0)
    coff_add_listsym();

  return r;
}

static gpasmVal do_local(gpasmVal r,
			 char *name,
			 int arity,
			 struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = dir;

  /* like variable except it is put in TOP instead of GLOBAL */

  if (state.stGlobal == state.stTop) {
    gperror(GPE_UNKNOWN, "Attempt to use \"local\" outside of macro");
  } else {
    for (; parms; parms = TAIL(parms)) {
      p = HEAD(parms);
      if ((p->tag == binop) &&
          (p->value.binop.op == '=')) {
        if (enforce_simple(p->value.binop.p0)) {
          char *lhs;
          gpasmVal value;
          /* fetch the symbol */
          lhs = p->value.binop.p0->value.symbol;
          value = maybe_evaluate(p->value.binop.p1);
          /* put the symbol and value in the TOP table*/
          add_symbol(state.stTop, lhs);
          set_global(lhs, value, TEMPORARY, gvt_constant);
        }
      } else if (p->tag == symbol) {
        /* put the symbol in the Top table */
        add_symbol(state.stTop, p->value.symbol);
      } else {
        gperror(GPE_ILLEGAL_ARGU, NULL);
      }
    }
  }

  return r;
}

static gpasmVal do_noexpand(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{
  state.lst.line.linetype = dir;
  if (state.cmd_line.macro_expand) {
    gpmessage(GPM_SUPLIN, NULL);
  } else {
    if (enforce_arity(arity, 0)) {
      state.lst.expand = false;
    }
  }
  return r;
}

static gpasmVal do_nolist(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{
  state.lst.line.linetype = dir;
  
  if (!state.lst.force)
    state.lst.enabled = false;
    
  coff_add_nolistsym();
  
  return r;
}

static gpasmVal do_maxram(gpasmVal r,
			  char *name,
			  int arity,
			  struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = dir;
  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (can_evaluate(p))
      state.maxram = evaluate(p);
  }

  return r;
}

static gpasmVal do_maxrom(gpasmVal r,
			  char *name,
			  int arity,
			  struct pnode *parms)
{

  state.lst.line.linetype = dir;

  /* FIXME: implement this directive */
  gpwarning(GPW_UNKNOWN, "gpasm doesn't support the maxrom directive yet"); 

  return r;
}

static gpasmVal do_macro(gpasmVal r,
			 char *name,
			 int arity,
			 struct pnode *parms)
{
  struct macro_head *head = malloc(sizeof(*head));

  head->parms = parms;
  head->body = NULL;
  head->defined = 0;
  /* Record data for the list, cod, and coff files */
  head->line_number = state.src->line_number;
  head->file_symbol = state.src->file_symbol;

  head->src_name = strdup(state.src->name);
  
  state.lst.line.linetype = dir;
  if (macro_parms_ok(parms))
    state.mac_head = head;

  state.mac_prev = &(head->body);
  state.mac_body = NULL;

  return r;
}

static gpasmVal do_messg(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{
  struct pnode *p;
  state.lst.line.linetype = dir;
  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (p->tag == string) {
      gpmessage(GPM_USER, p->value.string);
    } else {
      gperror(GPE_ILLEGAL_ARGU, NULL);
    }
  }

  return r;
}

static gpasmVal do_org(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = org;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (can_evaluate(p)) {
      r = evaluate(p) >> _16bit_core;
      if (state.mode == absolute) {
        state.org = r;
      }	else {
        /* Default section name, this will be overwritten if a label is 
           present. */
        snprintf(state.obj.new_sec_name, sizeof(state.obj.new_sec_name), ".org_%d", state.obj.org_num++);
        state.obj.new_sec_addr = r;
        state.obj.new_sec_flags = STYP_TEXT | STYP_ABS;
        state.lst.line.linetype = sec;
        state.next_state = state_section;
      }
    }
  }

  return r;
}

static gpasmVal do_page(gpasmVal r,
			char *name,
			int arity,
			struct pnode *parms)
{
  state.lst.line.linetype = dir;
  if (enforce_arity(arity, 0))
    lst_throw();

  return r;
}

static gpasmVal do_pagesel(gpasmVal r,
		           char *name,
		           int arity,
		           struct pnode *parms)
{
  struct pnode *p;
  int address;
  int page;
  int num_reloc;
  
  if ((state.device.class == PROC_CLASS_EEPROM8) ||
      (state.device.class == PROC_CLASS_PIC16E)) {
    /* do nothing */
    return r;
  } else if (state.device.class == PROC_CLASS_PIC16) {
    gpmessage(GPM_W_MODIFIED, NULL);
  }

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (state.mode == absolute) {
      address = maybe_evaluate(p);
      page = gp_processor_check_page(state.device.class, address);
      state.org += gp_processor_set_page(state.device.class, 
                                         state.processor_info->num_pages,
                                         page, 
                                         state.i_memory, 
                                         state.org);
    } else {
      num_reloc = count_reloc(p);
      
      if (num_reloc == 0) {
        /* it is an absolute address, generate the pagesel but no relocation */
        address = maybe_evaluate(p);
        page = gp_processor_check_page(state.device.class, address);

        state.org += gp_processor_set_page(state.device.class, 
                                           state.processor_info->num_pages,
                                           page, 
                                           state.i_memory, 
                                           state.org);
      } else if (num_reloc != 1) {
        gperror(GPE_ILLEGAL_LABEL, NULL);
      } else if (state.device.class == PROC_CLASS_PIC16) {
        reloc_evaluate(p, RELOCT_PAGESEL_WREG);
        emit(0);
        emit(0);
      } else {
        switch (state.processor_info->num_pages) {
        case 2:
          reloc_evaluate(p, RELOCT_PAGESEL_BITS);
          emit(0);
          break;      
        case 4:
          reloc_evaluate(p, RELOCT_PAGESEL_BITS);
          emit(0);
          emit(0);
          break;
        }
      }
    }

  }

  return r;
}

static gpasmVal do_processor(gpasmVal r,
			     char *name,
			     int arity,
			     struct pnode *parms)
{
  state.lst.line.linetype = dir;
  if (enforce_arity(arity, 1)) {
    struct pnode *p = HEAD(parms);
    
    if (enforce_simple(p))
      select_processor(p->value.symbol);
  }

  return r;
}

static gpasmVal do_radix(gpasmVal r,
			 char *name,
			 int arity,
			 struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = dir;
  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (enforce_simple(p)) {
      select_radix(p->value.symbol);
    }
  }

  return r;
}

static gpasmVal do_res(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{
  struct pnode *p;
  int count;
  int i;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (can_evaluate(p)) {
      count = evaluate(p);
      r = state.org;

      if (state.mode == absolute) {
        state.lst.line.linetype = equ;
        state.org += (count >> _16bit_core);
      } else {
        state.lst.line.linetype = res;
        if (SECTION_FLAGS & STYP_TEXT)
          count >>= _16bit_core;

        for (i = 0; i < count; i++) {
          if (SECTION_FLAGS & STYP_TEXT) {
            /* For some reason program memory is filled with a different 
               value. */
            emit(state.device.core_size);     
          } else {
            emit(0);     
          }
        }
      }
    }
  }

  return r;
}

static gpasmVal do_set(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = set;
  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (can_evaluate(p))
      r = evaluate(p);
  }

  return r;
}

static gpasmVal do_space(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = dir;

  if (state.pass == 2) {
    switch (arity) {
      case 0:
        /* do nothing */
        break;
      case 1:
        p = HEAD(parms);
        if (can_evaluate(p)) {
	  int i;
	
	  for (i = evaluate(p); i > 0; i--)
	    lst_line("");
        }
        break;
      default:
        enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal do_type(gpasmVal r,
		        char *name,
		        int arity,
		        struct pnode *parms)
{
  struct pnode *p;
  char *symbol_name = NULL;
  gp_symbol_type *coff_symbol = NULL;
  int value;

  state.lst.line.linetype = dir;
  
  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else if (enforce_arity(arity, 2)) {
    /* the first argument is the symbol name */
    p = HEAD(parms);
    if (enforce_simple(p)) {
      symbol_name = p->value.symbol;
      coff_symbol = gp_coffgen_findsymbol(state.obj.object, symbol_name);
      if (coff_symbol == NULL) {
        gperror(GPE_NOSYM, NULL);
      } else {
        p = HEAD(TAIL(parms));
        value = maybe_evaluate(p);   
        if ((value < 0) || (value > 0xffff)) {
          gperror(GPE_RANGE, NULL);
        } else {
          coff_symbol->type = value;   
        }
      }
    }
  }

  return r;
}

static gpasmVal do_udata(gpasmVal r,
		         char *name,
		         int arity,
		         struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = sec;
  state.next_state = state_section;
  
  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      strncpy(state.obj.new_sec_name, ".udata", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_BSS;
      break;
    case 1:
      /* new absolute section */
      p = HEAD(parms);
      strncpy(state.obj.new_sec_name, ".udata", sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p);
      state.obj.new_sec_flags = STYP_BSS | STYP_ABS;
      break;
    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal do_udata_acs(gpasmVal r,
		             char *name,
		             int arity,
		             struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = sec;
  state.next_state = state_section;
  
  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      strncpy(state.obj.new_sec_name,
              ".udata_acs",
              sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_BSS | STYP_ACCESS;
      break;
    case 1:
      /* new absolute section */
      p = HEAD(parms);
      strncpy(state.obj.new_sec_name,
              ".udata_acs",
              sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p);
      state.obj.new_sec_flags = STYP_BSS | STYP_ABS | STYP_ACCESS;
      break;
    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal do_udata_ovr(gpasmVal r,
		             char *name,
		             int arity,
		             struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = sec;
  state.next_state = state_section;
  
  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      strncpy(state.obj.new_sec_name,
              ".udata_ovr",
              sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_BSS | STYP_OVERLAY;
      break;
    case 1:
      /* new absolute section */
      p = HEAD(parms);
      strncpy(state.obj.new_sec_name,
              ".udata_ovr",
              sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p);
      state.obj.new_sec_flags = STYP_BSS | STYP_ABS | STYP_OVERLAY;
      break;
    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal do_udata_shr(gpasmVal r,
		             char *name,
		             int arity,
		             struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = sec;
  state.next_state = state_section;
  
  if (state.mode == absolute) {
    gperror(GPE_OBJECT_ONLY, NULL);
  } else {
    switch (arity) {
    case 0:
      /* new relocatable section */
      strncpy(state.obj.new_sec_name,
              ".udata_shr",
              sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = 0;
      state.obj.new_sec_flags = STYP_BSS | STYP_SHARED;
      break;
    case 1:
      /* new absolute section */
      p = HEAD(parms);
      strncpy(state.obj.new_sec_name,
              ".udata_shr",
              sizeof(state.obj.new_sec_name));
      state.obj.new_sec_addr = maybe_evaluate(p);
      state.obj.new_sec_flags = STYP_BSS | STYP_ABS | STYP_SHARED;
      break;
    default:
      enforce_arity(arity, 1);
    }
  }

  return r;
}

static gpasmVal do_undefine(gpasmVal r,
		            char *name,
		            int arity,
		            struct pnode *parms)
{
  struct pnode *p;

  state.lst.line.linetype = dir;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (p->tag == symbol) {
      if (remove_symbol(state.stDefines, p->value.symbol) == 0)
        gpwarning(GPW_NOT_DEFINED, NULL);
    } else {
      gperror(GPE_ILLEGAL_ARGU, NULL);
    }
  }

  return r;
}

static gpasmVal do_variable(gpasmVal r,
		       char *name,
		       int arity,
		       struct pnode *parms)
{
  struct pnode *p;
  int value;

  state.lst.line.linetype = dir;

  for (; parms; parms = TAIL(parms)) {
    p = HEAD(parms);
    if ((p->tag == binop) &&
        (p->value.binop.op == '=')) {
      if (enforce_simple(p->value.binop.p0)) {
        char *lhs;
        /* fetch the symbol */
        lhs = p->value.binop.p0->value.symbol;
        value = maybe_evaluate(p->value.binop.p1);
        /* put the symbol and value in the table*/
        set_global(lhs, value, TEMPORARY, gvt_constant);
      }
    } else if (p->tag == symbol) {
      /* put the symbol with a 0 value in the table*/
      set_global(p->value.symbol, 0, TEMPORARY, gvt_constant);
    } else {
      gperror(GPE_ILLEGAL_ARGU, NULL);
    }
  }

  return r;
}

static gpasmVal do_while(gpasmVal r,
			 char *name,
			 int arity,
			 struct pnode *parms)
{
  struct macro_head *head = malloc(sizeof(*head));
  struct pnode *p;

  if (state.src->type == src_while) {
    state.pass = 2; /* Ensure error actually gets displayed */
    gperror(GPE_UNKNOWN, "gpasm does not yet support nested while loops");
    exit (1);
  }
  
  state.lst.line.linetype = dir;
  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    head->parms = p;
  } else {
    head->parms = NULL;
  }

  head->body = NULL;
  /* Record data for the list, cod, and coff files */
  head->line_number = state.src->line_number;
  head->file_symbol = state.src->file_symbol;

  head->src_name = strdup(state.src->name);

  /* DON'T set up state.mac_head; this isn't a macro head. */
  state.while_head = head;

  state.mac_prev = &(head->body);
  state.mac_body = NULL;

  return r;
}

int asm_enabled(void)
{
  return ((state.astack == NULL) ||
	  (state.astack->enabled &&
	   state.astack->prev_enabled));
}

/* Check that a register file address is ok */
void file_ok(unsigned int file)
{
  /* don't check address, the linker takes care of it */   
  if (state.mode == relocatable)
    return;

  if ((0 > file) || (file > state.maxram) || (state.badram[file])) {
    gpwarning(GPW_INVALID_RAM, NULL);
  }

  /* Issue bank message if necessary */
  switch (state.device.class) {
  case PROC_CLASS_EEPROM8:
    /* do nothing */
    break;
  case PROC_CLASS_GENERIC:
  case PROC_CLASS_PIC12:
  case PROC_CLASS_SX:
    if (file & (~0x1f))
      gpmessage(GPM_BANK, NULL);   
    break;
  case PROC_CLASS_PIC14:
    if (file & (~0x7f))
      gpmessage(GPM_BANK, NULL);  
    break;
  case PROC_CLASS_PIC16:
    if (file & (~0xff))
      gpmessage(GPM_BANK, NULL);  
    break;
  case PROC_CLASS_PIC16E:
    /* do nothing */
    break;
  default:
    assert(0);
  }

  return;
}

static void emit_check(int insn, int argument, int mask)
{
  int test = argument;

  if (test < 0)
    test = -test;

  /* If there are bits that shouldn't be set then issue a warning */
  if (test & (~mask)) {
    gpwarning(GPW_RANGE, NULL);
  }
  emit(insn | (argument & mask));
  
  return;
} 

/* 
   For relative branches, issue a warning if the absolute value of
   argument is greater than range
*/
static void emit_check_relative(int insn, int argument, int mask, int range)
{
  char full_message[BUFSIZ];

  /* If the branch is too far then issue an error */
  if ((argument > range) || (argument < -(range+1))) {
    snprintf(full_message,
             sizeof(full_message),
             "Argument out of range (%d not between %d and %d)\n",
             argument,
             -(range+1),
             range);
    gperror(GPE_RANGE, full_message);
  }

  emit(insn | (argument & mask));
  
  return;
} 

static int check_flag(int flag)
{
  if ((flag != 0) && (flag != 1))
    gpwarning(GPW_RANGE, NULL);

  return flag & 0x1;
}

gpasmVal do_insn(char *name, struct pnode *parms)
{
  struct symbol *s;
  int arity;
  struct pnode *p;
  int file;		/* register file address, if applicable */
  gpasmVal r;		/* Return value */

  r = state.org;

  arity = list_length(parms);

  s = get_symbol(state.stBuiltin, name);

  if (s) {
    struct insn *i;

    i = get_symbol_annotation(s);
    /* Interpret the instruction if assembly is enabled, or if it's a
       conditional. */
    if (asm_enabled() || (i->attribs & ATTRIB_COND)) {
      state.lst.line.linetype = insn;
      switch (i->class) {
      case INSN_CLASS_LIT3_BANK:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  emit_check(i->opcode, (reloc_evaluate(p, RELOCT_F) >> 5), 0x07);
	}
	break;
      case INSN_CLASS_LIT3_PAGE:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  emit_check(i->opcode, (reloc_evaluate(p, RELOCT_F) >> 9), 0x07);
	}
	break;
      case INSN_CLASS_LIT1:
	{
	  int s = 0;
	  switch (arity) {
	  case 1:
	    s = check_flag(reloc_evaluate(HEAD(parms), RELOCT_F));
	  case 0:
	    emit(i->opcode | s);
	    break;
	  default:
	    enforce_arity(arity, 1);
	  }
	}
	break;
      case INSN_CLASS_LIT4:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  emit_check(i->opcode, reloc_evaluate(p, RELOCT_F), 0x0f);
	}
	break;
      case INSN_CLASS_LIT4S:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  emit_check(i->opcode, (reloc_evaluate(p, RELOCT_MOVLR) << 4), 0xf0);
	}
	break;
      case INSN_CLASS_LIT6:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  /* The literal cannot be a relocatable address */
          emit_check(i->opcode, maybe_evaluate(p), 0x3f);
	}
	break;
      case INSN_CLASS_LIT8:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  if (strcasecmp(i->name, "movlb") == 0) {
	    emit_check(i->opcode, reloc_evaluate(p, RELOCT_MOVLB), 0xff);
	  } else {
	    emit_check(i->opcode, reloc_evaluate(p, RELOCT_LOW), 0xff);
	  }
	}
	break;
      case INSN_CLASS_LIT8C12:
	if (enforce_arity(arity, 1)) {
	  int value;

	  p = HEAD(parms);
	  value = reloc_evaluate(p, RELOCT_CALL);	  

	  /* PC is 11 bits.  mpasm checks the maximum device address. */
	  if (value & (~0x7ff))
	    gperror(GPE_RANGE, NULL);

	  if ((value & 0x600) != (state.org & 0x600))
	    gpmessage(GPM_PAGE, NULL);	  

	  if (value & 0x100)
	    gperror(GPE_BAD_CALL_ADDR, NULL);
	  	  
	  emit(i->opcode | (value & 0xff));	      
	}
	break;
      case INSN_CLASS_LIT8C16:
	if (enforce_arity(arity, 1)) {
	  int value;

	  p = HEAD(parms);
	  value = reloc_evaluate(p, RELOCT_LOW);	  

	  /* PC is 16 bits.  mpasm checks the maximum device address. */
	  if (value & (~0xffff))
	    gperror(GPE_RANGE, NULL); 	  
	  	  
	  emit(i->opcode | (value & 0xff));	      
	}
	break;
      case INSN_CLASS_LIT9:
	if (enforce_arity(arity, 1)) {
	  int value;

	  p = HEAD(parms);
	  value = reloc_evaluate(p, RELOCT_GOTO);

	  /* PC is 11 bits.  mpasm checks the maximum device address. */
	  if (value & (~0x7ff))
	    gperror(GPE_RANGE, NULL); 	  
	  
	  if ((value & 0x600) != (state.org & 0x600))
	    gpmessage(GPM_PAGE, NULL);	  
	  	  
	  emit(i->opcode | (value & 0x1ff));
	}
	break;
      case INSN_CLASS_LIT11:
	if (enforce_arity(arity, 1)) {
	  int value;
	  
	  p = HEAD(parms);
          if (strcasecmp(i->name, "goto") == 0) {
	    value = reloc_evaluate(p, RELOCT_GOTO);
          } else {
	    value = reloc_evaluate(p, RELOCT_CALL);
          }
	  
	  /* PC is 13 bits.  mpasm checks the maximum device address. */
	  if (value & (~0x1fff))
	    gperror(GPE_RANGE, NULL); 	  
	  
	  if ((value & 0x1800) != (state.org & 0x1800))
	    gpmessage(GPM_PAGE, NULL);
	  
	  emit(i->opcode | (value & 0x7ff));
	}
	break;
      case INSN_CLASS_LIT13:
	if (enforce_arity(arity, 1)) {
	  int value;

	  p = HEAD(parms);
          if (strcasecmp(i->name, "goto") == 0) {
	    value = reloc_evaluate(p, RELOCT_GOTO);
          } else {
	    value = reloc_evaluate(p, RELOCT_CALL);
          }

	  /* PC is 16 bits.  mpasm checks the maximum device address. */
	  if (value & (~0xffff))
	    gperror(GPE_RANGE, NULL); 	  

	  if ((value & 0xe000) != (state.org & 0xe000))
	    gpmessage(GPM_PAGE, NULL);

	  emit(i->opcode | (value & 0x1fff));
	}
	break;
      case INSN_CLASS_LITFSR:
	if (enforce_arity(arity, 2)) {
	  int value;
          int fsr;

	  p = HEAD(parms);
          fsr = maybe_evaluate(p);
	  if ((fsr < 0) || (fsr > 2))
            gperror(GPE_RANGE, NULL); 	  

          p = HEAD(TAIL(parms));
	  /* the offset cannot be a relocatable address */
          value = maybe_evaluate(p);
	  if (value & (~0x3f))
            gperror(GPE_RANGE, NULL); 	  

	  emit(i->opcode | ((fsr & 0x3) << 6) | (value & 0x3f));
	}
	break;
      case INSN_CLASS_RBRA8:
	if (enforce_arity(arity, 1)) {
	  int offset;

	  p = HEAD(parms);
	  if (count_reloc(p) == 0) {
	    offset = maybe_evaluate(p) - ((state.org + 1)<<_16bit_core);
	    offset >>= _16bit_core;
	  } else {
	    offset = reloc_evaluate(p, RELOCT_CONDBRA);
	  }

	  /* The offset for the relative branch must be 
	     between -127 <= offset <= 127. */
	  emit_check_relative(i->opcode, offset, 0xff, 127);
	}
	break;
      case INSN_CLASS_RBRA11:
	if (enforce_arity(arity, 1)) {
	  int offset;

	  p = HEAD(parms);
	  if (count_reloc(p) == 0) {
	    offset = maybe_evaluate(p) - ((state.org + 1)<<_16bit_core);
	    offset >>= _16bit_core;
	  } else {
	    offset = reloc_evaluate(p, RELOCT_BRA);
	  }	  
         
	  emit_check_relative(i->opcode, offset, 0x7ff, 0x3ff);
	}
	break;
      case INSN_CLASS_LIT20:
	if (enforce_arity(arity, 1)) {
	  int dest;
	  p = HEAD(parms);
	  dest = reloc_evaluate(p, RELOCT_GOTO) >> _16bit_core;
	  emit(i->opcode | (dest & 0xff));
	  reloc_evaluate(p, RELOCT_GOTO2);     /* add the second relocation */
	  emit_check(0xf000, dest>>8, 0xfff);
	}
	break;
      case INSN_CLASS_CALL20:
	{
	  int dest;
	  int s = 0; /* By default, fast push is not used */
	  struct pnode *p2; /* second parameter */
	    
	  p = HEAD(parms);
	  switch (arity) {
	  case 2:
	    p2 = HEAD(TAIL(parms));
	    /* Allow "s" for fast push */
	    if ((p2->tag == symbol) &&
		(strcasecmp(p2->value.symbol, "s") == 0))
	      s = 1;
	    else
	      s = check_flag(maybe_evaluate(p2));
	    break;
	  case 1:
	    s = 0;
	    break;
	  default:
	    enforce_arity(arity, 2);
	  }
	  dest = reloc_evaluate(p, RELOCT_CALL) >> _16bit_core;
	  emit(i->opcode | (s<<8) | (dest & 0xff));
	  reloc_evaluate(p, RELOCT_CALL2);     /* add the second relocation */
	  emit_check(0xf000, (dest>>8), 0xfff);
	}
	break;
      case INSN_CLASS_FLIT12:
	{
	  int k;

	  if (enforce_arity(arity, 2)) {
	    p = HEAD(parms);
	    file = maybe_evaluate(p);
	    if(file > 3)
	      gperror(GPE_UNKNOWN, "FSR is out of range");

	    p = HEAD(TAIL(parms));
	    k = reloc_evaluate(p, RELOCT_LFSR1);
	    emit_check(i->opcode | ((file & 3) << 4), (k>>8), 0xf);
	    reloc_evaluate(p, RELOCT_LFSR2); /* add the second relocation */
	    emit(0xf000 | (k & 0xff));
	  }
	}
	break;

      case INSN_CLASS_FF:
	if (enforce_arity(arity, 2)) {
          int dest = maybe_evaluate(HEAD(TAIL(parms)));

          /* destination can't be PCL, TOSU, TOSH, TOSL */
          if ((dest == 0xff9) ||
              (dest == 0xfff) ||
              (dest == 0xffe) ||
              (dest == 0xffd)) {
            gperror(GPE_UNKNOWN, "Invalid destination");
          }
          
	  emit_check(i->opcode, reloc_evaluate(HEAD(parms), RELOCT_FF1), 0xfff);
	  emit_check(0xf000, reloc_evaluate(HEAD(TAIL(parms)), RELOCT_FF2), 0xfff);

	}
	break;
      case INSN_CLASS_FP:
        if (enforce_arity(arity, 2)) {
          int reg=0;
          file=reloc_evaluate(HEAD(parms), RELOCT_F);
          reg=reloc_evaluate(HEAD(TAIL(parms)), RELOCT_P);
          file_ok(file);
          if (reg & ~0xf1f) {
            gpwarning(GPW_RANGE, NULL);
          }
          emit(i->opcode | ( (reg  & 0x1f) << 8) |
              (file & 0xff) );
        }
        break;
      case INSN_CLASS_PF:
        if (enforce_arity(arity, 2)) {
          int reg=0;
          file=reloc_evaluate(HEAD(TAIL(parms)), RELOCT_F);
          reg=reloc_evaluate(HEAD(parms), RELOCT_P);
          file_ok(file);
          if (reg & ~0xf1f) {
            gpwarning(GPW_RANGE, NULL);
          }
          emit(i->opcode | ( (reg & 0x1f) << 8) |
              (file & 0xff) );
        }
        break;
      case INSN_CLASS_SF:
        if (enforce_arity(arity, 2)) {
          int source;
	  int dest;

	  p = HEAD(parms);
          if (p->tag != offset)
	    gperror(GPE_MISSING_BRACKET, NULL);
          source = maybe_evaluate(p);

          p = HEAD(TAIL(parms));
          dest = maybe_evaluate(p);

          /* destination can't be PCL, TOSU, TOSH, TOSL */
          if ((dest == 0xff9) ||
              (dest == 0xfff) ||
              (dest == 0xffe) ||
              (dest == 0xffd)) {
            gperror(GPE_UNKNOWN, "Invalid destination");
          }
          
	  emit_check(i->opcode, source, 0x7f);
	  emit_check(0xf000, reloc_evaluate(HEAD(TAIL(parms)), RELOCT_FF2), 0xfff);
        }
        break;
      case INSN_CLASS_SS:
        if (enforce_arity(arity, 2)) {
          int source;
	  int dest;

	  p = HEAD(parms);
          if (p->tag != offset)
	    gperror(GPE_MISSING_BRACKET, NULL);
          source = maybe_evaluate(p);

          p = HEAD(TAIL(parms));
          if (p->tag != offset)
	    gperror(GPE_MISSING_BRACKET, NULL);
          dest = maybe_evaluate(p);

	  emit_check(i->opcode, source, 0x7f);
	  emit_check(0xf000, dest, 0x7f);
        }
        break;
      case INSN_CLASS_OPF5:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  if (strcasecmp(i->name, "tris") == 0) {
	    file = reloc_evaluate(p, RELOCT_TRIS);
	  } else {
	    file = reloc_evaluate(p, RELOCT_F);
	  }
	  file_ok(file);
	  emit(i->opcode | (file & 0x1f));
	}
	break;
      case INSN_CLASS_OPWF5:
	{
	  int d = 1; /* Default destination of 1 (file) */
	  struct pnode *p2; /* second parameter */
	    
	  if(arity == 0) {
	    enforce_arity(arity, 2);
	    break;
	  }

	  p = HEAD(parms);
	  switch (arity) {
	  case 2:
	    p2 = HEAD(TAIL(parms));
	    /* Allow "w" and "f" as destinations */
	    if ((p2->tag == symbol) &&
		(strcasecmp(p2->value.symbol, "f") == 0))
	      d = 1;
	    else if ((p2->tag == symbol) &&
		     (strcasecmp(p2->value.symbol, "w") == 0))
	      d = 0;
	    else
	      d = check_flag(maybe_evaluate(p2));
	    break;
	  case 1:
            d = 1;
            gpmessage(GPM_NOF,NULL);
	    break;
	  default:
	    enforce_arity(arity, 2);
	  }
	  file = reloc_evaluate(p, RELOCT_F);
	  file_ok(file);
	  emit(i->opcode | (d << 5) | (file & 0x1f));
	}
	break;
      case INSN_CLASS_B5:
	{
	  struct pnode *f, *b;
	  int bit;
	    
	  if (enforce_arity(arity, 2)) {
	    f = HEAD(parms);
	    b = HEAD(TAIL(parms));
	    file = reloc_evaluate(f, RELOCT_F);
	    bit = maybe_evaluate(b);
	    if (!((0 <= bit) && (bit <= 7)))
              gpwarning(GPW_RANGE, NULL);
	    file_ok(file);
	    emit(i->opcode | ((bit & 7) << 5) |(file & 0x1f));
	  }
	}
	break;

      case INSN_CLASS_B8:
	{
	  struct pnode *f, *b;
	  int bit;
	    
	  if (enforce_arity(arity, 2)) {
	    f = HEAD(parms);
	    b = HEAD(TAIL(parms));
	    file = reloc_evaluate(f, RELOCT_F);
	    bit = maybe_evaluate(b);
	    if (!((0 <= bit) && (bit <= 7)))
              gpwarning(GPW_RANGE, NULL);
	    file_ok(file);
	    emit(i->opcode | ((bit & 7) << 8) | (file & 0xff));
	  }
	}
	break;

      case INSN_CLASS_OPF7:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  if (strcasecmp(i->name, "tris") == 0) {
	    gpwarning(GPW_NOT_RECOMMENDED, NULL);
	    file = reloc_evaluate(p, RELOCT_TRIS);
	  } else {
	    file = reloc_evaluate(p, RELOCT_F);
	  }
	  file_ok(file);
	  emit(i->opcode | (file & 0x7f));
	}
	break;

      case INSN_CLASS_OPF8:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  file = reloc_evaluate(p, RELOCT_F);
	  file_ok(file);
	  emit(i->opcode | (file & 0xff));
	}
	break;

      case INSN_CLASS_OPWF7:
	{
	  int d = 1; /* Default destination of 1 (file) */
	  struct pnode *p2; /* second parameter */
	    
	  if(arity == 0) {
	    enforce_arity(arity, 2);
	    break;
	  }

	  p = HEAD(parms);
	  switch (arity) {
	  case 2:
	    p2 = HEAD(TAIL(parms));
	    /* Allow "w" and "f" as destinations */
	    if ((p2->tag == symbol) &&
		(strcasecmp(p2->value.symbol, "f") == 0))
	      d = 1;
	    else if ((p2->tag == symbol) &&
		     (strcasecmp(p2->value.symbol, "w") == 0))
	      d = 0;
	    else
	      d = check_flag(maybe_evaluate(p2));
	    break;
	  case 1:
            d = 1;
            gpmessage(GPM_NOF,NULL);
	    break;
	  default:
	    enforce_arity(arity, 2);
	  }
	  file = reloc_evaluate(p, RELOCT_F);
	  file_ok(file);
	  emit(i->opcode | (d << 7) | (file & 0x7f));
	}
	break;

      case INSN_CLASS_OPWF8:
	{
	  int d = 1; /* Default destination of 1 (file) */
	  struct pnode *p2; /* second parameter */
	    
	  if(arity == 0) {
	    enforce_arity(arity, 2);
	    break;
	  }

	  p = HEAD(parms);
	  switch (arity) {
	  case 2:
	    p2 = HEAD(TAIL(parms));
	    /* Allow "w" and "f" as destinations */
	    if ((p2->tag == symbol) &&
		(strcasecmp(p2->value.symbol, "f") == 0))
	      d = 1;
	    else if ((p2->tag == symbol) &&
		     (strcasecmp(p2->value.symbol, "w") == 0))
	      d = 0;
	    else
	      d = check_flag(maybe_evaluate(p2));
	    break;
	  case 1:
            d = 1;
            gpmessage(GPM_NOF,NULL);
	    break;
	  default:
	    enforce_arity(arity, 2);
	  }
	  file = reloc_evaluate(p, RELOCT_F);
	  file_ok(file);
	  emit(i->opcode | (d << 8) | (file & 0xff));
	}
	break;
      case INSN_CLASS_B7:
	{
	  struct pnode *f, *b;
	  int bit;
	    
	  if (enforce_arity(arity, 2)) {
	    f = HEAD(parms);
	    b = HEAD(TAIL(parms));
	    file = reloc_evaluate(f, RELOCT_F);
	    bit = maybe_evaluate(b);
	    if (!((0 <= bit) && (bit <= 7)))
              gpwarning(GPW_RANGE, NULL);
	    file_ok(file);
	    emit(i->opcode | ((bit & 7) << 7) | (file & 0x7f));
	  }
	}
	break;

      case INSN_CLASS_OPFA8:
	{
	  int a = 0;
	  struct pnode *p2; /* second parameter */

	  if(arity == 0) {
	    enforce_arity(arity, 2);
	    break;
	  }

	  p = HEAD(parms);
	  file = reloc_evaluate(p, RELOCT_F);
	  file_ok(file);

	  /* add relocation for the access bit, if necessary */          
	  reloc_evaluate(p, RELOCT_ACCESS);

	  /* Default access (use the BSR unless access is to special registers) */
	  if ((file < state.device.bsr_boundary) || 
              (file >= (0xf00 + state.device.bsr_boundary))) {
            a = 0;
	  } else {
	    a = 1;
	  }

	  switch (arity) {
	  case 2:
	    p2 = HEAD(TAIL(parms));
	    /* Allow "w" and "f" as destinations */
	    if ((p2->tag == symbol) &&
		(strcasecmp(p2->value.symbol, "b") == 0))
	      a = 1;
	    else
	      a = check_flag(maybe_evaluate(p2));
	    break;
	  case 1:
	    /* use default a */
	    break;
	  default:
	    enforce_arity(arity, 2);
	  }
	  emit(i->opcode | (a << 8) | (file & 0xff));
	}
	break;

      case INSN_CLASS_BA8:
	{
	  struct pnode *f, *b,*par;
	  int bit,a=0;

	  if ((arity != 2) && (arity != 3)) {
	    enforce_arity(arity, 3);
	    break;
	  }

	  f = HEAD(parms);
	  file = reloc_evaluate(f, RELOCT_F);

	  if (arity == 3) {
	    par = HEAD(TAIL(TAIL(parms)));

	    if ((par->tag == symbol) &&
		(strcasecmp(par->value.symbol, "b") == 0))
	      a = 1;
	    else
	      a = check_flag(maybe_evaluate(par));
	  } else {
	    /* Default access (use the BSR unless access is to special 
               registers) */
	    if ((file < state.device.bsr_boundary) || 
		(file >= (0xf00 + state.device.bsr_boundary))) {
	      a = 0;
	    } else {
	      a = 1;
	    }          
	  }
	    
	  /* add relocation for the access bit, if necessary */          
	  reloc_evaluate(f, RELOCT_ACCESS);

	  b = HEAD(TAIL(parms));
	  bit = maybe_evaluate(b);
	  if (!((0 <= bit) && (bit <= 7)))
	    gpwarning(GPW_RANGE, NULL);
	  file_ok(file);
	  emit(i->opcode | ( a << 8) | ((bit & 7) << 9) | (file & 0xff));
	}
	break;

      case INSN_CLASS_OPWFA8:
	{
	  int d = 1; /* Default destination of 1 (file) */
	  int a = 0;
	  struct pnode *par; /* second parameter */
	    
	  if(arity == 0) {
	    enforce_arity(arity, 2);
	    break;
	  }

	  p = HEAD(parms);
	  file = reloc_evaluate(p, RELOCT_F);
	  file_ok(file);

	  /* add relocation for the access bit, if necessary */          
	  reloc_evaluate(p, RELOCT_ACCESS);

	  /* Default access (use the BSR unless access is to special registers) */
	  if ((file < state.device.bsr_boundary) || 
              (file >= (0xf00 + state.device.bsr_boundary))) {
            a = 0;
	  } else {
	    a = 1;
	  }

	  switch (arity) {
	  case 3:
	    par = HEAD(TAIL(TAIL(parms)));

	    if ((par->tag == symbol) &&
		(strcasecmp(par->value.symbol, "b") == 0))
	      a = 1;
	    else
	      a = check_flag(maybe_evaluate(par));
	    /* fall through */
	  case 2:
	    par = HEAD(TAIL(parms));
	    /* Allow "w" and "f" as destinations */
	    if ((par->tag == symbol) &&
		(strcasecmp(par->value.symbol, "f") == 0))
	      d = 1;
	    else if ((par->tag == symbol) &&
		     (strcasecmp(par->value.symbol, "w") == 0))
	      d = 0;
	    else
	      d = check_flag(maybe_evaluate(par));
	    break;
	  case 1:
            /* use default a and d */
            gpmessage(GPM_NOF,NULL);
            break;
	  default:
            enforce_arity(arity, 3);
	  }
	  emit(i->opcode | (d << 9) | (a << 8) | (file & 0xff));
	}
	break;

      case INSN_CLASS_IMPLICIT:
	if (arity != 0) {
          gpwarning(GPW_EXTRANEOUS, NULL);
	}
	if ((strcasecmp(i->name, "option") == 0) &&
            (state.device.core_size != CORE_12BIT_MASK)){
          gpwarning(GPW_NOT_RECOMMENDED, NULL);
        }
	emit(i->opcode);
	break;
      case INSN_CLASS_TBL:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  switch(maybe_evaluate(p))
	    {
	    case TBL_NO_CHANGE:
	      emit(i->opcode);
	      break;
	    case TBL_POST_INC:
	      emit(i->opcode | 1);
	      break;
	    case TBL_POST_DEC:
	      emit(i->opcode | 2);
	      break;
	    case TBL_PRE_INC:
	      emit(i->opcode | 3);
	      break;
	    default:
	      gperror(GPE_ILLEGAL_ARGU, NULL);
	    }
	}
	break;

      case INSN_CLASS_TBL2:
	if (enforce_arity(arity, 2)) {
	  int t=0; /* read low byte by default */
	  struct pnode *p2; /* second parameter */

	  /* "0" (lower byte) and "1" (upper byte) */
	  p = HEAD(parms);
	  t = check_flag(maybe_evaluate(p));

	  p2 = HEAD(TAIL(parms));
	  file = reloc_evaluate(p2, RELOCT_F);

	  file_ok(file);
          emit(i->opcode | (t << 9) | (file & 0xff));
	}
	break;

      case INSN_CLASS_TBL3:
	if (enforce_arity(arity, 3)) {
	  int inc=0,t=0;
	  struct pnode *p2; /* second parameter */
	  struct pnode *p3; /* third parameter */

	  /* "0" (lower byte) and "1" (upper byte) */
	  p = HEAD(parms);
	  t = check_flag(maybe_evaluate(p));

	  /* "0" (no change) and "1" (postincrement) */
	  p2 = HEAD(TAIL(parms));
	  inc = check_flag(maybe_evaluate(p2));

	  p3 = HEAD(TAIL(TAIL(parms)));
	  file = reloc_evaluate(p3, RELOCT_F);

	  file_ok(file);          
          emit(i->opcode | (t << 9) | (inc << 8) | (file & 0xff));
	}
	break;

      case INSN_CLASS_FUNC:
	r = (*(opfunc*)i->opcode)(r, name, arity, parms);
	break;
      }
    }
  } else {
    s = get_symbol(state.stMacros, name);
    if (s) {
      struct macro_head *h = get_symbol_annotation(s);

      /* Found the macro: execute it */
      if (asm_enabled()) {
	if ((h->defined != 1) && (state.pass == 2)) {
	  gperror(GPE_UNKNOWN, "Forward references to macros are not allowed.");	
	} else {
	  setup_macro(h, arity, parms);
	}
      }
    } else {

      if (asm_enabled()) {
        if (state.processor_chosen == 0){
          gperror(GPE_UNDEF_PROC, NULL);
        } else {
          char mesg[80];
          snprintf(mesg, sizeof(mesg), "Unknown opcode \"%.40s\"", name);
          gperror(GPE_UNKNOWN, mesg);
        }
      }

    }
  }

  return r;
}

/************************************************************************/

/* There are several groups of operations that we handle here.  First
   is op_0: the instructions that can happen before the processor type
   is known.  Second is op_1, the set of instructions that are common
   to all processors, third is processor-family specific: op_XXX */

/* Note that instructions within each group are sorted alphabetically */

struct insn op_0[] = {
  { "code",       0, (long int)do_code,      INSN_CLASS_FUNC,   0 },
  { "constant",   0, (long int)do_constant,  INSN_CLASS_FUNC,   0 },
  { "else",       0, (long int)do_else,      INSN_CLASS_FUNC,   ATTRIB_COND },
  { "endif",      0, (long int)do_endif,     INSN_CLASS_FUNC,   ATTRIB_COND },
  { "endm",       0, (long int)do_endm,	     INSN_CLASS_FUNC,   0 },
  { "endw",       0, (long int)do_endw,	     INSN_CLASS_FUNC,   0 },
  { "equ",        0, (long int)do_equ,       INSN_CLASS_FUNC,   0 },
  { "error",      0, (long int)do_error,     INSN_CLASS_FUNC,   0 },
  { "exitm",      0, (long int)do_exitm,     INSN_CLASS_FUNC,   0 },
  { "expand",     0, (long int)do_expand,    INSN_CLASS_FUNC,   0 },
  { "extern",     0, (long int)do_extern,    INSN_CLASS_FUNC,   0 },
  { "errorlevel", 0, (long int)do_errlvl,    INSN_CLASS_FUNC,   0 },
  { "global",     0, (long int)do_global,    INSN_CLASS_FUNC,   0 },
  { "idata",      0, (long int)do_idata,     INSN_CLASS_FUNC,   0 },
  { "if",         0, (long int)do_if,        INSN_CLASS_FUNC,   ATTRIB_COND },
  { "ifdef",      0, (long int)do_ifdef,     INSN_CLASS_FUNC,   ATTRIB_COND },
  { "ifndef",     0, (long int)do_ifndef,    INSN_CLASS_FUNC,   ATTRIB_COND },
  { "include",    0, (long int)do_include,   INSN_CLASS_FUNC,   0 },
  { "list",       0, (long int)do_list,	     INSN_CLASS_FUNC,   0 },
  { "local",      0, (long int)do_local,     INSN_CLASS_FUNC,   0 },
  { "macro",      0, (long int)do_macro,     INSN_CLASS_FUNC,	0 },
  { "messg",      0, (long int)do_messg,     INSN_CLASS_FUNC,   0 },
  { "noexpand",   0, (long int)do_noexpand,  INSN_CLASS_FUNC,   0 },
  { "nolist",     0, (long int)do_nolist,    INSN_CLASS_FUNC,   0 },
  { "page",       0, (long int)do_page,      INSN_CLASS_FUNC,   0 },
  { "processor",  0, (long int)do_processor, INSN_CLASS_FUNC,   0 },
  { "radix",      0, (long int)do_radix,     INSN_CLASS_FUNC,   0 },
  { "set",        0, (long int)do_set, 	     INSN_CLASS_FUNC,   0 },
  { "space",      0, (long int)do_space,     INSN_CLASS_FUNC,   0 },
  { "udata",      0, (long int)do_udata,     INSN_CLASS_FUNC,   0 },
  { "udata_acs",  0, (long int)do_udata_acs, INSN_CLASS_FUNC,   0 },
  { "udata_ovr",  0, (long int)do_udata_ovr, INSN_CLASS_FUNC,   0 },
  { "udata_shr",  0, (long int)do_udata_shr, INSN_CLASS_FUNC,   0 },
  { "variable",   0, (long int)do_variable,  INSN_CLASS_FUNC,   0 },
  { "while",      0, (long int)do_while,     INSN_CLASS_FUNC,   0 },
  { ".def",       0, (long int)do_def, 	     INSN_CLASS_FUNC,   0 },
  { ".dim",       0, (long int)do_dim, 	     INSN_CLASS_FUNC,   0 },
  { ".direct",    0, (long int)do_direct,    INSN_CLASS_FUNC,   0 },
  { ".eof",       0, (long int)do_eof,	     INSN_CLASS_FUNC,   0 },
  { ".file",      0, (long int)do_file,	     INSN_CLASS_FUNC,   0 },
  { ".ident",     0, (long int)do_ident,     INSN_CLASS_FUNC,   0 },
  { ".line",      0, (long int)do_line,	     INSN_CLASS_FUNC,   0 },
  { ".set",       0, (long int)do_set, 	     INSN_CLASS_FUNC,   0 },
  { ".type",      0, (long int)do_type,	     INSN_CLASS_FUNC,   0 },
  { "#if",        0, (long int)do_if,        INSN_CLASS_FUNC,   ATTRIB_COND },
  { "#else",      0, (long int)do_else,	     INSN_CLASS_FUNC,   ATTRIB_COND },
  { "#endif",     0, (long int)do_endif,     INSN_CLASS_FUNC,   ATTRIB_COND },
  { "#ifdef",     0, (long int)do_ifdef,     INSN_CLASS_FUNC,   ATTRIB_COND },
  { "#ifndef",    0, (long int)do_ifndef,    INSN_CLASS_FUNC,   ATTRIB_COND },
  { "#undefine",  0, (long int)do_undefine,  INSN_CLASS_FUNC,   0 }
};

const int num_op_0 = TABLE_SIZE(op_0);

struct insn op_1[] = {
  { "__badram",   0, (long int)do_badram,    INSN_CLASS_FUNC,   0 },
  { "__badrom",   0, (long int)do_badrom,    INSN_CLASS_FUNC,   0 },
  { "__config",   0, (long int)do_config,    INSN_CLASS_FUNC,   0 },
  { "__fuses",    0, (long int)do_config,    INSN_CLASS_FUNC,   0 },
  { "__idlocs",   0, (long int)do_idlocs,    INSN_CLASS_FUNC,   0 },
  { "__maxram",   0, (long int)do_maxram,    INSN_CLASS_FUNC,   0 },
  { "__maxrom",   0, (long int)do_maxrom,    INSN_CLASS_FUNC,   0 },
  { "bankisel",   0, (long int)do_bankisel,  INSN_CLASS_FUNC,   0 },
  { "banksel",    0, (long int)do_banksel,   INSN_CLASS_FUNC,   0 },
  { "data",       0, (long int)do_data,      INSN_CLASS_FUNC,   0 },
  { "da",         0, (long int)do_da, 	     INSN_CLASS_FUNC,   0 },
  { "db",         0, (long int)do_db, 	     INSN_CLASS_FUNC,   0 },
  { "de",         0, (long int)do_de, 	     INSN_CLASS_FUNC,   0 },
  { "dt",         0, (long int)do_dt, 	     INSN_CLASS_FUNC,   0 },
  { "dw",         0, (long int)do_dw, 	     INSN_CLASS_FUNC,   0 },
  { "fill",       0, (long int)do_fill,      INSN_CLASS_FUNC,   0 },
  { "org",        0, (long int)do_org,       INSN_CLASS_FUNC,   0 },
  { "pagesel",    0, (long int)do_pagesel,   INSN_CLASS_FUNC,   0 },
  { "res",        0, (long int)do_res,       INSN_CLASS_FUNC,   0 }
};

const int num_op_1 = TABLE_SIZE(op_1);

void opcode_init(int stage)
{
  int i;
  int count = 0;
  struct insn *base = NULL;

  switch (stage) {
  case 0:
    base = op_0;
    count = num_op_0;
    break;
  case 1:
    base = op_1;
    count = num_op_1;
    break;
  case 2:
    state.device.class = gp_processor_class(state.processor);
    switch (state.device.class) {
    case PROC_CLASS_EEPROM8:
      base = 0;
      count = 0;
      state.device.core_size = CORE_8BIT_MASK;
      state.device.config_address = 0;
      state.device.id_location = 0;
      break;
    case PROC_CLASS_GENERIC:
      base = 0;
      count = 0;
      state.device.core_size = CORE_12BIT_MASK;
      state.device.config_address = CONFIG_ADDRESS_12;
      state.device.id_location = IDLOC_ADDRESS_12;
      break;
    case PROC_CLASS_PIC12:
      base = op_12c5xx;
      count = num_op_12c5xx;
      state.device.core_size = CORE_12BIT_MASK;
      state.device.config_address = CONFIG_ADDRESS_12;
      state.device.id_location = IDLOC_ADDRESS_12;
      break;
    case PROC_CLASS_SX:
      base = op_sx;
      count = num_op_sx;
      state.device.core_size = CORE_12BIT_MASK;
      state.device.config_address = CONFIG_ADDRESS_12;
      state.device.id_location = IDLOC_ADDRESS_12;
      /* page instruction conflicts with the page directive */
      remove_symbol(state.stBuiltin, "page");    
      break;
    case PROC_CLASS_PIC14:
      base = op_16cxx;
      count = num_op_16cxx;
      state.device.core_size = CORE_14BIT_MASK;
      state.device.config_address = CONFIG_ADDRESS_14;
      state.device.id_location = IDLOC_ADDRESS_14;
      break;
    case PROC_CLASS_PIC16:
      base = op_17cxx;
      count = num_op_17cxx;
      state.device.core_size = CORE_16BIT_MASK;
      _17cxx_core = 1;
      state.device.config_address = CONFIG_17CXX;
      break;
    case PROC_CLASS_PIC16E:
      base = op_18cxx;
      count = num_op_18cxx;
      state.device.core_size = CORE_16BIT_MASK;
      _16bit_core = 1;
      state.c_memory_base = CONFIG1L;
      state.device.config_address = CONFIG1L;
      state.device.bsr_boundary = gp_processor_bsr_boundary(state.processor);

      /* The 16_bit core special macros are encoded directly into the
       * symbol table like regular instructions. */
      for (i = 0; i < num_op_18cxx_sp; i++)
	annotate_symbol( add_symbol(state.stBuiltin, op_18cxx_sp[i].name), 
			 &op_18cxx_sp[i]);

      if (state.extended_pic16e) {
        /* Some 18xx devices have an extended instruction set. */
        for (i = 0; i < num_op_18cxx_ext; i++)
	  annotate_symbol( add_symbol(state.stBuiltin, op_18cxx_ext[i].name), 
                           &op_18cxx_ext[i]);
      }

      break;

    default:
      assert(0);
    }
    break;
  case 3:
    /* add 12 and 14 bit special macros */
    base = special;
    count = num_op_special;
    break;
  default:
    assert(0);
  }

  for (i = 0; i < count; i++)
    annotate_symbol(add_symbol(state.stBuiltin, base[i].name), &base[i]);

  /* Special Case, Some instructions not available on 17c42 devices */
  if (state.processor == pic17c42) {
    remove_symbol(state.stBuiltin, "MULWF");    
    remove_symbol(state.stBuiltin, "MOVLR");    
    remove_symbol(state.stBuiltin, "MULLW");    
  }

}

/************************************************************************/

void begin_cblock(struct pnode *c)
{
  if (asm_enabled()) { 
    state.cblock = maybe_evaluate(c);
  }
}

void cblock_expr(struct pnode *s)
{
  if ((asm_enabled()) && (can_evaluate_concatenation(s))) {
    set_global(evaluate_concatenation(s), state.cblock, PERMANENT, gvt_cblock);
    state.cblock++;
  }
}

void cblock_expr_incr(struct pnode *s, struct pnode *incr)
{
  if ((asm_enabled()) && (can_evaluate_concatenation(s))) {
    set_global(evaluate_concatenation(s), state.cblock, PERMANENT, gvt_cblock);
    state.cblock += maybe_evaluate(incr);
  }
}
