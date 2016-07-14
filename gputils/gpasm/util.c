/* Some helpful utility functions for gpasm
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Craig Franklin

    Copyright (C) 2015-2016 Molnar Karoly <molnarkaroly@users.sf.net>

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
#include "gpmsg.h"
#include "directive.h"
#include "coff.h"

#define STR_INHX8M              "inhx8m"
#define STR_INHX8S              "inhx8s"
#define STR_INHX16              "inhx16"
#define STR_INHX32              "inhx32"

/*------------------------------------------------------------------------------------------------*/

/* MPASM compatible stripped version of strtoul:
 * gp_strtoi returns low sizeof(int) * 8 bits of the value:
 * value & ((2 ^ (sizeof(int) * 8)) - 1);
 * strtoul returns ULONG_MAX if the correct value is outside the range
 * of representable values (see man strtoul). The behavior is also
 * platform dependent: (int)strtoul("123456789", 16) returns -1 on 32 bit
 * platforms and 0x23456789 on 64 bit platforms. */

static int
_strtoi(const char *string, char **endptr, int radix)
{
  int value = 0;
  int sign  = 1;
  int digit = 0;
  int ch;

  if (*string == '-') {
    sign = -1;
    ++string;
  }

  while (true) {
    ch = *string;

    if (isdigit(ch)) {
      digit = ch - '0';
    }
    else if (islower(ch)) {
      digit = ch - ('a' - 10);
    }
    else if (isupper(ch)) {
      digit = ch - ('A' - 10);
    }
    else {
      break;
    }

    if (digit < radix) {
      value = value * radix + digit;
    }
    else {
      break;
    }

    ++string;
  }

  *endptr = (char *)string;
  return (value * sign);
}

/*------------------------------------------------------------------------------------------------*/

int
stringtolong(const char *string, int radix)
{
  char *endptr;
  int   value;
  char  ch;
  char  buf[80];

  value = _strtoi(string, &endptr, radix);
  if ((endptr == NULL) || ((ch = *endptr) != '\0')) {
    snprintf(buf, sizeof(buf),
             isprint(ch) ? "Illegal character '%c' in numeric constant." :
                           "Illegal character %#x in numeric constant.",
             ch);
    gpmsg_error(GPE_UNKNOWN, buf);
  }

  return value;
}

/*------------------------------------------------------------------------------------------------*/

#define HVM_NONE	0
#define HVM_BEGIN	1
#define HVM_NAME	2

static gp_boolean
_find_hv_macro_start(const char *String, const char **Start, const char **Body)
{
  const char *ptr;
  char        ch;
  int         state;

  if ((String == NULL) || (*String == '\0')) {
    return false;
  }

  ptr = String;
  do {
    *Start = NULL;
    *Body  = NULL;
    state  = HVM_NONE;
    do {
      /* Find the beginner '#' character. */
      while (true) {
        if ((ch = *ptr) == '\0') {
          /* Too soon it's over the string. */
          return false;
        }

        if (ch == '#') {
          /* This is the beginner '#' character. */
          *Start = ptr;
          ++ptr;
          state = HVM_BEGIN;
          break;
        }

        ++ptr;
      }

      /* Skip the ' ' characters. */
      while (true) {
        if ((ch = *ptr) == '\0') {
          /* Too soon it's over the string. */
          return false;
        }

        if (ch != ' ') {
          break;
        }

        ++ptr;
      }
      /* Next if this a '#' character. */
    } while (ch == '#');

    /* Find the 'v' or 'V' character. */
    do {
      if ((ch = *ptr) == '\0') {
        /* Too soon it's over the string. */
        return false;
      }

      ++ptr;

      if ((ch == 'v') || (ch == 'V')) {
        /* This a "v" macro. */
        state = HVM_NAME;
        break;
      }
    } while (ch == ' ');
  } while (state != HVM_NAME);

  /* Find the '(' character. */
  while (true) {
    ch = *ptr;

    if (ch == '(') {
      /* This a body of "v" macro. */
      *Body = ptr;
      return true;
    }

    if ((ch == '\0') || (ch != ' ')) {
      /* Too soon it's over the string or this no space character. */
      return false;
    }

  ++ptr;
  }
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
find_hv_macro(const char *String, const char **Start, const char **End)
{
  const char *ptr;
  int         bracket_count;
  char        ch;
  const char *body;

  if ((String == NULL) || (*String == '\0')) {
    return false;
  }

  if (!_find_hv_macro_start(String, Start, &body)) {
    return false;
  }

  bracket_count = 1;
  ptr = body;
  while (true) {
    ++ptr;
    if ((ch = *ptr) == '\0') {
      break;
    }

    if (ch == '(') {
      ++bracket_count;
    }
    else if (ch == ')') {
      --bracket_count;
    }

    if (bracket_count == 0) {
      /* Found the ending bracket, finish. */
      ++ptr;
      *End = ptr;
      return true;
    }
  }

  return false;
}

/*------------------------------------------------------------------------------------------------*/

int
gpasm_magic(const char *c)
{
  if (c[0] == '\\') {
    switch (c[1]) {
      case 'a': return '\a';
      case 'b': return '\b';
      case 'f': return '\f';
      case 'n': return '\n';
      case 'r': return '\r';
      case 't': return '\t';
      case 'v': return '\v';
      default:  return c[1];
    }
  }

  return c[0];
}

/*------------------------------------------------------------------------------------------------*/

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

  if (str == NULL) {
    return str;
  }

  while (*src != '\0') {
    if ((*src =='\\') && (src[1] == c)) {
      src++;
    }
    *dest++ = *src++;
  }

  *dest = '\0';

  return str;
}

/*------------------------------------------------------------------------------------------------*/

/* Determine the value of the escape char pointed to by ps.  Return a pointer
   to the next character. */

const char *
convert_escape_chars(const char *ps, int *value)
{
  int  count;
  char buffer[3];

  if (*ps != '\\') {
    *value = *ps++;
  }
  else {
    /* escape char, convert its value and write to the new string */
    switch (ps[1]) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7': {
        /* octal number */
        count = 0;
        *value = 0;
        ps++;

        while (count < 3) {
          if ((*ps < '0') || (*ps > '7')) {
            break;
          }

          *value = (*value << 3) + *ps - '0';
          ps++;
          count++;
        }
        break;
      }

      case 'x': {
        /* hex number */
        if ((ps[2] == '\0') || (ps[3] == '\0')) {
          gpmsg_error(GPE_UNKNOWN, "Missing hex value in \\x escape character.");
          *value = 0;
          /* return a NULL character */
          ps += 2;

          if (*ps != '\0') {
            ++ps;
          }
        }
        else {
          buffer[0] = ps[2];
          buffer[1] = ps[3];
          buffer[2] = '\0';
          *value = stringtolong(buffer, 16);
          ps += 4;
        }
        break;
      }

      default: {
        if (ps[1] == '\0') {
          gpmsg_error(GPE_UNKNOWN, "Missing value in \\ escape character.");
          *value = 0;
          /* return a NIL character */
          ps++;
        }
        else {
          *value = gpasm_magic(ps);
          ps += 2;
        }

        break;
      }
    }
  }

  return ps;
}

/*------------------------------------------------------------------------------------------------*/

/* In some contexts, such as in the operand to a literal instruction, a
 * single-character string literal in an expression can be coerced to a
 * character literal. coerce_str1 converts a string-type pnode to a
 * constant-type pnode in-place. */

void
coerce_str1(pnode_t *exp)
{
  int         value;
  const char *pc;

  if ((exp != NULL) && PnIsString(exp)) {
    pc = convert_escape_chars(PnString(exp), &value);

    if (*pc == '\0') {
      /* castable string, make the conversion */
      exp->tag = PTAG_CONSTANT;
      PnConstant(exp) = value;
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

void
set_global(const char *name, gpasmVal value, enum gpasmValTypes type, gp_boolean procDependent)
{
  symbol_t   *sym;
  variable_t *var;
  char       *coff_name;

  /* Search the entire stack (i.e. include macro's local symbol tables) for the symbol.
     If not found, then add it to the global symbol table.  */
  sym = gp_sym_get_symbol(state.stTop, name);

  if (sym == NULL) {
    sym = gp_sym_add_symbol(state.stGlobal, name);
  }

  var = gp_sym_get_symbol_annotation(sym);

  if (var == NULL) {
    /* new symbol */
    var = GP_Malloc(sizeof(*var));
    var->value              = value;
    var->coff_symbol_num    = state.obj.symbol_num;
    var->coff_section_num   = state.obj.section_num;
    var->coff_section_flags = state.obj.new_sect_flags;
    var->type               = type;
    var->previous_type      = type;     /* coff symbols can be changed to global */
    var->flags              = (procDependent) ? VATRR_PROC_DEPENDENT : 0;
    gp_sym_annotate_symbol(sym, var);

    /* increment the index into the coff symbol table for the relocations */
    switch (type) {
      case VAL_EXTERNAL:
      case VAL_GLOBAL:
      case VAL_STATIC:
      case VAL_ADDRESS:
      case VAL_DEBUG:
      case VAL_ABSOLUTE:
        state.obj.symbol_num++;
        break;

      default:
        break;
    }
  }
  else if (type == VAL_VARIABLE) {
    /*
     * TSD - the following embarrassing piece of code is a hack
     *       to fix a problem when global variables are changed
     *       during the expansion of a macro. Macros are expanded
     *       by running through them twice. If you have a statement
     *       like:
     *       some_var set some_var + 1
     *       Then this is incremented twice! So the if statement
     *       makes sure that the value is assigned on the second
     *       pass only in the macro. Jeez this really sucks....
     */
     var->value = value;

  }
  else if (state.pass == 2) {
    if (var->value != value) {
      gpmsg_verror(GPE_DIFFLAB, NULL, name);
    }

    coff_name = coff_local_name(name);
    coff_add_sym(coff_name, value, var->type);

    if (coff_name != NULL) {
      free(coff_name);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

variable_t *
get_global_constant(const char *Name)
{
  const symbol_t *sym;
  variable_t     *var;

  if (((sym = gp_sym_get_symbol(state.stGlobal, Name)) != NULL) &&
      ((var = gp_sym_get_symbol_annotation(sym)) != NULL) &&
      ((var->type == VAL_CONSTANT) || (var->type == VAL_VARIABLE))) {
    return var;
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

void
purge_variable_symbols(symbol_table_t *Table)
{
  size_t      i;
  symbol_t   *sym;
  variable_t *var;

  if (Table == NULL) {
    return;
  }

  for (i = 0; i < gp_sym_get_symbol_count(Table); ) {
    sym = gp_sym_get_symbol_with_index(Table, i);
    assert(sym != NULL);

    var = (variable_t *)gp_sym_get_symbol_annotation(sym);

    if ((var != NULL) && (var->type == VAL_VARIABLE)) {
      gp_sym_remove_symbol_with_index(Table, i);
    }
    else {
      ++i;
    }
  }

  purge_variable_symbols(gp_sym_get_guest_table(Table));
}

/*------------------------------------------------------------------------------------------------*/

void
purge_processor_variable_symbols(symbol_table_t *Table)
{
  size_t      i;
  symbol_t   *sym;
  variable_t *var;

  if (Table == NULL) {
    return;
  }

  for (i = 0; i < gp_sym_get_symbol_count(Table); ) {
    sym = gp_sym_get_symbol_with_index(Table, i);
    assert(sym != NULL);

    var = (variable_t *)gp_sym_get_symbol_annotation(sym);

    if ((var != NULL) && (var->type == VAL_VARIABLE) && FlagIsSet(var->flags, VATRR_PROC_DEPENDENT)) {
      gp_sym_remove_symbol_with_index(Table, i);
    }
    else {
      ++i;
    }
  }

  purge_processor_variable_symbols(gp_sym_get_guest_table(Table));
}

/*------------------------------------------------------------------------------------------------*/

void
select_errorlevel(int level)
{
  if (state.cmd_line.error_level) {
    gpmsg_vmessage(GPM_SUPVAL, NULL);
  }
  else {
    if ((level >= 0) && (level <= 2)) {
      if (state.cmd_line.strict_level && (state.strict_level > 0)) {
        /*
        The "strict messages" more important than the other messages, therefore
        enable each messages.
        */
        state.error_level = 0;
      }
      else {
        state.error_level = level;
      }
    }
    else {
      if (state.pass == 0) {
        fprintf(stderr, "Error: Invalid warning level \"%i\".\n", level);
      }
      else {
        gpmsg_error(GPE_ILLEGAL_ARGU, "Expected w= 0, 1, 2");
      }
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

void
select_strictlevel(int level)
{
  if (state.cmd_line.strict_level) {
    gpmsg_vmessage(GPM_SUPVAL, NULL);
  }
  else {
    if ((level >= 0) && (level <= 2)) {
      state.strict_level = level;

      if ((level > 0) && state.cmd_line.error_level) {
        /* Enable each messages. */
        state.error_level = 0;
      }
    }
    else {
      if (state.pass == 0) {
        fprintf(stderr, "Error: Invalid strict level \"%i\".\n", level);
      }
      else {
        gpmsg_error(GPE_ILLEGAL_ARGU, "Expected S= 0, 1, 2");
      }
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

void
select_expand(const char *expand)
{
  if (state.cmd_line.macro_expand) {
    gpmsg_vmessage(GPM_SUPLIN, NULL);
  }
  else {
    if (strcasecmp(expand, "on") == 0) {
      state.lst.expand = true;
    }
    else if (strcasecmp(expand, "off") == 0) {
      state.lst.expand = false;
    }
    else {
      state.lst.expand = true;

      if (state.pass == 0) {
        fprintf(stderr, "Error: Invalid option \"%s\".\n", expand);
      }
      else {
        gpmsg_error(GPE_ILLEGAL_ARGU, "Expected ON or OFF.");
      }
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

void
select_hexformat(const char *format_name)
{
  if (state.cmd_line.hex_format) {
    gpmsg_vwarning(GPW_CMDLINE_HEXFMT, NULL);
  }
  else {
    if (strcasecmp(format_name, STR_INHX8M) == 0) {
      state.hex_format = INHX8M;
    }
    else if (strcasecmp(format_name, STR_INHX8S) == 0) {
      state.hex_format = INHX8S;
    }
    else if (strcasecmp(format_name, STR_INHX16) == 0) {
      state.hex_format = INHX16;
    }
    else if (strcasecmp(format_name, STR_INHX32) == 0) {
      state.hex_format = INHX32;
    }
    else {
      state.hex_format = INHX8M;

      if (state.pass == 0) {
        fprintf(stderr, "Error: Invalid format \"%s\".\n", format_name);
      }
      else {
        gpmsg_error(GPE_ILLEGAL_ARGU, "Expected " STR_INHX8M ", " STR_INHX8S ", " STR_INHX16 ", or " STR_INHX32 ".");
      }
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

void
select_radix(const char *radix_name)
{
  if (state.cmd_line.radix) {
    gpmsg_vwarning(GPW_CMDLINE_RADIX, NULL);
  }
  else {
    if ((strcasecmp(radix_name, "h") == 0) ||
        (strcasecmp(radix_name, "hex") == 0) ||
        (strcasecmp(radix_name, "hexadecimal") == 0)) {
      state.radix = 16;
    }
    else if ((strcasecmp(radix_name, "d") == 0) ||
             (strcasecmp(radix_name, "dec") == 0) ||
             (strcasecmp(radix_name, "decimal") == 0)) {
      state.radix = 10;
    }
    else if ((strcasecmp(radix_name, "o") == 0) ||
             (strcasecmp(radix_name, "oct") == 0) ||
             (strcasecmp(radix_name, "octal") == 0)) {
      state.radix = 8;
    }
    else {
      state.radix = 16;

      if (state.pass == 0) {
        fprintf(stderr, "Error: Invalid radix \"%s\", will use hex.\n", radix_name);
      }
      else {
        gpmsg_vwarning(GPW_RADIX, NULL);
      }
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Function to append a line to an ongoing macro definition. */

void
macro_append(void)
{
  macro_body_t *body = GP_Malloc(sizeof(*body));

  body->src_line = NULL;

  *state.mac_prev = body;       /* append this to the chain */
  state.mac_prev  = &body->next; /* this is the new end of the chain */
  state.mac_body  = body;
  body->next = NULL;            /* make sure it's terminated */
}

/*------------------------------------------------------------------------------------------------*/

gpasmVal
do_or_append_insn(const char *op, pnode_t *parms)
{
  if (IN_MACRO_WHILE_DEFINITION) {
    if (strcasecmp(op, "endm") == 0) {
      return do_insn(op, parms);
    }
    else if (IN_WHILE_DEFINITION) {
      if (strcasecmp(op, "while") == 0) {
        assert(state.while_depth != 0);
        ++state.while_depth;
      }
      else if ((state.while_head != NULL) && (strcasecmp(op, "endw") == 0)) {
        assert(state.while_depth != 0);
        if (--state.while_depth == 0) {
          return do_insn(op, parms);
        }
      }
    }

    macro_append();
    return 0;
  }
  else {
    return do_insn(op, parms);
  }
}


/*------------------------------------------------------------------------------------------------*/

const char *
value_type_to_str(const variable_t *Variable, gp_boolean Previous)
{
  enum gpasmValTypes type;

  if (Variable == NULL) {
    return NULL;
  }

  type = (Previous) ? Variable->previous_type : Variable->type;

  switch (type) {
    case VAL_CONSTANT: return "CONSTANT";
    case VAL_VARIABLE: return "VARIABLE";
    case VAL_ADDRESS:  return "ADDRESS";
    case VAL_CBLOCK:   return "CBLOCK";
    case VAL_EXTERNAL: return "EXTERNAL";
    case VAL_GLOBAL:   return "GLOBAL";
    case VAL_STATIC:   return "STATIC";
    case VAL_ABSOLUTE: return "ABSOLUTE";
    case VAL_DEBUG:    return "DEBUG";
    default:           return "UNKNOWN";
  }
}

/*------------------------------------------------------------------------------------------------*/

const char *
pnode_symbol_name(const pnode_t *Pnode)
{
  return (PnIsSymbol(Pnode) ? PnSymbol(Pnode) : NULL);
}

/*------------------------------------------------------------------------------------------------*/

gpasmVal
pnode_symbol_value(const pnode_t *Pnode)
{
  const symbol_t   *sym;
  const variable_t *var;
  int               addr;

  if (PnIsSymbol(Pnode)) {
    if (strcmp(PnSymbol(Pnode), "$") == 0) {
      addr = (IS_RAM_ORG ? state.byte_addr :
                           gp_processor_insn_from_byte_p(state.processor, state.byte_addr));
      return addr;
    }
    else {
      sym = gp_sym_get_symbol(state.stTop, PnSymbol(Pnode));
      assert(sym != NULL);
      var = gp_sym_get_symbol_annotation(sym);
      assert(var != NULL);
      return var->value;
    }
  }
  else {
    return 0;
  }
}

/*------------------------------------------------------------------------------------------------*/

const char *
pnode_string(const pnode_t *Pnode)
{
  return (PnIsString(Pnode) ? PnString(Pnode) : NULL);
}

/*------------------------------------------------------------------------------------------------*/

/*
static void
print_macro_node(const macro_body_t *mac)
{
  if (mac->src_line != NULL) {
    printf(" src_line = %s\n", mac->src_line);
  }
}

static void
print_macro_body(const macro_body_t *mac)
{
  const macro_body_t *mb = mac;

  printf("{\n");
  while(mb != NULL) {
    print_macro_node(mb);
    mb = mb->next;
  }
  printf("}\n");
}*/

/*------------------------------------------------------------------------------------------------*/

void
file_delete_node(void *Node)
{
  file_context_t *n;

  if (Node == NULL) {
    return;
  }

  n = (file_context_t *)Node;
  free(n->name);
  free(n);
}

/*------------------------------------------------------------------------------------------------*/

/* add_file: Add a file of type 'type' to the file_context stack. */

file_context_t *
add_file(unsigned int type, const char *name)
{
  static unsigned int  file_id = 0;

  file_context_t      *new;

  /* First check to make sure this file is not already in the list. */
  new = state.file_list.last;
  while (new != NULL) {
    if (strcmp(new->name, name) == 0) {
      return new;
    }
    new = new->prev;
  }

  new = gp_node_new(sizeof(*new));

  new->name = GP_Strdup(name);
  new->ft   = type;
  new->id   = file_id++;

  if (state.file_list.first == NULL) {
    gp_list_set_delete_node_func(&state.file_list, file_delete_node);
  }

  gp_node_append(&state.file_list, new);
  return new;
}

/*------------------------------------------------------------------------------------------------*/

/* free_files: free memory allocated to the file_context stack */

void
free_files(void)
{
  gp_list_delete(&state.file_list);
}

/*------------------------------------------------------------------------------------------------*/

void
hex_init(void)
{
  if (state.hex_file == OUT_SUPPRESS) {
    /* Must delete hex file when suppressed. */
    gp_writehex(state.base_file_name, state.i_memory, state.hex_format, 1, state.dos_newlines, 1);
    return;
  }

  if (!gp_writehex_check(state.i_memory, state.hex_format)) {
    gpmsg_verror(GPE_IHEX, NULL);
    gp_writehex(state.base_file_name, state.i_memory, state.hex_format, 1, state.dos_newlines, 1);
  }
  else if (state.device.class != NULL) {
    if (!gp_writehex(state.base_file_name, state.i_memory, state.hex_format, state.num.errors,
                     state.dos_newlines, state.device.class->core_mask)) {
      gpmsg_error(GPE_UNKNOWN, "Error generating hex file.");
      exit(1);
    }
  }
  else {
    /* Won't have anything to write, just remove any old files. */
    gp_writehex(state.base_file_name, state.i_memory, state.hex_format, 1, state.dos_newlines, 1);
  }
}
