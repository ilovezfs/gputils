/* Implements directives, pseudo-ops and processor opcodes
   Copyright (C) 1998,1999,2000,2001 James Bowman, Craig Franklin

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

#include "gpasm.h"
#include "gpsymbol.h"
#include "gpopcode.h"
#include "directive.h"
#include "evaluate.h"
#include "parse.h"
#include "processor.h"
#include "lst.h"
#include "macro.h"
#include "gperror.h"
#include "gpmemory.h"
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

/* We really should really think about using glib for this: */
struct sllist {
  int value;
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

static int packed_hi_lo = 0;
static int packed_byte  = 0;

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

/* determine which page of program memory the address is located */
int check_page(int address)
{
  int page;

  switch (state.device.class) {
  case PROC_CLASS_EEPROM8:
    assert(0);
    break;
  case PROC_CLASS_GENERIC:
  case PROC_CLASS_PIC12:
  case PROC_CLASS_SX:
    if (address < 0x200) {
      page = 0;
    } else if (address < 0x400) {
      page = 1;
    } else if (address < 0x600) {
      page = 2;
    } else {
      page = 3;
    }
    break;
  case PROC_CLASS_PIC14:
    if (address < 0x800) {
      page = 0;
    } else if (address < 0x1000) {
      page = 1;
    } else if (address < 0x1800) {
      page = 2;
    } else {
      page = 3;
    }  
    break;
  case PROC_CLASS_PIC16:
    page = (address >> 8) & 0xff;
    break;
  case PROC_CLASS_PIC16E:
  default:
    assert(0);
  }

  return page;
}

/* determine which bank of data memory the address is located */
int check_bank(int address)
{
  int bank;

  switch (state.device.class) {
  case PROC_CLASS_EEPROM8:
    assert(0);
    break;
  case PROC_CLASS_GENERIC:
  case PROC_CLASS_PIC12:
  case PROC_CLASS_SX:
    bank = (address >> 5) & 0x3;
    break;
  case PROC_CLASS_PIC14:
    bank = (address >> 7) & 0x3;
    break;
  case PROC_CLASS_PIC16:
  case PROC_CLASS_PIC16E:
    bank = (address >> 8) & 0xff;
    break;
  default:
    assert(0);
  }

  return bank;
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

#if 0     /* deprecated data( ) */
static void data(struct pnode *L, int flavor, int lit_mask)
{
  static int  k = 0;
  int packing_bytes;


  if (L) {

    struct pnode *p;

    packing_bytes = (lit_mask & PACKING_BYTES) ? 1 : 0;

    p = HEAD(L);
    if (p->tag == string) {
      char *pc = p->value.string;
      
      while (*pc) {

	if(packing_bytes) 
	  emit_packed(*pc | flavor, lit_mask);
	else 
	  emit((*pc & lit_mask) | flavor);

	pc++;

      }

    } else {
      k  = maybe_evaluate(p);

      if(packing_bytes) 
	emit_packed(k | flavor, lit_mask);
      else 
	emit((k&lit_mask) | flavor);

    }
    data(TAIL(L), flavor, lit_mask);

    if(packed_hi_lo) 
      emit_packed(flavor, lit_mask);

  }

}

#endif

static void data(struct sllist *L, int flavor, int lit_mask)
{


  if (L) {

    struct sllist *p,*q;

    p = L->next;
    
    while(p) {

      if(lit_mask & PACKING_BYTES)
	emit_packed(p->value | flavor, lit_mask);
      else {
	if((p->value > lit_mask) || (p->value < 0)) {
          gpwarning(GPW_RANGE,NULL);
	}

	emit((p->value & lit_mask) | flavor);
      }

      q = p;
      p = p->next;
      free(q);

    }

  }

  if(packed_hi_lo) 
    emit_packed(flavor, lit_mask);
  
}


/* If we convert to glib, we can use the built in library call g_slist_append */

struct sllist *sllist_append(struct sllist   *list,
			     int value)
{
  struct sllist *new;

  new = malloc(sizeof(*new));
  new->value = value;
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

    p = HEAD(L);

    if (p->tag == string) {
      char *pc = p->value.string;

      if(packing_strings == 2) {
        shift = 7;
      } else {
        shift = 8;
      }
      
      while (*pc) {
	if(packing_strings) {
	  if(v>= (1<<31) ) {
	    v = (v<<shift) |  *pc++;
            if(_16bit_core) {
              v = endian_swap_word(v);
            }   	    
	    list = sllist_append(list, v);
	    v = 0;
	  } else
	    v = (1<<31) | *pc++;
	  
	} else {
	  list = sllist_append(list, *pc++);
	}
      }

      if(packing_strings && v >= 1<<31 ) {
        v = v <<shift;
        if(_16bit_core) {
          v = endian_swap_word(v);
        }        
	list = sllist_append(list, v);
      }

    } else {
      list = sllist_append(list,maybe_evaluate(p));

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
    sprintf(buf, "Duplicate macro parameter (%s).", HEAD(L)->value.symbol);
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

static gpasmVal do_banksel(gpasmVal r,
		           char *name,
		           int arity,
		           struct pnode *parms)
{
  struct pnode *p;

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (p->tag != symbol) {
      gperror(GPE_ILLEGAL_LABEL, NULL);
    } else {
      r = set_bank_bits(check_bank(maybe_evaluate(p)));
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
 * gpasm handles these differences with the following functions:
 *
 * cfg_evaluate_address(struct pnode *p) - Given a pointer to a node that
 *       presumably contains the configuration address, this function will
 *       ensure that the address is valid.
 * cfg_write(int address, int value) - Store the contents of 'value' at the
 *       configuration address 'address'
 * do_config( ) - Called by the parser when a __CONFIG assembler directive
 *       is encountered.
 *
 */

static int cfg_evaluate_address(struct pnode *p)
{
  int address;

  if(can_evaluate(p)) {
    address = evaluate(p);

    if(_16bit_core) {
      switch(address) {
      case CONFIG1L:
      case CONFIG1H:
      case CONFIG2L:
      case CONFIG2H:
      case CONFIG3L:
      case CONFIG3H:
      case CONFIG4L:
      case CONFIG4H:
      case CONFIG5L:
      case CONFIG5H:
      case CONFIG6L:
      case CONFIG6H:
      case CONFIG7L:
      case CONFIG7H:
      case DEVID1:
      case DEVID2:
	return address;
      default:
	gperror(GPE_RANGE,NULL);
	return CONFIG1L;
      }
    }
    else {
      if(address != state.device.config_address)
	gperror(GPE_RANGE,NULL);
      return state.device.config_address;
    }
  }

  return state.device.config_address;
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

static gpasmVal do_config(gpasmVal r,
			  char *name,
			  int arity,
			  struct pnode *parms)
{
  struct pnode *p;
  int ca = state.device.config_address;
  int org;
  
  /* save the current org to restore to later */
  org = state.org;

  state.lst.line.linetype = config;

  p = HEAD(parms);

  switch(arity) {
  case 1:
    if(_16bit_core) {
      gpwarning(GPW_EXPECTED,"18cxxx devices should specify __CONFIG address");
      ca =  CONFIG1L;
    }
    break;

  case 2:
    ca = cfg_evaluate_address(p);
    p = HEAD(TAIL(parms));

    break;

  default:
    enforce_arity(arity,2);
    return r;
  }
  state.lst.config_address = ca;
  state.org = ca;
  if (can_evaluate(p)) { 

    if(_16bit_core) {
      int val = evaluate(p);
      int curval = i_memory_get(state.i_memory, ca>>1);
      int mask = 0xff <<((ca&1) ? 0 : 8);

      if(val > 0xff) {
	gpwarning(GPW_RANGE,0);
      }
      /* If the config address is even, then this byte goes in LSB position */
      val = (val & 0xff) << ((ca&1) ? 8 : 0)  | MEM_USED_MASK; 

      if(curval & MEM_USED_MASK)
	curval &= mask;
      else
	curval |= mask;

      i_memory_put(state.i_memory, ca>>1,  curval | val);

    } else {
      emit(evaluate(p));
    }
  }
  /* return to the org before the config was called */
  state.org = org;
        
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
  data(&list, 0, DEFAULT_LIT_MASK);

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

  if(_16bit_core) {
    data(&list, 0, PACKING_BYTES);
  } else {
    data(&list, 0, ENDIAN_SWAP | PACKING_BYTES);
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
  data(&list, 0, DEFAULT_LIT_MASK);

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
  int temp_expand;
  int count = 1; 

  assert(!state.mac_head);
  if (state.mac_prev == NULL)
    gperror(GPE_ILLEGAL_COND, "Illegal condition (ENDW).");
  else {
    state.mac_prev = NULL;

    temp_expand = state.lst.expand;
    if (state.stGlobal != state.stTop) {
      state.lst.expand = 0;      /* don't expand while loops within macros */
    }

    /* Now actually do the WHILE body */
    while (maybe_evaluate(state.while_head->parms)) {
      execute_body(state.while_head);
      if (count > 255) {
        gperror(GPE_BAD_WHILE_LOOP, NULL);
        break;
      }
      count++;
    }
    
    state.lst.expand = temp_expand;
  }

  state.lst.line.linetype = dir;
  state.mac_body = NULL;
  state.while_head = NULL;

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

    #ifdef __DEBUG__
      print_errorcodes();  
    #endif
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
      state.next_state = _exitmacro;
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
  if (state.cmd_line.macro_expand == 1){
    gpmessage(GPM_SUPLIN, NULL);
  } else {
    if (enforce_arity(arity, 0)) {
      state.lst.expand = 1;
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

static gpasmVal do_idlocs(gpasmVal r,
		          char *name,
		          int arity,
		          struct pnode *parms)
{

  int org;
  int value;
  int idreg;
  int curvalue;
  int mask;

  /* save the current org to restore to later */
  org = state.org;

  if (_17cxx_core) {
    gperror(GPE_ILLEGAL_DIR, NULL);

  } else if (_16bit_core) {
    state.lst.line.linetype = config;

    if (enforce_arity(arity, 2)) {

      idreg = maybe_evaluate(HEAD(parms));

      if (idreg > 0x200007 || idreg < 0x200000) {
        gperror(GPE_RANGE,NULL);
      } else {

        state.device.id_location = idreg;
        state.lst.config_address = idreg;
        state.org = idreg;
      
        value = maybe_evaluate(HEAD(TAIL(parms)));
        
        if(value > 0xff) {
          gpwarning(GPW_RANGE, NULL);
	}
        curvalue = i_memory_get(state.i_memory, idreg>>1);
        mask = 0xff <<((idreg&1) ? 0 : 8);
	
        /* If the config address is even, then this byte goes in LSB position */
        value = (value & 0xff) << ((idreg&1) ? 8 : 0)  | MEM_USED_MASK; 

        if(curvalue & MEM_USED_MASK)
          curvalue &= mask;
        else
          curvalue |= mask;

        i_memory_put(state.i_memory, idreg>>1,  curvalue | value);

      }

    }

  } else {

    if (enforce_arity(arity, 1)) {
      state.org = state.device.id_location;
      state.lst.line.linetype = idlocs;
      value = maybe_evaluate(HEAD(parms));
      if (value > 0xffff) {
        gpmessage(GPM_IDLOC, NULL);
	value &= 0xffff;
      }     
      emit((value & 0xf000) >> 12);
      emit((value & 0x0f00) >> 8);
      emit((value & 0x00f0) >> 4);
      emit((value & 0x000f));
    }

  }

  /* return to the org before the __idlocs was called */
  state.org = org;
 
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
      state.next_state = _include;  
      state.next_buffer.file = strdup(p->value.string);
    } else if (p->tag == symbol) {
      /* This is a bad coding style. "" or <> should enclose all filenames */
      state.next_state = _include;  
      state.next_buffer.file = strdup(p->value.symbol);    
    } else {
      gperror(GPE_ILLEGAL_ARGU, NULL);
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

  state.lst.enabled = 1;
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
	} else if (strcasecmp(lhs, "r") == 0) {
	  if (enforce_simple(p->value.binop.p1))
	    select_radix(p->value.binop.p1->value.symbol);
	} else if (strcasecmp(lhs, "st") == 0) {
	  state.lst.symboltable = off_or_on(p->value.binop.p1);
	} else if (strcasecmp(lhs, "t") == 0) {
	  ; /* Ignore this for now: always wrap long list lines */
	} else if (strcasecmp(lhs, "w") == 0) {
          select_errorlevel(p->value.binop.p1->value.constant);
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
  if (state.cmd_line.macro_expand == 1){
    gpmessage(GPM_SUPLIN, NULL);
  } else {
    if (enforce_arity(arity, 0)) {
      state.lst.expand = 0;
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
    state.lst.enabled = 0;
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

static gpasmVal do_macro(gpasmVal r,
			 char *name,
			 int arity,
			 struct pnode *parms)
{
  struct macro_head *head = malloc(sizeof(*head));

  head->parms = parms;
  head->body = NULL;
  head->defined = 0;
  /* Record data for the list and cod files */
  head->line_number = state.src->line_number;
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
    if (can_evaluate(p))
      r = state.org = evaluate(p) >> _16bit_core;
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
  
  if ((state.device.class == PROC_CLASS_EEPROM8) ||
      (state.device.class == PROC_CLASS_PIC16E)) {
    /* do nothing */
    return r;
  }

  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (p->tag != symbol) {
      gperror(GPE_ILLEGAL_LABEL, NULL);
    } else {
      r = set_page_bits(check_page(maybe_evaluate(p)));
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

  state.lst.line.linetype = equ;
  if (enforce_arity(arity, 1)) {
    p = HEAD(parms);
    if (can_evaluate(p)) {
      r = state.org;
      state.org += (evaluate(p) >> _16bit_core);
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

  if (state.while_head != NULL) {
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
  /* Record data for the list and cod files */
  head->line_number = state.src->line_number;
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
    sprintf(full_message,"Argument out of range (%d not between %d and %d)\n",
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
	  emit_check(i->opcode, (maybe_evaluate(p) >> 5), 0x07);
	}
	break;
      case INSN_CLASS_LIT3_PAGE:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  emit_check(i->opcode, (maybe_evaluate(p) >> 9), 0x07);
	}
	break;
      case INSN_CLASS_LIT1:
	{
	  int s = 0;
	  switch (arity) {
	  case 1:
	    s = check_flag(maybe_evaluate(HEAD(parms)));
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
	  emit_check(i->opcode, maybe_evaluate(p), 0x0f);
	}
	break;
      case INSN_CLASS_LIT4S:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  emit_check(i->opcode, (maybe_evaluate(p) << 4), 0xf0);
	}
	break;
      case INSN_CLASS_LIT8:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  emit_check(i->opcode, maybe_evaluate(p), 0xff);
	}
	break;
      case INSN_CLASS_LIT8C12:
	if (enforce_arity(arity, 1)) {
	  int value;

	  p = HEAD(parms);
	  value = maybe_evaluate(p);	  

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
	  value = maybe_evaluate(p);	  

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
	  value = maybe_evaluate(p);

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
	  value = maybe_evaluate(p);
	  
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
	  value = maybe_evaluate(p);

	  /* PC is 16 bits.  mpasm checks the maximum device address. */
	  if (value & (~0xffff))
	    gperror(GPE_RANGE, NULL); 	  

	  if ((value & 0xe000) != (state.org & 0xe000))
	    gpmessage(GPM_PAGE, NULL);

	  emit(i->opcode | (value & 0x1fff));
	}
	break;
      case INSN_CLASS_RBRA8:
	if (enforce_arity(arity, 1)) {
	  int offset = ((maybe_evaluate( HEAD(parms)) ) - 
			((state.org + 1)<<_16bit_core));
	  offset >>= _16bit_core;

	  /* The offset for the relative branch must be 
	     between -127 <= offset <= 127. */
	  emit_check_relative(i->opcode, offset, 0xff, 127);
	}
	break;
      case INSN_CLASS_RBRA11:
	if (enforce_arity(arity, 1)) {
	  int offset = ((maybe_evaluate( HEAD(parms)) ) - 
			((state.org + 1)<<_16bit_core));
	  offset >>= _16bit_core;
	  emit_check_relative(i->opcode, offset, 0x7ff, 0x3ff);
	}
	break;
      case INSN_CLASS_LIT20:

	if (enforce_arity(arity, 1)) {
	  int dest;
	  p = HEAD(parms);
	  dest = maybe_evaluate(p) >> _16bit_core;
	  emit_check(i->opcode, dest, 0xff);
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
	  dest = maybe_evaluate(p) >> _16bit_core;
	  emit_check(i->opcode | (s<<8), (dest&0xff), 0xff);
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
	    k =  maybe_evaluate(p);
	    emit_check(i->opcode | ((file & 3) << 4), (k>>8), 0xf);
	    k &= 0xff;
	    emit_check(0xf000, k, 0xff);
	  }
	}
	break;

      case INSN_CLASS_FF:
	if (enforce_arity(arity, 2)) {

	  emit_check(i->opcode, maybe_evaluate(HEAD(parms)), 0xfff);
	  emit_check(0xf000, maybe_evaluate(HEAD(TAIL(parms))), 0xfff);

	}
	break;
      case INSN_CLASS_FP:
        if (enforce_arity(arity, 2)) {
          int reg=0;
          file=maybe_evaluate(HEAD(parms));
          reg=maybe_evaluate(HEAD(TAIL(parms)));
          file_ok(file);
          file_ok(reg);
          emit(i->opcode | ( (reg  & 0x1f) << 8) |
              (file & 0xff) );
        }
        break;
      case INSN_CLASS_PF:
        if (enforce_arity(arity, 2)) {
          int reg=0;
          reg=maybe_evaluate(HEAD(parms));
          file=maybe_evaluate(HEAD(TAIL(parms)));
          file_ok(file);
          file_ok(reg);
          emit(i->opcode | ( (reg & 0x1f) << 8) |
              (file & 0xff) );
        }
        break;
      case INSN_CLASS_OPF5:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  file = maybe_evaluate(p);
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
	  file = maybe_evaluate(p);
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
	    file = maybe_evaluate(f);
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
	    file = maybe_evaluate(f);
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
          if (strcasecmp(i->name, "tris") == 0) {
            gpwarning(GPW_NOT_RECOMMENDED, NULL);
          }
	  p = HEAD(parms);
	  file = maybe_evaluate(p);
	  file_ok(file);
	  emit(i->opcode | (file & 0x7f));
	}
	break;

      case INSN_CLASS_OPF8:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  file = maybe_evaluate(p);
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
	  file = maybe_evaluate(p);
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
	  file = maybe_evaluate(p);
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
	    file = maybe_evaluate(f);
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
	  file = maybe_evaluate(p);
	  file_ok(file);

	  /* Default access (use the BSR unless access is to special registers) */
	  if ((file < 0x60) || (file > 0xf5f)) {
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
	    
	    f = HEAD(parms);
	    b = HEAD(TAIL(parms));
	    file = maybe_evaluate(f);
	    bit = maybe_evaluate(b);
	    if (!((0 <= bit) && (bit <= 7)))
              gpwarning(GPW_RANGE, NULL);
	    file_ok(file);
	    emit(i->opcode | ( a << 8) | ((bit & 7) << 9) | (file & 0xff));
	    break;
	  default:
	    enforce_arity(arity, 3);
	  }
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
	  file = maybe_evaluate(p);
	  file_ok(file);

	  /* Default access (use the BSR unless access is to special registers) */
	  if ((file < 0x60) || (file > 0xf5f)) {
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
	  file = maybe_evaluate(p2);

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
	  file = maybe_evaluate(p3);

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
          sprintf(mesg, "Unknown opcode \"%.40s\"", name);
          gperror(GPE_UNKNOWN, mesg);
        }
      }

    }
  }

  return r;
}

void execute_body(struct macro_head *h)
{	
  struct source_context *new, *old; 
  struct macro_body *b = h->body;
  int expand;

  /* determine if the macro listing is to be expanded */
  if (state.pass == 2 && state.lst.enabled == 1) {
    if (state.stGlobal == state.stTop) {
      expand = 1;      /* always expand while loops */
    } else if (state.lst.expand == 1) {
      expand = 1;      /* only expand macros when commanded */
    } else {
      expand = 0;
    }
  } else {
    expand = 0;
  }

  if (expand) {
    /* setup a new source context for the macro */
    new = (struct source_context *)malloc(sizeof(*new));
    new->name = strdup(h->src_name);  
    new->line_number = h->line_number;
    new->prev = state.src;
    state.src = new;
    state.src->fc = add_file(ft_src, new->name); /* scan list for fc */
  }

  /* Execute the body of the macro, line by line */
  for (; b; b = b->next) {
    gpasmVal r;

    if (expand) {    
      state.src->line_number++;
      state.lst.line.linetype = none; /* default lst line type */
      state.lst.line.was_org = state.org;
    }    

    if (b->op) {
      /* Easy place to catch 'EXITM' */
      if (strcasecmp(b->op, "EXITM") == 0)
	break;
      else {
        r = do_or_append_insn(b->op, b->parms);
      }
    } else {
      r = state.org;
    }
    if (asm_enabled() && b->label) {
      if (state.mac_prev) {
	state.mac_body->label = b->label;
      } else {
        if ((b->op != NULL) && strcasecmp(b->op, "SET") == 0) {
	  set_global(b->label, r, TEMPORARY, gvt_constant);
	} else {
          set_global(b->label, r << _16bit_core, PERMANENT, gvt_constant);
	}
      }
    }
    if (expand && (b->src_line != NULL)) {
      lst_format_line(b->src_line, r);
    } 
  }

  if (expand) {
    /* restore conditions before the macro call */
    state.lst.line.was_org = state.org;
    old = state.src;
    state.src = state.src->prev;
    free(old);
  
    state.lst.line.linetype = none;
  } else {
    state.lst.line.linetype = insn;
  }

}

/************************************************************************/

/* There are several groups of operations that we handle here.  First
   is op_0: the instructions that can happen before the processor type
   is known.  Second is op_1, the set of instructions that are common
   to all processors, third is processor-family specific: op_XXX */

/* Note that instructions within each group are sorted alphabetically */

struct insn op_0[] = {
  { "constant",   0, (long int)do_constant,  INSN_CLASS_FUNC,   0 },
  { "else",       0, (long int)do_else,      INSN_CLASS_FUNC,   ATTRIB_COND },
  { "endif",      0, (long int)do_endif,     INSN_CLASS_FUNC,   ATTRIB_COND },
  { "endm",       0, (long int)do_endm,	     INSN_CLASS_FUNC,   0 },
  { "endw",       0, (long int)do_endw,	     INSN_CLASS_FUNC,   0 },
  { "equ",        0, (long int)do_equ,       INSN_CLASS_FUNC,   0 },
  { "error",      0, (long int)do_error,     INSN_CLASS_FUNC,   0 },
  { "exitm",      0, (long int)do_exitm,     INSN_CLASS_FUNC,   0 },
  { "expand",     0, (long int)do_expand,    INSN_CLASS_FUNC,   0 },
  { "errorlevel", 0, (long int)do_errlvl,    INSN_CLASS_FUNC,   0 },
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
  { "variable",   0, (long int)do_variable,  INSN_CLASS_FUNC,   0 },
  { "while",      0, (long int)do_while,     INSN_CLASS_FUNC,   0 },
  { ".set",       0, (long int)do_set, 	     INSN_CLASS_FUNC,   0 },
  { "#else",      0, (long int)do_else,	     INSN_CLASS_FUNC,   ATTRIB_COND },
  { "#endif",     0, (long int)do_endif,     INSN_CLASS_FUNC,   ATTRIB_COND },
  { "#ifdef",     0, (long int)do_ifdef,     INSN_CLASS_FUNC,   ATTRIB_COND },
  { "#ifndef",    0, (long int)do_ifndef,    INSN_CLASS_FUNC,   ATTRIB_COND },
  { "#include",   0, (long int)do_include,   INSN_CLASS_FUNC,   0 },
  { "#undefine",  0, (long int)do_undefine,  INSN_CLASS_FUNC,   0 }
};

const int num_op_0 = TABLE_SIZE(op_0);

struct insn op_1[] = {
  { "__badram",   0, (long int)do_badram,    INSN_CLASS_FUNC,   0 },
  { "__config",   0, (long int)do_config,    INSN_CLASS_FUNC,   0 },
  { "__fuses",    0, (long int)do_config,    INSN_CLASS_FUNC,   0 },
  { "__idlocs",   0, (long int)do_idlocs,    INSN_CLASS_FUNC,   0 },
  { "__maxram",   0, (long int)do_maxram,    INSN_CLASS_FUNC,   0 },
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

      /* The 16_bit core special macros are encoded directly into the
       * symbol table like regular instructions. */
      for (i = 0; i < num_op_18cxx_sp; i++)
	annotate_symbol( add_symbol(state.stBuiltin, op_18cxx_sp[i].name), 
			 &op_18cxx_sp[i]);

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
  state.cblock = maybe_evaluate(c);
}

void cblock_expr(struct pnode *s)
{
  if (can_evaluate_concatenation(s)) {
    set_global(evaluate_concatenation(s), state.cblock, PERMANENT, gvt_cblock);
    state.cblock++;
  }
}

void cblock_expr_incr(struct pnode *s, struct pnode *incr)
{
  if (can_evaluate_concatenation(s)) {
    set_global(evaluate_concatenation(s), state.cblock, PERMANENT, gvt_cblock);
    state.cblock += maybe_evaluate(incr);
  }
}
