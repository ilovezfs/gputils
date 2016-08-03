/* Some helpful utility functions for gpasm.
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Craig Franklin

    Copyright (C) 2015-2016 Molnar Karoly

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
#include "symbol_list.h"

#define STR_INHX8M                  "inhx8m"
#define STR_INHX8S                  "inhx8s"
#define STR_INHX16                  "inhx16"
#define STR_INHX32                  "inhx32"

#define HVM_NONE	            0
#define HVM_BEGIN	            1
#define HVM_NAME	            2

/*------------------------------------------------------------------------------------------------*/

/* MPASM compatible stripped version of strtoul:
 * The _strtoi returns low sizeof(int) * 8 bits of the value:
 * value & ((2 ^ (sizeof(int) * 8)) - 1);
 * The strtoul returns ULONG_MAX if the correct value is outside the range
 * of representable values (see man strtoul). The behavior is also
 * platform dependent: (int)strtoul("123456789", 16) returns -1 on 32 bit
 * platforms and 0x23456789 on 64 bit platforms. */

static int
_strtoi(const char *String, char **Endptr, int Radix)
{
  int value = 0;
  int sign  = 1;
  int digit = 0;
  int ch;

  if (String[0] == '-') {
    sign = -1;
    ++String;
  }

  while (true) {
    ch = *String;

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

    if (digit < Radix) {
      value = value * Radix + digit;
    }
    else {
      break;
    }

    ++String;
  }

  *Endptr = (char *)String;
  return (value * sign);
}

/*------------------------------------------------------------------------------------------------*/

int
string_to_int(const char *String, int Radix)
{
  char *endptr;
  int   value;
  char  ch;
  char  buf[80];

  value = _strtoi(String, &endptr, Radix);
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

/*
    String: An text which is probably represents a number.
    Type  : An address of type of number: bin, dec, oct or hex
*/

long
gp_strtol(const char *String, numstring_t *Type)
{
  char    ch0;
  char    ch1;
  char    ch_last;
  char   *end;
  long    val;
  size_t  length;

  assert(String != NULL);
  assert(Type != NULL);

  *Type  = NUM_STR_UNKNOWN;
  length = strlen(String);

  if (length == 0) {
    return 0;
  }

  ch0     = String[0];
  ch1     = String[1];
  ch_last = String[length - 1];

  if ((ch0 == 'b') || (ch0 == 'B')) {
    if (ch1 == '\'') {
      String += 2;
      /* b'___' OR B'___' */
      val = strtol(String, &end, 2);

      if ((end > String) && (end[0] == '\'') && (end[1] == '\0')) {
        *Type = NUM_STR_BIN;
        return val;
      }
    }
  }

  if ((ch0 == 'o') || (ch0 == 'O') || (ch0 == 'q') || (ch0 == 'Q')) {
    if (ch1 == '\'') {
      String += 2;
      /* o'___' OR O'___' OR q'___' OR Q'___' */
      val = strtol(String, &end, 8);

      if ((end > String) && (end[0] == '\'') && (end[1] == '\0')) {
        *Type = NUM_STR_OCT;
        return val;
      }
    }
  }

  if ((ch0 == 'd') || (ch0 == 'D')) {
    if (ch1 == '\'') {
      String += 2;
      /* d'___' OR D'___' */
      val = strtol(String, &end, 10);

      if ((end > String) && (end[0] == '\'') && (end[1] == '\0')) {
        *Type = NUM_STR_DEC;
        return val;
      }
    }
  }

  if ((ch0 == 'h') || (ch0 == 'H')) {
    if (ch1 == '\'') {
      String += 2;
      /* h'___' OR H'___' */
      val = strtol(String, &end, 16);

      if ((end > String) && (end[0] == '\'') && (end[1] == '\0')) {
        *Type = NUM_STR_HEX;
        return val;
      }
    }
  }

  if (ch0 == '.') {
    String += 1;
    /* .___ */
    val = strtol(String, &end, 10);

    if ((end > String) && (*end == '\0')) {
      *Type = NUM_STR_DEC;
      return val;
    }
  }

  if (ch0 == '0') {
    if ((ch1 == 'x') || (ch1 == 'X')) {
      String += 2;
      /* 0x___ OR 0X___ */
      val = strtol(String, &end, 16);

      if ((end > String) && (*end == '\0')) {
        *Type = NUM_STR_HEX;
        return val;
      }
    }
  }

  if ((ch_last == 'b') || (ch_last == 'B')) {
    /* ___b OR ___B */
    val = strtol(String, &end, 2);

    if ((end > String) && ((end[0] == 'b') || (end[0] == 'B')) && (end[1] == '\0')) {
      *Type = NUM_STR_BIN;
      return val;
    }
  }

  if ((ch_last == 'o') || (ch_last == 'O')) {
    /* ___o OR ___O */
    val = strtol(String, &end, 8);

    if ((end > String) && ((end[0] == 'o') || (end[0] == 'O')) && (end[1] == '\0')) {
      *Type = NUM_STR_OCT;
      return val;
    }
  }

  if ((ch_last == 'q') || (ch_last == 'Q')) {
    /* ___q OR ___Q */
    val = strtol(String, &end, 8);

    if ((end > String) && ((end[0] == 'q') || (end[0] == 'Q')) && (end[1] == '\0')) {
      *Type = NUM_STR_OCT;
      return val;
    }
  }

  if ((ch_last == 'd') || (ch_last == 'D')) {
    /* ___d OR ___D */
    val = strtol(String, &end, 10);

    if ((end > String) && ((end[0] == 'd') || (end[0] == 'D')) && (end[1] == '\0')) {
      *Type = NUM_STR_DEC;
      return val;
    }
  }

  if ((ch_last == 'h') || (ch_last == 'H')) {
    /* ___h OR ___H */
    val = strtol(String, &end, 16);

    if ((end > String) && ((end[0] == 'h') || (end[0] == 'H')) && (end[1] == '\0')) {
      *Type = NUM_STR_HEX;
      return val;
    }
  }

  /* ___ */
  val = strtol(String, &end, state.radix);

  if ((end > String) && (*end == '\0')) {
    switch (state.radix) {
      case 8:
        *Type = NUM_STR_OCT;
        break;

      case 10:
        *Type = NUM_STR_DEC;
        break;

      case 16:
        *Type = NUM_STR_HEX;
        break;

      default:
        *Type = NUM_STR_UNKNOWN;
    }
  }

  return val;
}

/*------------------------------------------------------------------------------------------------*/

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
gpasm_magic(const char *Str)
{
  if (Str[0] == '\\') {
    switch (Str[1]) {
      case 'a': return '\a';
      case 'b': return '\b';
      case 'f': return '\f';
      case 'n': return '\n';
      case 'r': return '\r';
      case 't': return '\t';
      case 'v': return '\v';
      default:  return Str[1];
    }
  }

  return Str[0];
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
convert_escaped_char(char *Str, char Ch)
{
  const char *src;
  char       *dest;

  if (Str == NULL) {
    return Str;
  }

  src  = Str;
  dest = Str;
  while (src[0] != '\0') {
    if ((src[0] == '\\') && (src[1] == Ch)) {
      src++;
    }

    *dest++ = *src++;
  }

  *dest = '\0';

  return Str;
}

/*------------------------------------------------------------------------------------------------*/

/* Determine the value of the escape char pointed to by ps.  Return a pointer
   to the next character. */

const char *
convert_escape_chars(const char *Ps, int *Value)
{
  int  count;
  char ch;
  char buffer[3];

  if (*Ps != '\\') {
    *Value = *Ps++;
  }
  else {
    /* escape char, convert its value and write to the new string */
    switch (Ps[1]) {
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
        *Value = 0;
        Ps++;

        while (count < 3) {
          ch = *Ps;
          if ((ch < '0') || (ch > '7')) {
            break;
          }

          *Value = (*Value << 3) + ch - '0';
          Ps++;
          count++;
        }
        break;
      }

      case 'x': {
        /* hex number */
        if ((Ps[2] == '\0') || (Ps[3] == '\0')) {
          gpmsg_error(GPE_UNKNOWN, "Missing hex value in \\x escape character.");
          *Value = 0;
          /* return a NULL character */
          Ps += 2;

          if (*Ps != '\0') {
            ++Ps;
          }
        }
        else {
          buffer[0] = Ps[2];
          buffer[1] = Ps[3];
          buffer[2] = '\0';
          *Value = string_to_int(buffer, 16);
          Ps += 4;
        }
        break;
      }

      default: {
        if (Ps[1] == '\0') {
          gpmsg_error(GPE_UNKNOWN, "Missing value in \\ escape character.");
          *Value = 0;
          /* return a NIL character */
          Ps++;
        }
        else {
          *Value = gpasm_magic(Ps);
          Ps += 2;
        }

        break;
      }
    }
  }

  return Ps;
}

/*------------------------------------------------------------------------------------------------*/

/* In some contexts, such as in the operand to a literal instruction, a
 * single-character string literal in an expression can be coerced to a
 * character literal. coerce_str1 converts a string-type pnode to a
 * constant-type pnode in-place. */

void
coerce_str1(pnode_t *Exp)
{
  int         value;
  const char *pc;

  if ((Exp != NULL) && PnIsString(Exp)) {
    pc = convert_escape_chars(PnString(Exp), &value);

    if (*pc == '\0') {
      /* castable string, make the conversion */
      Exp->tag        = PTAG_CONSTANT;
      PnConstant(Exp) = value;
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
set_symbol_attr(int *Section_number, unsigned int *Class, enum gpasmValTypes Type)
{
  int          n_section;
  unsigned int cl;
  gp_boolean   valid;

  switch (Type) {
    case VAL_EXTERNAL:
      n_section = N_UNDEF;
      cl        = C_EXT;
      valid     = true;
      break;

    case VAL_GLOBAL:
      n_section = state.obj.section_num;
      cl        = C_EXT;
      valid     = true;
      break;

    case VAL_STATIC:
      n_section = state.obj.section_num;
      cl        = C_STAT;
      valid     = true;
      break;

    case VAL_ADDRESS:
      n_section = state.obj.section_num;
      cl        = C_LABEL;
      valid     = true;
      break;

    case VAL_ABSOLUTE:
      n_section = N_ABS;
      cl        = C_NULL;
      valid     = true;
      break;

    case VAL_DEBUG:
      n_section = N_DEBUG;
      cl        = C_NULL;
      valid     = true;
      break;

    default:
      valid     = false;
  }

  if (valid) {
    if (Section_number != NULL) {
      *Section_number = n_section;
    }

    if (Class != NULL) {
      *Class = cl;
    }
  }

  return valid;
}

/*------------------------------------------------------------------------------------------------*/

void
set_global(const char *Name, gpasmVal Value, enum gpasmValTypes Type, gp_boolean Proc_dependent,
           gp_boolean Has_no_value)
{
  symbol_t     *sym;
  variable_t   *var;
  unsigned int  flags;
  int           section_number;
  unsigned int  class;

  /* Search the entire stack (i.e. include macro's local symbol tables) for the symbol.
     If not found, then add it to the global symbol table.  */
  sym = gp_sym_get_symbol(state.stTop, Name);

  if (sym == NULL) {
    sym = gp_sym_add_symbol(state.stGlobal, Name);
  }

  var = (variable_t *)gp_sym_get_symbol_annotation(sym);

  if (var == NULL) {
    /* new symbol */
    flags  = (Proc_dependent) ? VATRR_PROC_DEPENDENT : 0;
    flags |= (Has_no_value)   ? VATRR_HAS_NO_VALUE   : 0;

    var = GP_Malloc(sizeof(*var));
    var->value              = Value;
    var->coff_symbol_num    = state.obj.symbol_num;
    var->coff_section_num   = state.obj.section_num;
    var->coff_section_flags = state.obj.new_sect_flags;
    var->type               = Type;
    var->previous_type      = Type;
    var->flags              = flags;
    gp_sym_annotate_symbol(sym, var);

    if (set_symbol_attr(&section_number, &class, Type)) {
      /* Gives to the list the properties of this prospective symbol. */
      symbol_list_add_symbol(sym, Name, state.obj.symbol_num, section_number, class, state.byte_addr);
      /* Increment the index into the coff symbol table for the relocations. */
      state.obj.symbol_num++;
    }
  }
  else if (Type == VAL_VARIABLE) {
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
     var->value = Value;
     FlagClr(var->flags, VATRR_HAS_NO_VALUE);
  }
  else if (state.pass == 2) {
    if (FlagIsSet(var->flags, VATRR_HAS_NO_VALUE)) {
      msg_has_no_value(NULL, Name);
    }
    else if (var->value != Value) {
      gpmsg_verror(GPE_DIFFLAB, NULL, Name);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Returns descriptor of a constant or variable. */

variable_t *
get_global_constant(const char *Name)
{
  const symbol_t *sym;
  variable_t     *var;

  if (((sym = gp_sym_get_symbol(state.stGlobal, Name)) != NULL) &&
      ((var = gp_sym_get_symbol_annotation(sym)) != NULL) &&
      ((var->type == VAL_CONSTANT) || (var->type == VAL_VARIABLE))) {

    if (FlagIsSet(var->flags, VATRR_HAS_NO_VALUE)) {
      gpmsg_vwarning(GPW_SYM_NO_VALUE, NULL, Name);
    }

    return var;
  }

  return NULL;
}

/*------------------------------------------------------------------------------------------------*/

/* Clean out the variables. */

void
delete_variable_symbols(symbol_table_t *Table)
{
  size_t            i;
  symbol_t         *sym;
  const variable_t *var;

  if (Table == NULL) {
    return;
  }

  for (i = 0; i < gp_sym_get_symbol_count(Table); ) {
    sym = gp_sym_get_symbol_with_index(Table, i);
    assert(sym != NULL);

    var = (const variable_t *)gp_sym_get_symbol_annotation(sym);

    if ((var != NULL) && (var->type == VAL_VARIABLE)) {
      gp_sym_remove_symbol_with_index(Table, i);
    }
    else {
      ++i;
    }
  }

  delete_variable_symbols(gp_sym_get_guest_table(Table));
}

/*------------------------------------------------------------------------------------------------*/

/* Clean out the processor dependent variables. */

void
delete_processor_variable_symbols(symbol_table_t *Table)
{
  size_t            i;
  symbol_t         *sym;
  const variable_t *var;

  if (Table == NULL) {
    return;
  }

  for (i = 0; i < gp_sym_get_symbol_count(Table); ) {
    sym = gp_sym_get_symbol_with_index(Table, i);
    assert(sym != NULL);

    var = (const variable_t *)gp_sym_get_symbol_annotation(sym);

    if ((var != NULL) && (var->type == VAL_VARIABLE) && FlagIsSet(var->flags, VATRR_PROC_DEPENDENT)) {
      gp_sym_remove_symbol_with_index(Table, i);
    }
    else {
      ++i;
    }
  }

  delete_processor_variable_symbols(gp_sym_get_guest_table(Table));
}

/*------------------------------------------------------------------------------------------------*/

void
select_error_level(int Level)
{
  if (state.cmd_line.error_level) {
    gpmsg_vmessage(GPM_SUPVAL, NULL);
  }
  else {
    if ((Level >= 0) && (Level <= 2)) {
      if (state.cmd_line.strict_level && (state.strict_level > 0)) {
        /*
          The "strict messages" more important than the other messages, therefore
          enable each messages.
        */
        state.error_level = 0;
      }
      else {
        state.error_level = Level;
      }
    }
    else {
      if (state.pass == 0) {
        fprintf(stderr, "Error: Invalid warning level \"%i\".\n", Level);
      }
      else {
        gpmsg_error(GPE_ILLEGAL_ARGU, "Expected w= 0, 1, 2");
      }
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

void
select_strict_level(int Level)
{
  if (state.cmd_line.strict_level) {
    gpmsg_vmessage(GPM_SUPVAL, NULL);
  }
  else {
    if ((Level >= 0) && (Level <= 2)) {
      state.strict_level = Level;

      if ((Level > 0) && state.cmd_line.error_level) {
        /* Enable each messages. */
        state.error_level = 0;
      }
    }
    else {
      if (state.pass == 0) {
        fprintf(stderr, "Error: Invalid strict level \"%i\".\n", Level);
      }
      else {
        gpmsg_error(GPE_ILLEGAL_ARGU, "Expected S= 0, 1, 2");
      }
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

void
select_expand(const char *Expand)
{
  if (state.cmd_line.macro_expand) {
    gpmsg_vmessage(GPM_SUPLIN, NULL);
  }
  else {
    if (strcasecmp(Expand, "on") == 0) {
      state.lst.expand = true;
    }
    else if (strcasecmp(Expand, "off") == 0) {
      state.lst.expand = false;
    }
    else {
      state.lst.expand = true;

      if (state.pass == 0) {
        fprintf(stderr, "Error: Invalid option: \"%s\"\n", Expand);
      }
      else {
        gpmsg_error(GPE_ILLEGAL_ARGU, "Expected ON or OFF.");
      }
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

void
select_hex_format(const char *Format_name)
{
  if (state.cmd_line.hex_format) {
    gpmsg_vwarning(GPW_CMDLINE_HEXFMT, NULL);
  }
  else {
    if (strcasecmp(Format_name, STR_INHX8M) == 0) {
      state.hex_format = INHX8M;
    }
    else if (strcasecmp(Format_name, STR_INHX8S) == 0) {
      state.hex_format = INHX8S;
    }
    else if (strcasecmp(Format_name, STR_INHX16) == 0) {
      state.hex_format = INHX16;
    }
    else if (strcasecmp(Format_name, STR_INHX32) == 0) {
      state.hex_format = INHX32;
    }
    else {
      state.hex_format = INHX8M;

      if (state.pass == 0) {
        fprintf(stderr, "Error: Invalid format: \"%s\"\n", Format_name);
      }
      else {
        gpmsg_error(GPE_ILLEGAL_ARGU, "Expected " STR_INHX8M ", " STR_INHX8S ", " STR_INHX16 ", or " STR_INHX32 ".");
      }
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

void
select_radix(const char *Radix_name)
{
  if (state.cmd_line.radix) {
    gpmsg_vwarning(GPW_CMDLINE_RADIX, NULL);
  }
  else {
    if ((strcasecmp(Radix_name, "h") == 0) ||
        (strcasecmp(Radix_name, "hex") == 0) ||
        (strcasecmp(Radix_name, "hexadecimal") == 0)) {
      state.radix = 16;
    }
    else if ((strcasecmp(Radix_name, "d") == 0) ||
             (strcasecmp(Radix_name, "dec") == 0) ||
             (strcasecmp(Radix_name, "decimal") == 0)) {
      state.radix = 10;
    }
    else if ((strcasecmp(Radix_name, "o") == 0) ||
             (strcasecmp(Radix_name, "oct") == 0) ||
             (strcasecmp(Radix_name, "octal") == 0)) {
      state.radix = 8;
    }
    else {
      state.radix = 16;

      if (state.pass == 0) {
        fprintf(stderr, "Error: Invalid radix \"%s\", will use hex.\n", Radix_name);
      }
      else {
        gpmsg_vwarning(GPW_RADIX, NULL);
      }
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

gpasmVal
do_or_append_insn(const char *Op, pnode_t *Parms)
{
  if (IN_MACRO_WHILE_DEFINITION) {
    if (strcasecmp(Op, "endm") == 0) {
      return do_insn(Op, Parms);
    }
    else if (IN_WHILE_DEFINITION) {
      if (strcasecmp(Op, "while") == 0) {
        assert(state.while_depth != 0);
        ++state.while_depth;
      }
      else if ((state.while_head != NULL) && (strcasecmp(Op, "endw") == 0)) {
        assert(state.while_depth != 0);
        if (--state.while_depth == 0) {
          return do_insn(Op, Parms);
        }
      }
    }

    macro_append();
    return 0;
  }
  else {
    return do_insn(Op, Parms);
  }
}

/*------------------------------------------------------------------------------------------------*/

/* Build string from a macro parameter list.*/

char *
macro_params_to_string(char *String, size_t String_max_length, size_t *Length, const pnode_t *Macro_params)
{
  size_t l;
  size_t size;

  if (Macro_params == NULL) {
    return String;
  }

  assert(String_max_length > 0);

  switch (Macro_params->tag) {
    case PTAG_LIST:
      String = macro_params_to_string(String, String_max_length, Length, PnListHead(Macro_params));
      String = macro_params_to_string(String, String_max_length, Length, PnListTail(Macro_params));
      break;

    case PTAG_SYMBOL: {
      if (String == NULL) {
        String    = GP_Malloc(String_max_length + 1);
        String[0] = '\0';
        *Length   = 0;
      }

      l = *Length;

      if (l == 0) {
        /* This is the first parameter. */
        l = snprintf(String, String_max_length, "%s", PnSymbol(Macro_params));
      }
      else {
        size = (String_max_length > l) ? (String_max_length - l) : 0;

        if (size > 0) {
          l += snprintf(String + l, size, ", %s", PnSymbol(Macro_params));
        }
      }

      *Length = l;
      break;
    }

    case PTAG_CONSTANT:
    case PTAG_STRING:
    case PTAG_OFFSET:
    case PTAG_BINOP:
    case PTAG_UNOP:
    default:
      break;
  }

  return String;
}

/*------------------------------------------------------------------------------------------------*/

const char *
variable_type_to_str(enum gpasmValTypes Type)
{
  switch (Type) {
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
value_type_to_str(const variable_t *Variable, gp_boolean Previous)
{
  enum gpasmValTypes type;

  if (Variable == NULL) {
    return NULL;
  }

  type = (Previous) ? Variable->previous_type : Variable->type;
  return variable_type_to_str(type);
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

void
msg_has_no_value(const char *Optional_text, const char *Symbol_name)
{
  if (state.mpasm_compatible) {
    return;
  }

  switch (state.strict_level) {
    case 1:
      gpmsg_vwarning(GPW_SYM_NO_VALUE, Optional_text, Symbol_name);
      break;

    case 2:
      gpmsg_verror(GPE_SYM_NO_VALUE, Optional_text, Symbol_name);
      break;
  }
}

/*------------------------------------------------------------------------------------------------*/

/*
static void
_print_macro_line(const macro_body_t *Mac)
{
  if (Mac->src_line != NULL) {
    printf(" src_line = %s\n", Mac->src_line);
  }
}

static void
_print_macro_body(const macro_body_t *Mac)
{
  const macro_body_t *mb = Mac;

  printf("{\n");
  while(mb != NULL) {
    _print_macro_line(mb);
    mb = mb->next;
  }
  printf("}\n");
}*/

/*------------------------------------------------------------------------------------------------*/

/* Function to append a line to an ongoing macro definition. */

void
macro_append(void)
{
  macro_body_t *body = GP_Malloc(sizeof(*body));

  body->src_line = NULL;
  body->next     = NULL;            /* make sure it's terminated */

  *state.mac_prev = body;           /* append this to the chain */
  state.mac_prev  = &body->next;    /* this is the new end of the chain */
  state.mac_body  = body;
}

/*------------------------------------------------------------------------------------------------*/

void
hex_create(void)
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
