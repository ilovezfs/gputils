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
#include "directive.h"
#include "evaluate.h"
#include "parse.h"
#include "lst.h"
#include "macro.h"
#include "gperror.h"
#include "gpmemory.h"
#include "special.h"

/* Forward declarations */
void execute_body(struct macro_head *h);

/* XXXPRO: new class of processor may require new instruction classes */
enum insn_class {
  INSN_CLASS_LIT1,	/* bit 0 contains a 1 bit literal		*/
  INSN_CLASS_LIT4S,     /* Bits 7:4 contain a 4 bit literal, bits 3:0 are unused   */
  INSN_CLASS_LIT8,	/* bits 7:0 contain an 8 bit literal		*/
  INSN_CLASS_LIT9,	/* bits 8:0 contain a 9 bit literal		*/
  INSN_CLASS_LIT11,	/* bits 10:0 contain an 11 bit literal		*/
  INSN_CLASS_LIT13,	/* bits 12:0 contain an 11 bit literal		*/
  INSN_CLASS_IMPLICIT,	/* instruction has no variable bits at all	*/
  INSN_CLASS_OPF5,	/* bits 4:0 contain a register address		*/
  INSN_CLASS_OPWF5,	/* as above, but bit 5 has a destination flag	*/
  INSN_CLASS_B5,	/* as for OPF5, but bits 7:5 have bit number	*/
  INSN_CLASS_OPF7,	/* bits 6:0 contain a register address		*/
  INSN_CLASS_OPWF7,	/* as above, but bit 7 has destination flag	*/
  INSN_CLASS_B7,	/* as for OPF7, but bits 9:7 have bit number	*/

  INSN_CLASS_OPF8,	/* bits 7:0 contain a register address 	*/
  INSN_CLASS_OPFA8,	/* bits 7:0 contain a register address & bit has access flag  */
  INSN_CLASS_OPWF8,	/* as above, but bit 8 has dest flag	*/
  INSN_CLASS_OPWFA8,	/* as above, but bit 9 has dest flag & bit 8 has access flag		*/
  INSN_CLASS_B8,	/* like OPF7, but bits 9:11 have bit number	*/
  INSN_CLASS_BA8,	/* like OPF7, but bits 9:11 have bit number & bit 8 has access flag	*/
  INSN_CLASS_LIT20,	/* 20bit lit, bits 7:0 in first word bits 19:8 in second		*/
  INSN_CLASS_CALL20,	/* Like LIT20, but bit 8 has fast push flag			 	*/
  INSN_CLASS_RBRA8,	/* Bits 7:0 contain a relative branch address			 	*/
  INSN_CLASS_RBRA11,	/* Bits 10:0 contain a relative branch address			 	*/
  INSN_CLASS_FLIT12,	/* LFSR, 12bit lit loaded into 1 of 4 FSRs				*/
  INSN_CLASS_FF,	/* two 12bit file addresses						*/
  INSN_CLASS_FP,        /* Bits 7:0 contain a register address, bits 12:8 contains a peripheral address    */
  INSN_CLASS_PF,        /* Bits 7:0 contain a register address, bits 12:8 contains a peripheral address    */
  INSN_CLASS_TBL,	/* a table read or write instruction	      	*/
  INSN_CLASS_TBL2,	/* a table read or write instruction. 
			   Bits 7:0 contains a register address; Bit 8 is unused;
			   Bit 9, table byte select. (0:lower ; 1:upper) */
  INSN_CLASS_TBL3,	/* a table read or write instruction. 
			   Bits 7:0 contains a register address; 
			   Bit 8, 1 if increment pointer, 0 otherwise;
			   Bit 9, table byte select. (0:lower ; 1:upper) */
  INSN_CLASS_FUNC,	/* instruction is an assembler function		*/
  INSN_CLASS_LIT3_BANK, /* SX: bits 3:0 contain a 3 bit literal, shifted 5 bits */
  INSN_CLASS_LIT3_PAGE, /* SX: bits 3:0 contain a 3 bit literal, shifted 9 bits */
  INSN_CLASS_LIT4       /* SX: bits 3:0 contain a 4 bit literal         */
};

#define ATTRIB_COND 1

/************************************************************************/
/* create flags that control the behavior of the table macros like data,
 * dt,de, and dw */

#define DEFAULT_LIT_MASK 0xffff
#define ENDIAN_SWAP      (1<<31)
#define PACKING_BYTES    (1<<30)

struct insn {
  char *name;
  long int mask;
  enum insn_class class;
  int attribs;
};

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
int check_page(struct pnode *p)
{
  int number, page;

  number = maybe_evaluate(HEAD(p));

  if (state.device.core_size == CORE_12BIT_MASK) {
    if (number < 0x200) {
      page = 0;
    } else if (number < 0x400) {
      page = 1;
    } else if (number < 0x600) {
      page = 2;
    } else {
      page = 3;
    } 
  } else {
    if (number < 0x800) {
      page = 0;
    } else if (number < 0x1000) {
      page = 1;
    } else if (number < 0x1800) {
      page = 2;
    } else {
      page = 3;
    }   
  }
  
  return page;
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
	    (MAX_RAM <= loc))
	  gpwarning(GPW_INVALID_RAM, NULL);
	else
	  state.badram[loc] = 1;
      }
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
      case CONFIG3H:
      case CONFIG4L:
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

      if(val > 0xff)
	gpwarning(GPW_RANGE,0);
	
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
  data(&list, 0, 0xff);


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
  data(&list, i->mask, 0xff);

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
        
        if(value > 0xff)
          gpwarning(GPW_RANGE, NULL);
	
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
  /* Issue bank bit warning if necessary*/
  return;
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
	  emit(i->mask | ((maybe_evaluate(p) >> 5) & 0x07));
	  /* should issue warning if out of range */
	}
	break;
      case INSN_CLASS_LIT3_PAGE:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  emit(i->mask | ((maybe_evaluate(p) >> 9) & 0x07));
	  /* should issue warning if out of range */
	}
	break;
      case INSN_CLASS_LIT1:
	{
	  int s = 0;
	  switch (arity) {
	  case 1:
	    s = maybe_evaluate(HEAD(parms)) & 1;
	  case 0:
	    emit(i->mask | s);
	    break;
	  default:
	    enforce_arity(arity, 1);
	  }
	}
	break;
      case INSN_CLASS_LIT4:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  emit(i->mask | (maybe_evaluate(p) & 0x0f));
	  /* should issue warning if out of range */
	}
	break;
      case INSN_CLASS_LIT4S:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  emit(i->mask | ( (maybe_evaluate(p) & 0x0f) << 4) );
	  /* should issue warning if out of range */
	}
	break;
      case INSN_CLASS_LIT8:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  emit(i->mask | (maybe_evaluate(p) & 0xff));
	}
	break;
      case INSN_CLASS_LIT9:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  emit(i->mask | (maybe_evaluate(p) & 0x1ff));
	}
	break;
      case INSN_CLASS_LIT11:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  emit(i->mask | (maybe_evaluate(p) & 0x7ff));
	}
	break;
      case INSN_CLASS_LIT13:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  emit(i->mask | (maybe_evaluate(p) & 0x1fff));
	}
	break;
      case INSN_CLASS_RBRA8:
	if (enforce_arity(arity, 1)) {
	  emit(i->mask | 
	       ((((maybe_evaluate( HEAD(parms)) ) - 
		 ((state.org + 1)<<_16bit_core)) >> _16bit_core)  & 0xff));
	}
	break;
      case INSN_CLASS_RBRA11:
	if (enforce_arity(arity, 1)) {
	  emit(i->mask | 
	       ((((maybe_evaluate( HEAD(parms)) ) - 
		 ((state.org + 1)<<_16bit_core)) >> _16bit_core)  & 0x7ff));
	}
	break;
      case INSN_CLASS_LIT20:

	if (enforce_arity(arity, 1)) {
	  int dest;
	  p = HEAD(parms);
	  dest = maybe_evaluate(p) >> _16bit_core;
	  emit(i->mask | (dest & 0xff));
	  emit(0xf000 |  ( (dest>>8) & 0xfff));
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
	      s = maybe_evaluate(p2);
	    break;
	  case 1:
	    s = 0;
	    break;
	  default:
	    enforce_arity(arity, 2);
	  }
	  dest = maybe_evaluate(p) >> _16bit_core;
	  emit(i->mask | (s<<8) |(dest & 0xff));
	  emit(0xf000 |  ( (dest>>8) & 0xfff));

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

	    emit(i->mask | ( ((file & 3) <<4) | ((k>>8) & 0xf) ) );
	    emit(0xf000 |  (k & 0xff));

	  }
	}
	break;

      case INSN_CLASS_FF:
	if (enforce_arity(arity, 2)) {

	  emit(i->mask | (maybe_evaluate( HEAD(parms)) & 0xfff));
	  emit(0xf000  | (maybe_evaluate( HEAD(TAIL(parms))) & 0xfff));

	}
	break;
      case INSN_CLASS_FP:
        if (enforce_arity(arity, 2)) {
          int reg=0;
          file=maybe_evaluate(HEAD(parms));
          reg=maybe_evaluate(HEAD(TAIL(parms)));
          file_ok(file);
          file_ok(reg);
          emit(i->mask | ( (reg  & 0x1f) << 8) |
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
          emit(i->mask | ( (reg & 0x1f) << 8) |
              (file & 0xff) );
        }
        break;
      case INSN_CLASS_OPF5:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  file = maybe_evaluate(p);
	  file_ok(file);
	  emit(i->mask | (file & 0x1f));
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
	      d = maybe_evaluate(p2);
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
	  emit(i->mask | (d << 5) | (file & 0x1f));
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
	    emit(i->mask | ((bit & 7) << 5) |(file & 0x1f));
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
	    emit(i->mask | ((bit & 7) << 8) | (file & 0xff));
	  }
	}
	break;

      case INSN_CLASS_OPF7:
	if (enforce_arity(arity, 1)) {
          if (i->name == "TRIS"){
            gpwarning(GPW_NOT_RECOMMENDED, NULL);
          }
	  p = HEAD(parms);
	  file = maybe_evaluate(p);
	  file_ok(file);
	  emit(i->mask | (file & 0x7f));
	}
	break;

      case INSN_CLASS_OPF8:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  file = maybe_evaluate(p);
	  file_ok(file);
	  emit(i->mask | (file & 0xff));
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
	      d = maybe_evaluate(p2);
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
	  emit(i->mask | (d << 7) | (file & 0x7f));
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
	      d = maybe_evaluate(p2);
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
	  emit(i->mask | (d << 8) | (file & 0xff));
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
	    emit(i->mask | ((bit & 7) << 7) | (file & 0x7f));
	  }
	}
	break;

      case INSN_CLASS_OPFA8:
	{
	  int a = 0; /* Default access (don't use the BSR) */
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
		(strcasecmp(p2->value.symbol, "b") == 0))
	      a = 1;
	    else
	      a = maybe_evaluate(p2);
	    break;
	  case 1:
	    /* use default a */
	    break;
	  default:
	    enforce_arity(arity, 2);
	  }
	  file = maybe_evaluate(p);
	  file_ok(file);
	  emit(i->mask | (a << 8) | (file & 0xff));
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
	      a = maybe_evaluate(par);
	    /* fall through */
	  case 2:
	    
	    f = HEAD(parms);
	    b = HEAD(TAIL(parms));
	    file = maybe_evaluate(f);
	    bit = maybe_evaluate(b);
	    if (!((0 <= bit) && (bit <= 7)))
              gpwarning(GPW_RANGE, NULL);
	    file_ok(file);
	    emit(i->mask | ( a << 8) | ((bit & 7) << 9) | (file & 0xff));
	    break;
	  default:
	    enforce_arity(arity, 3);
	  }
	}
	break;

      case INSN_CLASS_OPWFA8:
	{
	  int d = 1; /* Default destination of 1 (file) */
	  int a = 0; /* Default access bank (don't use BSR) */
	  struct pnode *par; /* second parameter */
	    
	  if(arity == 0) {
	    enforce_arity(arity, 2);
	    break;
	  }


	  p = HEAD(parms);
	  switch (arity) {
	  case 3:
	    par = HEAD(TAIL(TAIL(parms)));

	    if ((par->tag == symbol) &&
		(strcasecmp(par->value.symbol, "b") == 0))
	      a = 1;
	    else
	      a = maybe_evaluate(par);
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
	      d = maybe_evaluate(par);
	    break;
	  case 1:
            /* use default a and d */
            gpmessage(GPM_NOF,NULL);
            break;
	  default:
            enforce_arity(arity, 3);
	  }
	  file = maybe_evaluate(p);
	  file_ok(file);
	  emit(i->mask | (d << 9) | (a << 8) | (file & 0xff));
	}
	break;

      case INSN_CLASS_IMPLICIT:
	if (arity != 0) {
          gpwarning(GPW_EXTRANEOUS, NULL);
	}
        if ((i->name == "OPTION") && 
            (state.device.core_size != CORE_12BIT_MASK)){
          gpwarning(GPW_NOT_RECOMMENDED, NULL);
        }
	emit(i->mask);
	break;
      case INSN_CLASS_TBL:
	if (enforce_arity(arity, 1)) {
	  p = HEAD(parms);
	  switch(maybe_evaluate(p))
	    {
	    case TBL_NO_CHANGE:
	      emit(i->mask);
	      break;
	    case TBL_POST_INC:
	      emit(i->mask | 1);
	      break;
	    case TBL_POST_DEC:
	      emit(i->mask | 2);
	      break;
	    case TBL_PRE_INC:
	      emit(i->mask | 3);
	      break;
	    default:
	      printf("tbl: %x\n",maybe_evaluate(p));
	    }
	}
	break;

      case INSN_CLASS_TBL2:
	if (enforce_arity(arity, 2)) {
	  int t=0; /* read low byte by default */
	  struct pnode *p2; /* second parameter */

	  /* "0" (lower byte) and "1" (upper byte) */
	  p = HEAD(parms);
	  t = maybe_evaluate(p);

	  p2 = HEAD(TAIL(parms));
	  file = maybe_evaluate(p2);

	  file_ok(file);
          emit(i->mask | (t << 9) | (file & 0xff));
	}
	break;

      case INSN_CLASS_TBL3:
	if (enforce_arity(arity, 3)) {
	  int inc=0,t=0;
	  struct pnode *p2; /* second parameter */
	  struct pnode *p3; /* third parameter */

	  /* "0" (lower byte) and "1" (upper byte) */
	  p = HEAD(parms);
	  t = maybe_evaluate(p);

	  /* "0" (no change) and "1" (postincrement) */
	  p2 = HEAD(TAIL(parms));
	  inc = maybe_evaluate(p2);

	  p3 = HEAD(TAIL(TAIL(parms)));
	  file = maybe_evaluate(p3);

	  file_ok(file);          
          emit(i->mask | (t << 9) | (inc << 8) | (file & 0xff));
	}
	break;

      case INSN_CLASS_FUNC:
	r = (*(opfunc*)i->mask)(r, name, arity, parms);
	break;
      }
    }
  } else {
    s = get_symbol(state.stMacros, name);
    if (s) {
      /* Found the macro: execute it */
      if (asm_enabled())
	setup_macro(get_symbol_annotation(s), arity, parms);
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

static struct insn  op_0[] = {
  { "constant", (long int)do_constant,  INSN_CLASS_FUNC,	0 },
  { "else",	(long int)do_else, 	INSN_CLASS_FUNC,	ATTRIB_COND },
  { "endif",	(long int)do_endif,	INSN_CLASS_FUNC,	ATTRIB_COND },
  { "endm",	(long int)do_endm,	INSN_CLASS_FUNC,	0 },
  { "endw",	(long int)do_endw,	INSN_CLASS_FUNC,	0 },
  { "equ",	(long int)do_equ,  	INSN_CLASS_FUNC,	0 },
  { "error",	(long int)do_error, 	INSN_CLASS_FUNC,	0 },
  { "exitm",	(long int)do_exitm,     INSN_CLASS_FUNC,	0 },
  { "expand",	(long int)do_expand,    INSN_CLASS_FUNC,	0 },
  { "errorlevel", (long int)do_errlvl,  INSN_CLASS_FUNC,	0 },
  { "if",	(long int)do_if,   	INSN_CLASS_FUNC,	ATTRIB_COND },
  { "ifdef",	(long int)do_ifdef, 	INSN_CLASS_FUNC,	ATTRIB_COND },
  { "ifndef",	(long int)do_ifndef,	INSN_CLASS_FUNC,	ATTRIB_COND },
  { "include",	(long int)do_include, 	INSN_CLASS_FUNC,	0 },
  { "list",	(long int)do_list, 	INSN_CLASS_FUNC,	0 },
  { "local",	(long int)do_local, 	INSN_CLASS_FUNC,	0 },
  { "macro",	(long int)do_macro,	INSN_CLASS_FUNC,	0 },
  { "messg",	(long int)do_messg,	INSN_CLASS_FUNC,	0 },
  { "noexpand", (long int)do_noexpand,  INSN_CLASS_FUNC,	0 },
  { "nolist",	(long int)do_nolist,    INSN_CLASS_FUNC,	0 },
  { "page",	(long int)do_page,      INSN_CLASS_FUNC,	0 },
  { "processor",(long int)do_processor, INSN_CLASS_FUNC,	0 },
  { "radix",	(long int)do_radix, 	INSN_CLASS_FUNC,	0 },
  { "set",	(long int)do_set, 	INSN_CLASS_FUNC,	0 },
  { "space",	(long int)do_space,     INSN_CLASS_FUNC,	0 },
  { "variable", (long int)do_variable,  INSN_CLASS_FUNC,	0 },
  { "while",	(long int)do_while, 	INSN_CLASS_FUNC,	0 },
  { ".set",	(long int)do_set, 	INSN_CLASS_FUNC,	0 },
  { "#else",	(long int)do_else,	INSN_CLASS_FUNC,	ATTRIB_COND },
  { "#endif",	(long int)do_endif,	INSN_CLASS_FUNC,	ATTRIB_COND },
  { "#ifdef",	(long int)do_ifdef, 	INSN_CLASS_FUNC,	ATTRIB_COND },
  { "#ifndef",	(long int)do_ifndef,	INSN_CLASS_FUNC,	ATTRIB_COND },
  { "#include",	(long int)do_include, 	INSN_CLASS_FUNC,	0 },
  { "#undefine", (long int)do_undefine,	INSN_CLASS_FUNC,	0 }
};

static struct insn  op_1[] = {
  { "__badram", (long int)do_badram,    INSN_CLASS_FUNC,	0 },
  { "__config", (long int)do_config,    INSN_CLASS_FUNC,	0 },
  { "__fuses",  (long int)do_config,    INSN_CLASS_FUNC,	0 },
  { "__idlocs", (long int)do_idlocs,    INSN_CLASS_FUNC,	0 },
  { "__maxram", (long int)do_maxram,    INSN_CLASS_FUNC,	0 },
  { "data",     (long int)do_data, 	INSN_CLASS_FUNC,	0 },
  { "da",       (long int)do_da, 	INSN_CLASS_FUNC,	0 },
  { "db",       (long int)do_db, 	INSN_CLASS_FUNC,	0 },
  { "de",       (long int)do_de, 	INSN_CLASS_FUNC,	0 },
  { "dt",       (long int)do_dt, 	INSN_CLASS_FUNC,	0 },
  { "dw",       (long int)do_dw, 	INSN_CLASS_FUNC,	0 },
  { "fill",	(long int)do_fill,  	INSN_CLASS_FUNC,	0 },
  { "org",	(long int)do_org,       INSN_CLASS_FUNC,	0 },
  { "res",	(long int)do_res,       INSN_CLASS_FUNC,	0 }
};

/* XXXPRO: Need to add a struct here for any extra processor classes. */

/* PIC 12-bit instruction set */
static struct insn  op_12c5xx[] = {
  { "ADDWF",  0x1c0,	INSN_CLASS_OPWF5 	},
  { "ANDLW",  0xe00,	INSN_CLASS_LIT8 	},
  { "ANDWF",  0x140,	INSN_CLASS_OPWF5 	},
  { "BCF",    0x400,	INSN_CLASS_B5 		},
  { "BSF",    0x500,	INSN_CLASS_B5 		},
  { "BTFSC",  0x600,	INSN_CLASS_B5 		},
  { "BTFSS",  0x700,	INSN_CLASS_B5		},
  { "CALL",   0x900,	INSN_CLASS_LIT8 	},
  { "CLRF",   0x060,	INSN_CLASS_OPF5 	},
  { "CLRW",   0x040,	INSN_CLASS_IMPLICIT 	},
  { "CLRWDT", 0x004, 	INSN_CLASS_IMPLICIT 	},
  { "COMF",   0x240,	INSN_CLASS_OPWF5 	},
  { "DECF",   0x0c0,	INSN_CLASS_OPWF5 	},
  { "DECFSZ", 0x2c0,	INSN_CLASS_OPWF5 	},
  { "GOTO",   0xa00,	INSN_CLASS_LIT9 	},
  { "INCF",   0x280,	INSN_CLASS_OPWF5 	},
  { "INCFSZ", 0x3c0,	INSN_CLASS_OPWF5 	},
  { "IORLW",  0xd00,	INSN_CLASS_LIT8 	},
  { "IORWF",  0x100,	INSN_CLASS_OPWF5 	},
  { "MOVF",   0x200,	INSN_CLASS_OPWF5 	},
  { "MOVLW",  0xc00,	INSN_CLASS_LIT8 	},
  { "MOVWF",  0x020,	INSN_CLASS_OPF5 	},
  { "NOP",    0x000,	INSN_CLASS_IMPLICIT 	},
  { "OPTION", 0x002,	INSN_CLASS_IMPLICIT 	},
  { "RETLW",  0x800,	INSN_CLASS_LIT8 	},
  { "RETURN", 0x800,	INSN_CLASS_IMPLICIT 	}, /* special mnemonic */
  { "RLF",    0x340,	INSN_CLASS_OPWF5 	},
  { "RRF",    0x300,	INSN_CLASS_OPWF5 	},
  { "SLEEP",  0x003,	INSN_CLASS_IMPLICIT 	},
  { "SUBWF",  0x080,	INSN_CLASS_OPWF5 	},
  { "SWAPF",  0x380,	INSN_CLASS_OPWF5 	},
  { "TRIS",   0x000,	INSN_CLASS_OPF5 	}, /* not optimal */
  { "XORLW",  0xf00,	INSN_CLASS_LIT8 	},
  { "XORWF",  0x180,	INSN_CLASS_OPWF5 	}
};

/* Scenix SX has a superset of the PIC 12-bit instruction set */
/*
 * It would be nice if there was a more elegant way to do this,
 * either by adding a flags field to struct insn, or by allowing a
 * processor to have more than one associated table.
 */
static struct insn  op_sx[] = {
  { "ADDWF",  0x1c0,	INSN_CLASS_OPWF5 	},
  { "ANDLW",  0xe00,	INSN_CLASS_LIT8 	},
  { "ANDWF",  0x140,	INSN_CLASS_OPWF5 	},
  { "BANK",   0x018,    INSN_CLASS_LIT3_BANK    }, /* SX only */
  { "BCF",    0x400,	INSN_CLASS_B5 		},
  { "BSF",    0x500,	INSN_CLASS_B5 		},
  { "BTFSC",  0x600,	INSN_CLASS_B5 		},
  { "BTFSS",  0x700,	INSN_CLASS_B5		},
  { "CALL",   0x900,	INSN_CLASS_LIT8 	},
  { "CLRF",   0x060,	INSN_CLASS_OPF5 	},
  { "CLRW",   0x040,	INSN_CLASS_IMPLICIT 	},
  { "CLRWDT", 0x004, 	INSN_CLASS_IMPLICIT 	},
  { "COMF",   0x240,	INSN_CLASS_OPWF5 	},
  { "DECF",   0x0c0,	INSN_CLASS_OPWF5 	},
  { "DECFSZ", 0x2c0,	INSN_CLASS_OPWF5 	},
  { "GOTO",   0xa00,	INSN_CLASS_LIT9 	},
  { "INCF",   0x280,	INSN_CLASS_OPWF5 	},
  { "INCFSZ", 0x3c0,	INSN_CLASS_OPWF5 	},
  { "IORLW",  0xd00,	INSN_CLASS_LIT8 	},
  { "IORWF",  0x100,	INSN_CLASS_OPWF5 	},
  { "IREAD",  0x041,    INSN_CLASS_IMPLICIT     }, /* SX only */
  { "MODE",   0x050,    INSN_CLASS_LIT4         }, /* SX only */
  { "MOVF",   0x200,	INSN_CLASS_OPWF5 	},
  { "MOVLW",  0xc00,	INSN_CLASS_LIT8 	},
  { "MOVMW",  0x042,    INSN_CLASS_IMPLICIT     }, /* SX only */
  { "MOVWF",  0x020,	INSN_CLASS_OPF5 	},
  { "MOVWM",  0x043,    INSN_CLASS_IMPLICIT     }, /* SX only */
  { "NOP",    0x000,	INSN_CLASS_IMPLICIT 	},
  { "OPTION", 0x002,	INSN_CLASS_IMPLICIT 	},
  { "PAGE",   0x010,    INSN_CLASS_LIT3_PAGE    }, /* SX only */
  { "RETI",   0x00e,    INSN_CLASS_IMPLICIT     }, /* SX only */
  { "RETIW",  0x00f,    INSN_CLASS_IMPLICIT     }, /* SX only */
  { "RETLW",  0x800,	INSN_CLASS_LIT8 	},
  { "RETP",   0x00d,    INSN_CLASS_IMPLICIT     }, /* SX only */
  { "RETURN", 0x00c,    INSN_CLASS_IMPLICIT     }, /* SX only */
  { "RLF",    0x340,	INSN_CLASS_OPWF5 	},
  { "RRF",    0x300,	INSN_CLASS_OPWF5 	},
  { "SLEEP",  0x003,	INSN_CLASS_IMPLICIT 	},
  { "SUBWF",  0x080,	INSN_CLASS_OPWF5 	},
  { "SWAPF",  0x380,	INSN_CLASS_OPWF5 	},
  { "TRIS",   0x000,	INSN_CLASS_OPF5 	}, /* not optimal */
  { "XORLW",  0xf00,	INSN_CLASS_LIT8 	},
  { "XORWF",  0x180,	INSN_CLASS_OPWF5 	}
};

/* PIC 14-bit instruction set */
static struct insn  op_16cxx[] = {
  { "ADDLW",  0x3e00, 	INSN_CLASS_LIT8 	},
  { "ADDWF",  0x0700,	INSN_CLASS_OPWF7 	},
  { "ANDLW",  0x3900,	INSN_CLASS_LIT8 	},
  { "ANDWF",  0x0500,	INSN_CLASS_OPWF7 	},
  { "BCF",    0x1000,	INSN_CLASS_B7 		},
  { "BSF",    0x1400,	INSN_CLASS_B7 		},
  { "BTFSC",  0x1800,	INSN_CLASS_B7 		},
  { "BTFSS",  0x1c00,	INSN_CLASS_B7		},
  { "CALL",   0x2000,	INSN_CLASS_LIT11 	},
  { "CLRF",   0x0180,	INSN_CLASS_OPF7		},
  { "CLRW",   0x0103,	INSN_CLASS_IMPLICIT 	},
  { "CLRWDT", 0x0064, 	INSN_CLASS_IMPLICIT 	},
  { "COMF",   0x0900,	INSN_CLASS_OPWF7 	},
  { "DECF",   0x0300,	INSN_CLASS_OPWF7 	},
  { "DECFSZ", 0x0b00,	INSN_CLASS_OPWF7 	},
  { "GOTO",   0x2800,	INSN_CLASS_LIT11 	},
  { "INCF",   0x0a00,	INSN_CLASS_OPWF7 	},
  { "INCFSZ", 0x0f00,	INSN_CLASS_OPWF7 	},
  { "IORLW",  0x3800,	INSN_CLASS_LIT8 	},
  { "IORWF",  0x0400,	INSN_CLASS_OPWF7 	},
  { "MOVF",   0x0800,	INSN_CLASS_OPWF7 	},
  { "MOVLW",  0x3000,	INSN_CLASS_LIT8 	},
  { "MOVWF",  0x0080,	INSN_CLASS_OPF7 	},
  { "NOP",    0x0000,	INSN_CLASS_IMPLICIT 	},
  { "OPTION", 0x0062,	INSN_CLASS_IMPLICIT 	},
  { "RETFIE", 0x0009,	INSN_CLASS_IMPLICIT 	},
  { "RETLW",  0x3400,	INSN_CLASS_LIT8 	},
  { "RETURN", 0x0008,	INSN_CLASS_IMPLICIT 	},
  { "RLF",    0x0d00,	INSN_CLASS_OPWF7 	},
  { "RRF",    0x0c00,	INSN_CLASS_OPWF7 	},
  { "SLEEP",  0x0063,	INSN_CLASS_IMPLICIT 	},
  { "SUBLW",  0x3c00,	INSN_CLASS_LIT8 	},
  { "SUBWF",  0x0200,	INSN_CLASS_OPWF7 	},
  { "SWAPF",  0x0e00,	INSN_CLASS_OPWF7 	},
  { "TRIS",   0x0060,	INSN_CLASS_OPF7		},
  { "XORLW",  0x3a00,	INSN_CLASS_LIT8 	},
  { "XORWF",  0x0600,	INSN_CLASS_OPWF7 	}
};

/* PIC 16-bit instruction set */
static struct insn  op_17cxx[] = {
  { "ADDLW",  0xb100,   INSN_CLASS_LIT8         },
  { "ADDWF",  0x0e00,   INSN_CLASS_OPWF8        },
  { "ADDWFC", 0x1000,   INSN_CLASS_OPWF8        },
  { "ANDLW",  0xb500,   INSN_CLASS_LIT8         },
  { "ANDWF",  0x0a00,   INSN_CLASS_OPWF8        },
  { "BCF",    0x8800,   INSN_CLASS_B8           },
  { "BSF",    0x8000,   INSN_CLASS_B8           },
  { "BTFSC",  0x9800,   INSN_CLASS_B8           },
  { "BTFSS",  0x9000,   INSN_CLASS_B8           },
  { "BTG",    0x3800,   INSN_CLASS_B8           },
  { "CALL",   0xe000,   INSN_CLASS_LIT13        },
  { "CLRF",   0x2800,   INSN_CLASS_OPWF8        },
  { "CLRWDT", 0x0004,   INSN_CLASS_IMPLICIT     },
  { "COMF",   0x1200,   INSN_CLASS_OPWF8        },
  { "CPFSEQ", 0x3100,   INSN_CLASS_OPF8         },
  { "CPFSGT", 0x3200,   INSN_CLASS_OPF8         },
  { "CPFSLT", 0x3000,   INSN_CLASS_OPF8         },
  { "DAW",    0x2e00,   INSN_CLASS_OPWF8        },
  { "DECF",   0x0600,   INSN_CLASS_OPWF8        },
  { "DECFSZ", 0x1600,   INSN_CLASS_OPWF8        },
  { "DCFSNZ", 0x2600,   INSN_CLASS_OPWF8        },
  { "GOTO",   0xc000,   INSN_CLASS_LIT13        },
  { "INCF",   0x1400,   INSN_CLASS_OPWF8        },
  { "INCFSZ", 0x1e00,   INSN_CLASS_OPWF8        },
  { "INFSNZ", 0x2400,   INSN_CLASS_OPWF8        },
  { "IORLW",  0xb300,   INSN_CLASS_LIT8         },
  { "IORWF",  0x0800,   INSN_CLASS_OPWF8        },
  { "LCALL",  0xb700,   INSN_CLASS_LIT8         },
  { "MOVFP",  0x6000,   INSN_CLASS_FP           },
  { "MOVPF",  0x4000,   INSN_CLASS_PF           },
  { "MOVLB",  0xb800,   INSN_CLASS_LIT8         },
  { "MOVLR",  0xba00,   INSN_CLASS_LIT4S        },
  { "MOVLW",  0xb000,   INSN_CLASS_LIT8         },
  { "MOVWF",  0x0100,   INSN_CLASS_OPF8         },
  { "MULLW",  0xbc00,   INSN_CLASS_LIT8         },
  { "MULWF",  0x3400,   INSN_CLASS_OPF8         },
  { "NEGW",   0x2c00,   INSN_CLASS_OPWF8        },
  { "NOP",    0x0000,   INSN_CLASS_IMPLICIT     },
  { "RETFIE", 0x0005,   INSN_CLASS_IMPLICIT     },
  { "RETLW",  0xb600,   INSN_CLASS_LIT8         },
  { "RETURN", 0x0002,   INSN_CLASS_IMPLICIT     },
  { "RLCF",   0x1a00,   INSN_CLASS_OPWF8        },
  { "RLNCF",  0x2200,   INSN_CLASS_OPWF8        },
  { "RRCF",   0x1800,   INSN_CLASS_OPWF8        },
  { "RRNCF",  0x2000,   INSN_CLASS_OPWF8        },
  { "SETF",   0x2a00,   INSN_CLASS_OPWF8        },
  { "SLEEP",  0x0003,   INSN_CLASS_IMPLICIT     },
  { "SUBLW",  0xb200,   INSN_CLASS_LIT8         },
  { "SUBWF",  0x0400,   INSN_CLASS_OPWF8        },
  { "SUBWFB", 0x0200,   INSN_CLASS_OPWF8        },
  { "SWAPF",  0x1c00,   INSN_CLASS_OPWF8        },
  { "TABLRD", 0xa800,   INSN_CLASS_TBL3         },
  { "TABLWT", 0xac00,   INSN_CLASS_TBL3         },
  { "TLRD",   0xa000,   INSN_CLASS_TBL2         },
  { "TLWT",   0xa400,   INSN_CLASS_TBL2         },
  { "TSTFSZ", 0x3300,   INSN_CLASS_OPF8         },
  { "XORLW",  0xb400,   INSN_CLASS_LIT8         },
  { "XORWF",  0x0c00,   INSN_CLASS_OPWF8        }
};

static struct insn  op_18cxx[] = {
  { "ADDLW",  0x0f00, 	INSN_CLASS_LIT8 	},
  { "ADDWF",  0x2400,	INSN_CLASS_OPWFA8 	},
  { "ADDWFC", 0x2000,	INSN_CLASS_OPWFA8 	},
  { "ANDLW",  0x0b00,	INSN_CLASS_LIT8 	},
  { "ANDWF",  0x1400,	INSN_CLASS_OPWFA8 	},
  { "BC",     0xe200,	INSN_CLASS_RBRA8       	},
  { "BCF",    0x9000,	INSN_CLASS_BA8 		},
  { "BN",     0xe600,	INSN_CLASS_RBRA8       	},
  { "BNC",    0xe300,	INSN_CLASS_RBRA8       	},
  { "BNN",    0xe700,	INSN_CLASS_RBRA8       	},
  { "BNOV",   0xe500,	INSN_CLASS_RBRA8       	},
  { "BNZ",    0xe100,	INSN_CLASS_RBRA8       	},
  { "BRA",    0xd000,	INSN_CLASS_RBRA11	},
  { "BSF",    0x8000,	INSN_CLASS_BA8 		},
  { "BTFSC",  0xb000,	INSN_CLASS_BA8 		},
  { "BTFSS",  0xa000,	INSN_CLASS_BA8		},
  { "BTG",    0x7000,	INSN_CLASS_BA8		},
  { "BOV",    0xe400,	INSN_CLASS_RBRA8       	},
  { "BZ",     0xe000,	INSN_CLASS_RBRA8       	},
  { "CALL",   0xec00,	INSN_CLASS_CALL20 	},
  { "CLRF",   0x6a00,	INSN_CLASS_OPFA8	},
  { "CLRWDT", 0x0004, 	INSN_CLASS_IMPLICIT 	},
  { "COMF",   0x1c00,	INSN_CLASS_OPWFA8 	},
  { "CPFSEQ", 0x6200,	INSN_CLASS_OPFA8 	},
  { "CPFSGT", 0x6400,	INSN_CLASS_OPFA8 	},
  { "CPFSLT", 0x6000,	INSN_CLASS_OPFA8 	},
  { "DAW",    0x0007,	INSN_CLASS_IMPLICIT 	},
  { "DECF",   0x0400,	INSN_CLASS_OPWFA8 	},
  { "DECFSZ", 0x2c00,	INSN_CLASS_OPWFA8 	},
  { "DCFSNZ", 0x4c00,	INSN_CLASS_OPWFA8 	},
  { "GOTO",   0xef00,	INSN_CLASS_LIT20 	},
  { "INCF",   0x2800,	INSN_CLASS_OPWFA8 	},
  { "INCFSZ", 0x3c00,	INSN_CLASS_OPWFA8 	},
  { "INFSNZ", 0x4800,	INSN_CLASS_OPWFA8 	},
  { "IORLW",  0x0900,	INSN_CLASS_LIT8 	},
  { "IORWF",  0x1000,	INSN_CLASS_OPWFA8 	},
  { "LFSR",   0xee00,	INSN_CLASS_FLIT12 	},
  { "MOVF",   0x5000,	INSN_CLASS_OPWFA8 	},
  { "MOVFF",  0xc000,	INSN_CLASS_FF 		},
  { "MOVLB",  0x0100,	INSN_CLASS_LIT8 	},
  { "MOVLW",  0x0e00,	INSN_CLASS_LIT8 	},
  { "MOVWF",  0x6e00,	INSN_CLASS_OPFA8 	},
  { "MULLW",  0x0d00,	INSN_CLASS_LIT8 	},
  { "MULWF",  0x0200,	INSN_CLASS_OPFA8 	},
  { "NEGF",   0x6c00,	INSN_CLASS_OPFA8 	},
  { "NOP",    0x0000,	INSN_CLASS_IMPLICIT 	},
  { "POP",    0x0006,	INSN_CLASS_IMPLICIT 	},
  { "PUSH",   0x0005,	INSN_CLASS_IMPLICIT 	},
  { "RCALL",  0xd800,	INSN_CLASS_RBRA11 	},
  { "RESET",  0x00ff,	INSN_CLASS_IMPLICIT 	}, 
  { "RETFIE", 0x0010,	INSN_CLASS_LIT1 	},
  { "RETLW",  0x0c00,	INSN_CLASS_LIT8 	},
  { "RETURN", 0x0012,	INSN_CLASS_LIT1 	},
  { "RLCF",   0x3400,	INSN_CLASS_OPWFA8 	},
  { "RLNCF",  0x4400,	INSN_CLASS_OPWFA8 	},
  { "RRCF",   0x3000,	INSN_CLASS_OPWFA8 	},
  { "RRNCF",  0x4000,	INSN_CLASS_OPWFA8 	},
  { "SETF",   0x6800,	INSN_CLASS_OPFA8 	},
  { "SLEEP",  0x0003,	INSN_CLASS_IMPLICIT 	},
  { "SUBFWB", 0x5400,	INSN_CLASS_OPWFA8 	},
  { "SUBLW",  0x0800,	INSN_CLASS_LIT8 	},
  { "SUBWF",  0x5c00,	INSN_CLASS_OPWFA8 	},
  { "SUBWFB", 0x5800,	INSN_CLASS_OPWFA8 	},
  { "SWAPF",  0x3800,	INSN_CLASS_OPWFA8 	},
  { "TBLRD",  0x0008,	INSN_CLASS_TBL 		},
  { "TBLWT",  0x000c,	INSN_CLASS_TBL 		},
  { "TSTFSZ", 0x6600,	INSN_CLASS_OPFA8	},
  { "XORLW",  0x0a00,	INSN_CLASS_LIT8 	},
  { "XORWF",  0x1800,	INSN_CLASS_OPWFA8	}
};

/* PIC 16-bit "Special" instruction set */
static struct insn  op_18cxx_sp[] = {
  { "CLRC",  0x90d8,	INSN_CLASS_IMPLICIT 	},
  { "CLRDC", 0x92d8,	INSN_CLASS_IMPLICIT 	},
  { "CLRN",  0x98d8,	INSN_CLASS_IMPLICIT 	},
  { "CLROV", 0x96d8,	INSN_CLASS_IMPLICIT 	},
  { "CLRW",  0x6ae8,	INSN_CLASS_IMPLICIT 	},
  { "CLRZ",  0x94d8,	INSN_CLASS_IMPLICIT 	},

  { "SETC",  0x80d8,	INSN_CLASS_IMPLICIT 	},
  { "SETDC", 0x82d8,	INSN_CLASS_IMPLICIT 	},
  { "SETN",  0x88d8,	INSN_CLASS_IMPLICIT 	},
  { "SETOV", 0x86d8,	INSN_CLASS_IMPLICIT 	},
  { "SETZ",  0x84d8,	INSN_CLASS_IMPLICIT 	},

  { "SKPC",  0xa0d8,	INSN_CLASS_IMPLICIT 	},
  { "SKPDC", 0xa2d8,	INSN_CLASS_IMPLICIT 	},
  { "SKPN",  0xa8d8,	INSN_CLASS_IMPLICIT 	},
  { "SKPOV", 0xa6d8,	INSN_CLASS_IMPLICIT 	},
  { "SKPZ",  0xa4d8,	INSN_CLASS_IMPLICIT 	},

  { "SKPNC", 0xb0d8,	INSN_CLASS_IMPLICIT 	},
  { "SKPNDC",0xb2d8,	INSN_CLASS_IMPLICIT 	},
  { "SKPNN", 0xb8d8,	INSN_CLASS_IMPLICIT 	},
  { "SKPNOV",0xb6d8,	INSN_CLASS_IMPLICIT 	},
  { "SKPNZ", 0xb4d8,	INSN_CLASS_IMPLICIT 	},

  { "TGC",   0x70d8,	INSN_CLASS_IMPLICIT 	},
  { "TGDC",  0x72d8,	INSN_CLASS_IMPLICIT 	},
  { "TGN",   0x78d8,	INSN_CLASS_IMPLICIT 	},
  { "TGOV",  0x76d8,	INSN_CLASS_IMPLICIT 	},
  { "TGZ",   0x74d8,	INSN_CLASS_IMPLICIT 	}

};

/* PIC 12-bit and 14-bit "Special" instruction set */
static struct insn special[] = {
  { "addcf",    (long int)do_addcf,  INSN_CLASS_FUNC,        0 },
  { "adddcf",   (long int)do_adddcf, INSN_CLASS_FUNC,        0 },
  { "b",        (long int)do_b,      INSN_CLASS_FUNC,        0 },
  { "bc",       (long int)do_bc,     INSN_CLASS_FUNC,        0 },
  { "bdc",      (long int)do_bdc,    INSN_CLASS_FUNC,        0 },
  { "bz",       (long int)do_bz,     INSN_CLASS_FUNC,        0 },
  { "bnc",      (long int)do_bnc,    INSN_CLASS_FUNC,        0 },
  { "bndc",     (long int)do_bndc,   INSN_CLASS_FUNC,        0 },
  { "bnz",      (long int)do_bnz,    INSN_CLASS_FUNC,        0 },
  { "clrc",     (long int)do_clrc,   INSN_CLASS_FUNC,        0 },
  { "clrdc",    (long int)do_clrdc,  INSN_CLASS_FUNC,        0 },
  { "clrz",     (long int)do_clrz,   INSN_CLASS_FUNC,        0 },
  { "lcall",    (long int)do_lcall,  INSN_CLASS_FUNC,        0 },
  { "lgoto",    (long int)do_lgoto,  INSN_CLASS_FUNC,        0 },
  { "movfw",    (long int)do_movfw,  INSN_CLASS_FUNC,        0 },
  { "negf",     (long int)do_negf,   INSN_CLASS_FUNC,        0 },
  { "setc",     (long int)do_setc,   INSN_CLASS_FUNC,        0 },
  { "setdc",    (long int)do_setdc,  INSN_CLASS_FUNC,        0 },
  { "setz",     (long int)do_setz,   INSN_CLASS_FUNC,        0 }, 
  { "skpc",     (long int)do_skpc,   INSN_CLASS_FUNC,        0 },
  { "skpdc",    (long int)do_skpdc,  INSN_CLASS_FUNC,        0 },
  { "skpz",     (long int)do_skpz,   INSN_CLASS_FUNC,        0 },
  { "skpnc",    (long int)do_skpnc,  INSN_CLASS_FUNC,        0 },
  { "skpndc",   (long int)do_skpndc, INSN_CLASS_FUNC,        0 },
  { "skpnz",    (long int)do_skpnz,  INSN_CLASS_FUNC,        0 },
  { "subcf",    (long int)do_subcf,  INSN_CLASS_FUNC,        0 },
  { "subdcf",   (long int)do_subdcf, INSN_CLASS_FUNC,        0 },
  { "tstf",     (long int)do_tstf,   INSN_CLASS_FUNC,        0 }

};

/* XXXPRO: Need to add a line here for any extra processors. */

#define NUM_OP_0	(sizeof(op_0) / sizeof(op_0[0]))
#define NUM_OP_1	(sizeof(op_1) / sizeof(op_1[0]))
#define NUM_OP_12C5XX	(sizeof(op_12c5xx) / sizeof(op_12c5xx[0]))
#define NUM_OP_16CXX	(sizeof(op_16cxx) / sizeof(op_16cxx[0]))
#define NUM_OP_SX	(sizeof(op_sx) / sizeof(op_sx[0]))
#define NUM_OP_17CXX    (sizeof(op_17cxx) / sizeof(op_17cxx[0]))
#define NUM_OP_18CXX	(sizeof(op_18cxx) / sizeof(op_18cxx[0]))
#define NUM_OP_18CXX_SP	(sizeof(op_18cxx_sp) / sizeof(op_18cxx_sp[0]))
#define NUM_OP_SPECIAL	(sizeof(special) / sizeof(special[0]))

void opcode_init(int stage)
{
  int i;
  int count = 0;
  struct insn *base = NULL;

  switch (stage) {
  case 0:
    base = op_0;
    count = NUM_OP_0;
    break;
  case 1:
    base = op_1;
    count = NUM_OP_1;
    break;
  case 2:

    /* XXXPRO: Need to add a line here for any extra processors. */

    switch (state.processor) {
    case eeprom8:
      base = 0;
      count = 0;
      state.device.core_size = CORE_8BIT_MASK;
      state.device.config_address = 0;
      state.device.id_location = 0;
      break;
    case generic:
      base = 0;
      count = 0;
      state.device.core_size = CORE_12BIT_MASK;
      state.device.config_address = CONFIG_ADDRESS_12;
      state.device.id_location = IDLOC_ADDRESS_12;
      break;
    case pic12c508:
    case pic12c508a:
    case pic12c509:
    case pic12c509a:
    case pic12ce518:
    case pic12ce519:
    case pic12cr509a:
    case pic16c5x:
    case pic16c505:
    case pic16c52:
    case pic16c54:
    case pic16c54a:
    case pic16c54b:
    case pic16c54c:
    case pic16c55:
    case pic16c55a:
    case pic16c56:
    case pic16c56a:
    case pic16c57:
    case pic16c57c:
    case pic16c58:
    case pic16c58a:
    case pic16c58b:   
    case pic16cr54:
    case pic16cr54a:
    case pic16cr54b:
    case pic16cr54c:
    case pic16cr56:
    case pic16cr56a:
    case pic16cr57a:
    case pic16cr57b:
    case pic16cr57c:
    case pic16cr58a:
    case pic16cr58b:
    case pic16hv540:
      base = op_12c5xx;
      count = NUM_OP_12C5XX;
      state.device.core_size = CORE_12BIT_MASK;
      state.device.config_address = CONFIG_ADDRESS_12;
      state.device.id_location = IDLOC_ADDRESS_12;
      break;
    case sx18:
    case sx20:
    case sx28:
      base = op_sx;
      count = NUM_OP_SX;
      state.device.core_size = CORE_12BIT_MASK;
      state.device.config_address = CONFIG_ADDRESS_12;
      state.device.id_location = IDLOC_ADDRESS_12;
      /* page instruction conflicts with the page directive */
      remove_symbol(state.stBuiltin, "page");    
      break;
    case pic12c671:
    case pic12c672:
    case pic12ce673:
    case pic12ce674:
    case pic14000:
    case pic16cxx:
    case pic16c432:
    case pic16c433:
    case pic16c461:
    case pic16c554:
    case pic16c558:
    case pic16c61:
    case pic16c62:
    case pic16c62a:
    case pic16c62b:
    case pic16c620:
    case pic16c620a:
    case pic16c621:
    case pic16c621a:
    case pic16c622:
    case pic16c622a:
    case pic16c63:
    case pic16c63a:
    case pic16c64:
    case pic16c64a:
    case pic16c641:
    case pic16c642:
    case pic16c65:
    case pic16c65a:   
    case pic16c65b:
    case pic16c66:
    case pic16c661:
    case pic16c662:
    case pic16c67:
    case pic16c70:
    case pic16c71:
    case pic16c71a:
    case pic16c710:
    case pic16c711:
    case pic16c712:
    case pic16c715:
    case pic16c716:
    case pic16c717:
    case pic16c72:
    case pic16c72a:
    case pic16c73:
    case pic16c73a:
    case pic16c73b:
    case pic16c74:
    case pic16c745:
    case pic16c74a:
    case pic16c74b:
    case pic16c76:
    case pic16c765:
    case pic16c77:
    case pic16c770:
    case pic16c771:
    case pic16c773:
    case pic16c774:
    case pic16c781:
    case pic16c782:
    case pic16c83:
    case pic16c84:
    case pic16c85:   
    case pic16c86:   
    case pic16c923:
    case pic16c924:
    case pic16c925:
    case pic16c926:
    case pic16ce623:
    case pic16ce624:
    case pic16ce625:
    case pic16cr62:
    case pic16cr620a:
    case pic16cr63:
    case pic16cr64:  
    case pic16cr65:
    case pic16cr72:
    case pic16cr83:
    case pic16cr84:
    case pic16f627:
    case pic16f628:
    case pic16f73:
    case pic16f74:
    case pic16f83:
    case pic16f84:
    case pic16f85:
    case pic16f86:
    case pic16f84a:
    case pic16f870:
    case pic16f871:
    case pic16f872:
    case pic16f873:
    case pic16f874:
    case pic16f876:
    case pic16f877:
    case pic16f877a:
    case pic16lc74b:
      base = op_16cxx;
      count = NUM_OP_16CXX;
      state.device.core_size = CORE_14BIT_MASK;
      state.device.config_address = CONFIG_ADDRESS_14;
      state.device.id_location = IDLOC_ADDRESS_14;
      break;
    case pic17cxx:
    case pic17c42:
    case pic17c42a:
    case pic17c43:
    case pic17c44:
    case pic17c752:
    case pic17c756:
    case pic17c756a:
    case pic17c762:
    case pic17c766:
    case pic17cr42:
    case pic17cr43:
      base = op_17cxx;
      count = NUM_OP_17CXX;
      state.device.core_size = CORE_16BIT_MASK;
      _17cxx_core = 1;
      state.device.config_address = CONFIG_17CXX;
      break;
    case pic18cxx2:
    case pic18c242:
    case pic18c252:
    case pic18c442:
    case pic18c452:
    case pic18c601:
    case pic18c658:
    case pic18c801:
    case pic18c858:
    case pic18f010:
    case pic18f012:
    case pic18f020:
    case pic18f022:
    case pic18f242:
    case pic18f248:
    case pic18f252:
    case pic18f258:
    case pic18f442:
    case pic18f448:
    case pic18f452:
    case pic18f458:
      base = op_18cxx;
      count = NUM_OP_18CXX;
      state.device.core_size = CORE_16BIT_MASK;
      _16bit_core = 1;
      state.c_memory_base = CONFIG1L;
      state.device.config_address = CONFIG1L;

      /* The 16_bit core special macros are encoded directly into the
       * symbol table like regular instructions. */
      for (i = 0; i < NUM_OP_18CXX_SP; i++)
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
    count = NUM_OP_SPECIAL;
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
