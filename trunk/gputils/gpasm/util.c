/* Some helpful utility functions for gpasm
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

#include "libgputils.h"
#include "gpasm.h"
#include "gperror.h"
#include "directive.h"
#include "coff.h"

static struct file_context *last = NULL;

int gpasm_magic(char *c)
{
  if (c[0] == '\\') {
    switch (c[1]) {
    case 'b':
      return '\b';
    case 'n':
      return '\n';
    case 'r':
      return '\r';
    default:
      return c[1];
    }
  }

  return c[0];
}

void set_global(char *name,
		gpasmVal value,
		enum globalLife lifetime,
		enum gpasmValTypes type)
{
  struct symbol *sym;
  struct variable *var;

  /* Search the entire stack (i.e. include macro's local symbol
     tables) for the symbol.  If not found, then add it to the global
     symbol table.  */
  sym = get_symbol(state.stTop, name);
  if (sym == NULL)
    sym = add_symbol(state.stGlobal, name);
  var = get_symbol_annotation(sym);
  if (var == NULL) {
    /* new symbol */
    var = malloc(sizeof(*var));
    annotate_symbol(sym, var);
    var->value = value;
    var->coff_num = state.obj.symbol_num;
    var->type = type;
    var->previous_type = type;  /* coff symbols can be changed to global */
  } else if (lifetime == TEMPORARY) {
    /*
     * TSD - the following embarrassing piece of code is a hack
     *       to fix a problem when global variables are changed
     *       during the expansion of a macro. Macros are expanded
     *       by running through them twice. if you have a stetement
     *       like:
     *       some_var set some_var + 1
     *       then this is incremented twice! So the if statement
     *       makes sure that the value is assigned on the second
     *       pass only in the macro. Jeez this really sucks....
     */
     var->value = value;

  } else if (state.pass == 2) {
    if (var->value != value) {
      char message[BUFSIZ];

      sprintf(message,
              "Value of symbol \"%s\" differs on second pass\n pass 1=%d,  pass 2=%d",
              name,var->value,value);
      gperror(GPE_DIFFLAB, message);      
    }
    /* write coff symbol */ 
    if (var->type == gvt_extern) {
      coff_add_sym(name, value, 0, T_NULL, C_EXT);
    } else if (var->type == gvt_global) {
      coff_add_sym(name, value, state.obj.section_num, T_NULL, C_EXT);
    } else if (var->type == gvt_static) {
      coff_add_sym(name, value, state.obj.section_num, T_NULL, C_STAT);
    } else if (var->type == gvt_address) {
      coff_add_sym(name, value, state.obj.section_num, T_NULL, C_LABEL);
    }
  }

  /* increment the index into the coff symbol table for the relocations */
  switch(type) {
  case gvt_extern:
  case gvt_global:
  case gvt_static:
  case gvt_address:
    state.obj.symbol_num++;
    break;
  default:
    break;
  }

}

void select_errorlevel(int level)
{
  if (state.cmd_line.error_level == 1) {
    gpmessage(GPM_SUPVAL, NULL);
  } else {
    if (level == 0) {
      state.error_level = 0;
    } else if (level == 1) {
      state.error_level = 1;
    } else if (level == 2) {
      state.error_level = 2;
    } else {
      if (state.pass == 0) {
	fprintf(stderr,
		"Error: invalid warning level \"%i\"\n",
		level);
      } else {
        gperror(GPE_ILLEGAL_ARGU, "Expected w= 0, 1, 2");
      }
    }
  }
}

void select_expand(char *expand)
{
  if (state.cmd_line.macro_expand == 1) {
    gpmessage(GPM_SUPLIN, NULL);
  } else {
    if (strcasecmp(expand, "ON") == 0) {
      state.lst.expand = 1;
    } else if (strcasecmp(expand, "OFF") == 0) {
      state.lst.expand = 0;
    } else {
      state.lst.expand = 1;
      if (state.pass == 0) {
	fprintf(stderr,
		"Error: invalid option \"%s\"\n",
		expand);
      } else {
        gpwarning(GPE_ILLEGAL_ARGU, "Expected ON or OFF");
      }
    }
  }
}

void select_hexformat(char *format_name)
{
  if (state.cmd_line.hex_format == 1) {
    gpwarning(GPW_CMDLINE_HEXFMT, NULL);
  } else {
    if (strcasecmp(format_name, "inhx8m") == 0) {
      state.hex_format = inhx8m;
    } else if (strcasecmp(format_name, "inhx8s") == 0) {
      state.hex_format = inhx8s;
    } else if (strcasecmp(format_name, "inhx16") == 0) {
      state.hex_format = inhx16;
    } else if (strcasecmp(format_name, "inhx32") == 0) {
      state.hex_format = inhx32;
    } else {
      state.hex_format = inhx8m;
      if (state.pass == 0) {
	fprintf(stderr,
		"Error: invalid format \"%s\"\n",
		format_name);
      } else {
        gperror(GPE_ILLEGAL_ARGU, "Expected inhx8m, inhx8s, inhx16, or inhx32");
      }
    }
  }
}

void select_radix(char *radix_name)
{
  if (state.cmd_line.radix == 1) {
    gpwarning(GPW_CMDLINE_RADIX, NULL);
  } else {
    if (strcasecmp(radix_name, "hex") == 0) {
      state.radix = 16;
    } else if (strcasecmp(radix_name, "dec") == 0) {
      state.radix = 10;
    } else if (strcasecmp(radix_name, "decimal") == 0) {
      state.radix = 10;
    } else if (strcasecmp(radix_name, "oct") == 0) {
      state.radix = 8;
    } else if (strcasecmp(radix_name, "octal") == 0) {
      state.radix = 8;
    } else {
      state.radix = 16;
      if (state.pass == 0) {
	fprintf(stderr,
		"invalid radix \"%s\", will use hex.\n",
		radix_name);
      } else {
        gpwarning(GPW_RADIX, NULL);
      }
    }
  }
}

/************************************************************************/

/* Function to append a line to an ongoing macro definition */
void macro_append(void)
{
  struct macro_body *body = malloc(sizeof(*body));

  body->label = NULL;
  body->op = NULL;
  body->parms = NULL;
  body->src_line = NULL;

  *state.mac_prev = body;	/* append this to the chain */
  state.mac_prev = &body->next;	/* this is the new end of the chain */
  state.mac_body = body;
  body->next = NULL;		/* make sure it's terminated */
}

gpasmVal do_or_append_insn(char *op, struct pnode *parms)
{
  gpasmVal r;

  if (!state.mac_prev ||
      (strcasecmp(op, "endm") == 0) ||
      (state.while_head && (strcasecmp(op, "endw") == 0))) {
    r = do_insn(op, parms);
  } else {
    macro_append();
    state.mac_body->op = op;
    state.mac_body->parms = parms;
    r = 0;
  }

  return r;
}

void print_pnode(struct pnode *p)
{
  if(!p) {
    printf("Null\n");
    return;
  }

  switch(p->tag) {
  case constant:
    printf("  constant: %d\n",p->value.constant);
    break;
  case symbol:
    printf("  symbol: %s\n",p->value.symbol);
    break;
  case unop:
    printf("  unop: %d\n",p->value.unop.op);
    break;

  case binop:
    printf("  binop: %d\n",p->value.binop.op);
    break;
  case string:
    printf("  string: %s\n",p->value.string);
    break;
  case list:
    printf("  list:\n");
    break;
   
  default:
    printf("unknown type\n");

  }
}

void print_macro_node(struct macro_body *mac)
{
  if(mac->label)
    printf(" label = %s\n",mac->label);
  if(mac->op)
    printf(" op = %s\n",mac->op);
  if(mac->src_line)
    printf(" src_line = %s\n",mac->src_line);
  if(mac->parms)
    print_pnode(mac->parms);
}

void print_macro_body(struct macro_body *mac)
{
  struct macro_body *mb = mac;

  printf("{\n");
  while(mb) {
    print_macro_node(mb);
    mb = mb->next;
  }
  printf("}\n");
}


/************************************************************************/

/* add_file: add a file of type 'type' to the file_context stack.
 */

struct file_context * add_file(unsigned int type, char *name)
{
  static unsigned int file_id = 0;
  struct file_context *new;

  /* First check to make sure this file is not already in the list */

  if(last) {
    new = last;
    do {
      if(strcmp(new->name, name) == 0)
	return(new);
      new = new->prev;
    } while(new != NULL);
  }

  new = malloc(sizeof(*new));

  new->name = strdup(name);
  new->ft = type;
  new->prev = last;
  new->id = file_id++;
  new->next = NULL;
  if(last)
    last->next = new;

  last = new;
  state.files = new;
  return(new);
}

/* free_files: free memory allocated to the file_context stack
 */

void free_files(void)
{
  struct file_context *old;

  while(last != NULL) {
    old = last;
    last = old->prev;
    free(old->name);
    free(old);
  } 

}

void hex_init(void)
{
  int num_errors;

  /* FIXME: Must delete hex file when suppressed.  Find a better way. */
  if (state.codfile == suppress)
    num_errors = 1;
  else
    num_errors = state.num.errors;  

  if (check_writehex(state.i_memory, state.hex_format)) {
    gperror(GPE_IHEX,NULL); 
  } else {
    int byte_words;
  
    if (state.device.core_size > 0xff) {
      byte_words = 0;
    } else {
      byte_words = 1;
      if (state.hex_format != inhx8m) {
        gpwarning(GPW_UNKNOWN,"Must use inhx8m format for EEPROM8");
        state.hex_format = inhx8m;
      }
    }
  
    if (writehex(state.basefilename, state.i_memory, 
                 state.hex_format, num_errors,
                 byte_words, state.dos_newlines)) {
      gperror(GPE_UNKNOWN,"Error generating hex file");
    }
  }
  
  return;
}
