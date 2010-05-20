/* Some helpful utility functions for gpasm
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
#include "gperror.h"
#include "directive.h"
#include "coff.h"

static struct file_context *last = NULL;

int
stringtolong(const char *string, int radix)
{
  char *endptr;
  int value;
  
  value = strtoul(string, &endptr, radix);                           
  if ((endptr == NULL) || (*endptr != '\0')) {
    char complaint[80];

    snprintf(complaint, sizeof(complaint),
             isprint(*endptr) ?
             "Illegal character '%c' in numeric constant " :
             "Illegal character %#x in numeric constant" ,
             *endptr);
    gperror(GPE_UNKNOWN, complaint);
  }

  return value;
}

int gpasm_magic(const char *c)
{
  if (c[0] == '\\') {
    switch (c[1]) {
    case 'a':
      return '\a';
    case 'b':
      return '\b';
    case 'f':
      return '\f';
    case 'n':
      return '\n';
    case 'r':
      return '\r';
    case 't':
      return '\t';
    case 'v':
      return '\v';
    default:
      return c[1];
    }
  }

  return c[0];
}

/*
  convert_escaped_char(char *src,char c)
  Input: pointer to a string
  Output returns the input string with escaped char converted to a regular char

  For example if escaped character is a double quote then:

  This is a escaped quote: \"

  is translated to:

  This is a escaped quote: "

*/
char *
convert_escaped_char(char *str, char c)
{
  char *src  = str;
  char *dest = str;

  if (!str)
    return str;

  while (*src) {
    if (*src =='\\' && src[1] == c)
      src++;
    *dest++ = *src++;
  }
  *dest=0;

  return str;
}

/* Determine the value of the escape char pointed to by ps.  Return a pointer
to the next character. */ 

const char *
convert_escape_chars(const char *ps, int *value)
{
  int count;
  
  if (*ps != '\\') {
    *value = *ps++;
  } else {
    /* escape char, convert its value and write to the new string */    
    switch (ps[1]) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
      /* octal number */
      count = 0;
      *value = 0;
      ps++;

      while (count < 3) {
        if (*ps < '0' || *ps > '7')
          break;
        *value = (*value << 3) + *ps - '0';
        ps++;
        count++;
      }        
      break;
    case 'x':
      /* hex number */
      if ((ps[2] == '\0') || (ps[3] == '\0')) {
        gperror(GPE_UNKNOWN, "missing hex value in \\x escape character");
        *value = 0;
        /* return a NULL character */
        ps += 2;
	if (*ps)
	  ++ps;
      } else {
        char buffer[3];

        buffer[0] = ps[2];
        buffer[1] = ps[3];
        buffer[2] = 0;
        *value = stringtolong(buffer, 16);
        ps += 4;
      }
      break;
    default:
      if (ps[1] == '\0') {
        gperror(GPE_UNKNOWN, "missing value in \\ escape character");
        *value = 0;
        /* return a NULL character */
        ps++;
      } else {
        *value = gpasm_magic(ps);
        ps += 2;
      }
    }
  }
  
  return ps;

}

/* In some contexts, such as in the operand to a literal instruction, a
 * single-character string literal in an expression can be coerced to a
 * character literal. coerce_str1 converts a string-type pnode to a
 * constant-type pnode in-place. */
void coerce_str1(struct pnode *exp)
{
  if ((exp != NULL) && (exp->tag == string)) {
    int value;
    const char *pc = convert_escape_chars(exp->value.string, &value);
    if (*pc == '\0') {
      /* castable string, make the conversion */
      exp->tag = constant;
      exp->value.constant = value;
    }
  }
}

void set_global(const char *name,
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
    var->coff_section_num = state.obj.section_num;
    var->type = type;
    var->previous_type = type;  /* coff symbols can be changed to global */
    var->lifetime = lifetime;

    /* increment the index into the coff symbol table for the relocations */
    switch(type) {
    case gvt_extern:
    case gvt_global:
    case gvt_static:
    case gvt_address:
    case gvt_debug:
    case gvt_absolute:
      state.obj.symbol_num++;
      break;
    default:
      break;
    }

  } else if (lifetime == TEMPORARY) {
    /*
     * TSD - the following embarrassing piece of code is a hack
     *       to fix a problem when global variables are changed
     *       during the expansion of a macro. Macros are expanded
     *       by running through them twice. if you have a statement
     *       like:
     *       some_var set some_var + 1
     *       then this is incremented twice! So the if statement
     *       makes sure that the value is assigned on the second
     *       pass only in the macro. Jeez this really sucks....
     */
     var->value = value;

  } else if (state.pass == 2) {
    char *coff_name;

    if (var->value != value) {
      char message[BUFSIZ];

      snprintf(message, sizeof(message),
               "Value of symbol \"%s\" differs on second pass\n pass 1=%d,  pass 2=%d",
               name,var->value,value);
      gperror(GPE_DIFFLAB, message);      
    }

    coff_name = coff_local_name(name);
    coff_add_sym(coff_name, value, var->type);

    if (coff_name != NULL)
      free(coff_name);
  }

}

void purge_temp_symbols(struct symbol_table *table) {
  int i;
  if (table != NULL) {
    for (i = 0; i < HASH_SIZE; ++i) {
      struct symbol *cur_symbol;
      struct symbol *last_symbol = NULL;
      cur_symbol = table->hash_table[i];
      while (cur_symbol != NULL) {
        if (cur_symbol != NULL) {
          struct variable *var = (struct variable *)get_symbol_annotation(cur_symbol);
          if (var != NULL) {
            if (var->lifetime == TEMPORARY) {
              struct symbol *next_symbol = cur_symbol->next;
              free(cur_symbol);
              table->count--;
              if (last_symbol == NULL)
                table->hash_table[i] = next_symbol;
              else
                last_symbol->next = next_symbol;
              cur_symbol = next_symbol;
              continue;
            }
          }
        }
        last_symbol = cur_symbol;
        cur_symbol = cur_symbol->next;
      }
    }

    purge_temp_symbols(table->prev);
  }
}

void select_errorlevel(int level)
{
  if (state.cmd_line.error_level) {
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

void select_expand(const char *expand)
{
  if (state.cmd_line.macro_expand) {
    gpmessage(GPM_SUPLIN, NULL);
  } else {
    if (strcasecmp(expand, "ON") == 0) {
      state.lst.expand = true;
    } else if (strcasecmp(expand, "OFF") == 0) {
      state.lst.expand = false;
    } else {
      state.lst.expand = true;
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

void select_hexformat(const char *format_name)
{
  if (state.cmd_line.hex_format) {
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

void select_radix(const char *radix_name)
{
  if (state.cmd_line.radix) {
    gpwarning(GPW_CMDLINE_RADIX, NULL);
  } else {
    if (strcasecmp(radix_name, "h") == 0 ||
	strcasecmp(radix_name, "hex") == 0 ||
	strcasecmp(radix_name, "hexadecimal") == 0) {
      state.radix = 16;
    } else if (strcasecmp(radix_name, "d") == 0 ||
	       strcasecmp(radix_name, "dec") == 0 ||
	       strcasecmp(radix_name, "decimal") == 0) {
      state.radix = 10;
    } else if (strcasecmp(radix_name, "o") == 0 ||
	       strcasecmp(radix_name, "oct") == 0 ||
	       strcasecmp(radix_name, "octal") == 0) {
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
  if(mac->src_line)
    printf(" src_line = %s\n",mac->src_line);
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

struct file_context * add_file(unsigned int type, const char *name)
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

  if (state.hexfile == suppress) {
    /* Must delete hex file when suppressed. */
    writehex(state.basefilename, 
             state.i_memory, 
             state.hex_format, 
             1,
             state.dos_newlines,
	     1);
    return;
  }

  if (check_writehex(state.i_memory, state.hex_format)) {
    gperror(GPE_IHEX,NULL); 
    writehex(state.basefilename, state.i_memory,
	     state.hex_format, 1,
	     state.dos_newlines, 1);
  } else if (state.device.class != NULL) {
    if (writehex(state.basefilename, state.i_memory, 
                 state.hex_format, state.num.errors,
                 state.dos_newlines,
		 state.device.class->core_size)) {
      gperror(GPE_UNKNOWN,"Error generating hex file");
    }
  } else {
    /* Won't have anything to write, just remove any old files */
    writehex(state.basefilename, state.i_memory,
	     state.hex_format, 1,
	     state.dos_newlines, 1);
  }
  
  return;
}
