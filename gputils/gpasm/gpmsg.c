/* Error handling for gpasm
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Craig Franklin
   Copyright (C) 2012 Borut Razem

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
#include "lst.h"

typedef struct message_code {
  enum {
    ERR_CODE_NUM,
    ERR_CODE_RANGE
  }                    type;
  int                  value0;
  int                  value1;
  struct message_code *next;
} message_code_t;

typedef enum {
  MSG_TYPE_ERROR,
  MSG_TYPE_WARNING,
  MSG_TYPE_MESSAGE
} err_type_t;

static message_code_t *message_codes_list      = NULL;
static message_code_t *message_codes_list_last = NULL;

/*------------------------------------------------------------------------------------------------*/

static gp_boolean
_check_code(int Code)
{
  const message_code_t *ec;
  int                   v0;
  int                   v1;
  gp_boolean            print;

  print = true;
  ec    = message_codes_list;
  while (ec != NULL) {
    v0 = ec->value0;

    if (ec->type == ERR_CODE_NUM) {
      if (v0 == Code) {
        print = true;
      }
      else if (v0 == (-Code)) {
        print = false;
      }
    }
    else {
      v1 = ec->value1;

      if (v0 >= 0) {
        if ((v0 <= Code) && (v1 >= Code)) {
          print = true;
        }
      }
      else {
        if ((v0 <= (-Code)) && (v1 >= (-Code))) {
          print = false;
        }
      }
    }

    ec = ec->next;
  }

  return print;
}

/*------------------------------------------------------------------------------------------------*/

static void
_free_message_codes(void)
{
  message_code_t *ec;
  message_code_t *next;

  ec = message_codes_list;
  while (ec != NULL) {
    next = ec->next;
    free(ec);
    ec = next;
  }
}

/*------------------------------------------------------------------------------------------------*/

static const char *
_get_error(int Code)
{
  switch(Code) {
    case GPE_USER:
      return "ERROR: (%s)";
    case GPE_NOENT:
      return "Cannot open file. Include file \"%s\" not found.";
    case GPE_STRCPLX:
      return "String substitution too complex.";
    case GPE_BADCHAR:
      return "Illegal character: '%c'";
    case GPE_OPENPAR:
      return "Unmatched (";
    case GPE_CLOSEPAR:
      return "Unmatched )";
    case GPE_NOSYM:
      return "Symbol not previously defined: \"%s\"";
    case GPE_DIVBY0:
      return "Divide by zero.";
    case GPE_DUPLAB:
      return "Duplicate label, \"%s\" or redefining symbol that cannot be redefined.";
    case GPE_DIFFLAB:
      return "Address label duplicated or different in second pass: \"%s\"";
    case GPE_ADDROVF:
      return "Address wrapped around 0.";
    case GPE_ADDROVR:
      return "Overwriting previous address contents: 0x%04X";
    case GPE_BAD_CALL_ADDR:
      return "Call or jump not allowed at this address, must be in low half of page.";
    case GPE_ILLEGAL_LABEL:
      return "Illegal label: \"%s\"";
    case GPE_ILLEGAL_DIR:
      return "Illegal directive: \"%s\"";
    case GPE_ILLEGAL_ARGU:
      return "Illegal argument: \"%s\"";
    case GPE_ILLEGAL_COND:
      return "Illegal condition.";
    case GPE_RANGE:
      return "Argument out of range.";
    case GPE_TOO_MANY_ARGU:
      return "Too many arguments.";
    case GPE_MISSING_ARGU:
      return "Missing argument(s).";
    case GPE_EXPECTED:
      return "Expected.";
    case GPE_EXTRA_PROC:
      return "Processor type previously defined.";
    case GPE_UNDEF_PROC:
      return "Processor type is undefined.";
    case GPE_UNKNOWN_PROC:
      return "Unknown processor: \"%s\"";
    case GPE_IHEX:
      return "Hex file format INHX32 required.";
    case GPE_NO_MACRO_NAME:
      return "Macro name missing.";
    case GPE_DUPLICATE_MACRO:
      return "Duplicate macro name.";
    case GPE_BAD_WHILE_LOOP:
      return "WHILE must terminate within 256 iterations.";
    case GPE_ILLEGAL_NESTING:
      return "Illegal nesting.";
    case GPE_UNMATCHED_ENDM:
      return "Unmatched ENDM.";
    case GPE_OBJECT_ONLY:
      return "Directive only allowed when generating an object file: \"%s\"";
    case GPE_LABEL_IN_SECTION:
      return "Labels must be defined in a code or data section when making an object file.";
    case GPE_UNRESOLVABLE:
      return "Operand contains unresolvable labels or is too complex.";
    case GPE_WRONG_SECTION:
      return "Executable code and data must be defined in an appropriate section.";
    case GPE_CONTIG_SECTION:
      return "Each object file section must be contiguous: \"%s\"";
    case GPE_MUST_BE_LABEL:
      return "Operand must be an address label.";
    case GPE_ORG_ODD:
      return "ORG at odd address.";
    case GPE_FILL_ODD:
      return "Cannot use FILL Directive with odd number of bytes.";
    case GPE_CONTIG_CONFIG:
      return "__CONFIG directives must be contiguous.";
    case GPE_CONTIG_IDLOC:
      return "__IDLOC directives must be contiguous.";
    case GPE_NO_EXTENDED_MODE:
      return "Extended mode not available for this device.";
    case GPE_MISSING_BRACKET:
      return "Square brackets required around offset operand.";
    case GPE_CONSTANT:
      return "Expression within brackets must be constant.";
    case GPE_IDLOCS_ORDER:
      return "__IDLOCS directives must be listed in ascending order.";
    case GPE_CONFIG_UNKNOWN:
    return "An error with the CONFIG directive occured.";
    case GPE_CONFIG_usCONFIG:
      return "You cannot mix CONFIG and __CONFIG directives.";
    case GPE_RES_ODD_PIC16EA:
      return "RES directive cannot reserve odd number of bytes in PIC18 absolute mode.";
    case GPE_UNKNOWN:
      return "";

    /* gputils special errors */
    case GPE_INTERNAL:
      return "Internal error: %s";
    case GPE_PARSER:
      return "Parser error: %s";
    case GPE_SCANNER:
      return "Scanner error: %s";

    case GPE_IDLOCS_P16E:
      return "IDLOCS directive use solely to the pic18 family.";
    case GPE_NOF:
      return "The destination of the storage is not selected, use W or F.";
    case GPE_ACC_NOSEL:
      return "The access of RAM is not selected, use A or B:";
    case GPE_TOO_LONG:
      return "The string (\"%s\") too length (%zu bytes). It cannot be more than %zu bytes.";
    case GPE_IS_ACCRAM:
      return "This register is located on the Access RAM:";
    case GPE_NO_ACCRAM:
      return "This register is not located on the Access RAM:";
    case GPE_BANK:
      return "Register in operand not located in RAM Bank %i. Ensure that Bank bits are correct:";
    case GPE_INVALID_RAM:
      return "Invalid RAM location specified.";
    case GPE_INVALID_ROM:
      return "Invalid ROM location specified.";

    default:
      return "UNKNOWN ERROR";
  }
}

/*------------------------------------------------------------------------------------------------*/

static const char *
_get_warning(int Code)
{
  switch(Code) {
    case GPW_NOT_DEFINED:
      return "Symbol not previously defined: \"%s\"";
    case GPW_RANGE:
      return "Argument out of range. Least significant bits used.";
    case GPW_OP_COLUMN_ONE:
      return "Found opcode in column 1: \"%s\"";
    case GPW_DIR_COLUMN_ONE:
      return "Found directive in column 1: \"%s\"";
    case GPW_MACRO_COLUMN_ONE:
      return "Found call to macro in column 1: \"%s\"";
    case GPW_LABEL_COLUMN:
      return "Found label after column 1.";
    case GPW_MISSING_QUOTE:
      return "Missing quote.";
    case GPW_EXTRANEOUS:
      return "Extraneous arguments on the line.";
    case GPW_EXPECTED:
      return "Expected.";
    case GPW_CMDLINE_PROC:
      return "Processor superseded by command line.";
    case GPW_CMDLINE_RADIX:
      return "Radix superseded by command line.";
    case GPW_CMDLINE_HEXFMT:
      return "Hex file format specified on command line.";
    case GPW_RADIX:
      return "Expected dec, oct, hex. Will use hex.";
    case GPW_INVALID_RAM:
      return "Invalid RAM location specified.";
    case GPW_EXCEED_ROM:
      return "Address exceeds maximum range for this processor.";
    case GPW_DISABLE_ERROR:
      return "Error messages cannot be disabled.";
    case GPW_REDEFINING_PROC:
      return "Redefining processor.";
    case GPW_NOT_RECOMMENDED:
      return "Use of this instruction is not recommended:";
    case GPW_WORD_ALIGNED:
      return "Destination address must be word aligned.";
    case GPW_INVALID_ROM:
      return "Invalid ROM location specified.";

    /* gputils special warnings */
    case GPW_BANK_PAGE_SEL_AFTER_SKIP:
      return "%s after skip instruction. I this really what you intended?";
    case GPW_UNDEF_PROC:
      return "Processor type is undefined.";
    case GPW_IS_ACCRAM:
      return "This register is located on the Access RAM:";
    case GPW_NO_ACCRAM:
      return "This register is not located on the Access RAM:";
    case GPW_NOF:
      return "The destination of the storage is not selected, use W or F.";
    case GPW_BANK:
      return "Register in operand not located in RAM Bank %i. Ensure that Bank bits are correct:";
    case GPW_USER:
      return "WARNING: (%s)";

    default:
      return "UNKNOWN WARNING";
  }
}

/*------------------------------------------------------------------------------------------------*/

static const char *
_get_message(int Code)
{
  switch(Code) {
    case GPM_USER:
      return "MESSAGE: \"%s\"";
    case GPM_BANK:
      return "Register in operand not located in RAM Bank %i. Ensure that Bank bits are correct:";
    case GPM_RANGE:
      return "Program word too large. Truncated to core size: 0x%04X";
    case GPM_IDLOC:
      return "An ID Locations value too large. Last four hex digits used: 0x%X ==> 0x%04X";
    case GPM_NOF:
      return "Using default destination of 1 (file).";
    case GPM_PAGE:
      return "Crossing page boundary -- ensure page bits are set.";
    case GPM_PAGEBITS:
      return "Setting page bits.";
    case GPM_SUPVAL:
      return "Warning level superseded by command line value.";
    case GPM_SUPLIN:
      return "Macro expansion superseded by command line value.";
    case GPM_SUPRAM:
      return "Superseding current maximum RAM and RAM map.";
    case GPM_EXT_PAGE:
      return "Page or Bank selection not needed for this device. No code generated.";
    case GPM_CBLOCK:
      return "CBLOCK constants will start with a value of 0.";
    case GPM_W_MODIFIED:
      return "W Register modified.";
    case GPM_SPECIAL_MNEMONIC:
      return "Special Instruction Mnemonic used.";

    case GPM_ACC_DEF:
      return "Using default access of 0 (Access Bank):";
    case GPM_NOB:
      return "RAM Bank undefined in this chunk of code. Ensure that bank bits are correct. Assuming bank %u from now on.";
    case GPM_EXT_BANK:
      return "Bank selection not needed for this device. No code generated.";
    case GPM_EXT_PAGE2:
      return "Page selection not needed for this device. No code generated.";

    case GPM_UNKNOWN:
    default:
      return "UNKNOWN MESSAGE";
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_verr(err_type_t Err_type, int Code, const char *Message, va_list Ap)
{
  va_list                 ap0;
  const char             *type;
  const char             *gap;
  const source_context_t *src = state.src;

  if ((src != NULL) && state.macro_dereference) {
    while ((src != NULL) && (src->type == SRC_MACRO)) {
      src = src->prev;
    }
  }

  switch (Err_type) {
    case MSG_TYPE_ERROR:
      type = "Error";
      gap  = "  ";
      break;

    case MSG_TYPE_WARNING:
      type = "Warning";
      gap  = "";
      break;

    case MSG_TYPE_MESSAGE:
    default:
      type = "Message";
      gap  = "";
      break;
  }

  /* standard output */
  if (!state.quiet) {
    if (src != NULL) {
      printf("%s:%d:%s[%03d] %s", src->name, src->line_number, type, Code, gap);
    }
    else {
      printf("%s[%03d] %s", type, Code, gap);
    }

    va_copy(ap0, Ap);
    vprintf(Message, ap0);
    va_end(ap0);
    putchar('\n');
  }

  /* error file */
  if (state.err.enabled) {
    if (src != NULL) {
      fprintf(state.err.f, "%s[%03d] %s%s %d : ", type, Code, gap, src->name, src->line_number);
    }
    else {
      fprintf(state.err.f, "%s[%03d] %s", type, Code, gap);
    }

    vfprintf(state.err.f, Message, Ap);
    putc('\n', state.err.f);
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_err(err_type_t Err_type, int Code, const char *Message)
{
  const char             *type;
  const char             *gap;
  const source_context_t *src = state.src;

  if ((src != NULL) && state.macro_dereference) {
    while ((src != NULL) && (src->type == SRC_MACRO)) {
      src = src->prev;
    }
  }

  switch (Err_type) {
    case MSG_TYPE_ERROR:
      type = "Error";
      gap  = "  ";
      break;

    case MSG_TYPE_WARNING:
      type = "Warning";
      gap  = "";
      break;

    case MSG_TYPE_MESSAGE:
    default:
      type = "Message";
      gap  = "";
      break;
  }

  /* standard output */
  if (!state.quiet) {
    if (src != NULL) {
      printf("%s:%d:%s[%03d] %s%s\n", src->name, src->line_number, type, Code, gap, Message);
    }
    else {
      printf("%s[%03d] %s%s\n", type, Code, gap, Message);
    }
  }

  /* error file */
  if (state.err.enabled) {
    if (src != NULL) {
      fprintf(state.err.f, "%s[%03d] %s%s %d : %s\n", type, Code, gap, src->name, src->line_number, Message);
    }
    else {
      fprintf(state.err.f, "%s[%03d] %s%s\n", type, Code, gap, Message);
    }
  }
}

/*------------------------------------------------------------------------------------------------*/

void
gpmsg_init(void)
{
  if (state.err_file != OUT_NAMED) {
    snprintf(state.err_file_name, sizeof(state.err_file_name), "%s.err", state.base_file_name);
  }

  if (state.err_file == OUT_SUPPRESS) {
    state.err.enabled = false;
    unlink(state.err_file_name);
  }
   else {
    state.err.f = fopen(state.err_file_name, "wt");

    if (state.err.f == NULL) {
      perror(state.err_file_name);
      exit(1);
    }

    state.err.enabled = true;
  }

  message_codes_list      = NULL;
  message_codes_list_last = NULL;
}

/*------------------------------------------------------------------------------------------------*/

void
gpmsg_close(void)
{
  if (state.err.enabled) {
    fclose(state.err.f);
  }

  _free_message_codes();
  message_codes_list      = NULL;
  message_codes_list_last = NULL;
}

/*------------------------------------------------------------------------------------------------*/

void
gpmsg_add_code(int Code)
{
  message_code_t *new;

  if ((Code <= -(GMSG_ERR_LEVEL0_MIN)) && (Code >= -(GMSG_ERR_LEVEL0_MAX))) {
    gpmsg_vwarning(GPW_DISABLE_ERROR, NULL);
    return;
  }
  else if ((Code <= -(GMSG_ERR_LEVEL1_MIN)) && (Code >= -(GMSG_ERR_LEVEL1_MAX))) {
    gpmsg_vwarning(GPW_DISABLE_ERROR, NULL);
    return;
  }

  new = (message_code_t *)GP_Calloc(1, sizeof(message_code_t));
  new->type   = ERR_CODE_NUM;
  new->value0 = Code;

  if (message_codes_list == NULL) {
    /* The list is empty. */
    message_codes_list = new;
  }
  else {
    /* Append the new node to the end of the list. */
    message_codes_list_last->next = new;
  }

  message_codes_list_last = new;
}

/*------------------------------------------------------------------------------------------------*/

/* This can not be used in "mpasm compatible" mode! */

void
gpmsg_add_code_range(int Code0, int Code1)
{
  message_code_t *new;
  int             t;

  if (Code0 > Code1) {
    t     = Code0;
    Code0 = Code1;
    Code1 = t;
  }

  if (gp_num_range_is_overlapped(Code0, Code1, -(GMSG_ERR_LEVEL0_MIN), -(GMSG_ERR_LEVEL0_MAX))) {
    gpmsg_vwarning(GPW_DISABLE_ERROR, NULL);
    return;
  }
  else if (gp_num_range_is_overlapped(Code0, Code1, -(GMSG_ERR_LEVEL1_MIN), -(GMSG_ERR_LEVEL1_MAX))) {
    gpmsg_vwarning(GPW_DISABLE_ERROR, NULL);
    return;
  }

  new = (message_code_t *)GP_Calloc(1, sizeof(message_code_t));
  new->type   = ERR_CODE_RANGE;
  new->value0 = Code0;
  new->value1 = Code1;

  if (message_codes_list == NULL) {
    /* The list is empty. */
    message_codes_list = new;
  }
  else {
    /* Append the new node to the end of the list. */
    message_codes_list_last->next = new;
  }

  message_codes_list_last = new;
}

/*------------------------------------------------------------------------------------------------*/

void
gpmsg_error(int Code, const char *Message)
{
  if (state.pass != 2) {
    return;
  }

  if (Message == NULL) {
    Message = _get_error(Code);
  }

  /* standard output */
  _err(MSG_TYPE_ERROR, Code, Message);

  /* list file output */
  lst_line("Error[%03d]  : %s", Code, Message);

  state.num.errors++;
}

/*------------------------------------------------------------------------------------------------*/

void
gpmsg_verror(int Code, const char *Message, ...)
{
  va_list     ap;
  const char *msg;
  char        buf[BUFSIZ];

  if (state.pass != 2) {
    return;
  }

  msg = _get_error(Code);

  if ((Message != NULL) && (Message[0] != '\0')) {
    snprintf(buf, sizeof(buf), "%s %s", msg, Message);
    msg = buf;
  }

  /* standard output */
  va_start(ap, Message);
  _verr(MSG_TYPE_ERROR, Code, msg, ap);
  va_end(ap);

  /* list file output */
  va_start(ap, Message);
  lst_err_line("Error", Code, msg, ap);
  va_end(ap);

  state.num.errors++;
}

/*------------------------------------------------------------------------------------------------*/

void
gpmsg_warning(int Code, const char *Message)
{
  if (state.pass != 2) {
    return;
  }

  if ((state.error_level <= 1) && _check_code(Code)) {
    if (Message == NULL) {
      Message = _get_warning(Code);
    }

    /* standard output */
    _err(MSG_TYPE_WARNING, Code, Message);

    /* list file output */
    lst_line("Warning[%03d]: %s", Code, Message);

    state.num.warnings++;
  }
  else {
    state.num.warnings_suppressed++;
  }
}

/*------------------------------------------------------------------------------------------------*/

void
gpmsg_vwarning(int Code, const char *Message, ...)
{
  va_list     ap;
  const char *msg;
  char        buf[BUFSIZ];

  if (state.pass != 2) {
    return;
  }

  if ((state.error_level <= 1) && _check_code(Code)) {
    msg = _get_warning(Code);

    if ((Message != NULL) && (Message[0] != '\0')) {
      snprintf(buf, sizeof(buf), "%s %s", msg, Message);
      msg = buf;
    }

    /* standard output */
    va_start(ap, Message);
    _verr(MSG_TYPE_WARNING, Code, msg, ap);
    va_end(ap);

    va_start(ap, Message);
    lst_err_line("Warning", Code, msg, ap);
    va_end(ap);

    state.num.warnings++;
  }
  else {
    state.num.warnings_suppressed++;
  }
}

/*------------------------------------------------------------------------------------------------*/

void
gpmsg_message(int Code, const char *Message)
{
  if (state.pass != 2) {
    return;
  }

  if ((state.error_level == 0) && _check_code(Code)) {
    if (Message == NULL) {
      Message = _get_message(Code);
    }

    /* standard output */
    _err(MSG_TYPE_MESSAGE, Code, Message);

    /* list file output */
    lst_line("Message[%03d]: %s", Code, Message);

    state.num.messages++;
  }
  else {
    state.num.messages_suppressed++;
  }
}

/*------------------------------------------------------------------------------------------------*/

void
gpmsg_vmessage(int Code, const char *Message, ...)
{
  va_list     ap;
  const char *msg;
  char        buf[BUFSIZ];

  if (state.pass != 2) {
    return;
  }

  if ((state.error_level == 0) && _check_code(Code)) {
    msg = _get_message(Code);

    if ((Message != NULL) && (Message[0] != '\0')) {
      snprintf(buf, sizeof(buf), "%s %s", msg, Message);
      msg = buf;
    }

    /* standard output */
    va_start(ap, Message);
    _verr(MSG_TYPE_MESSAGE, Code, msg, ap);
    va_end(ap);

    /* list file output */
    va_start(ap, Message);
    lst_err_line("Message", Code, msg, ap);
    va_end(ap);

    state.num.messages++;
  }
  else {
    state.num.messages_suppressed++;
  }
}
