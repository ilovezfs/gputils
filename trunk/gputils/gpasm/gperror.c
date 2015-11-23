/* Error handling for gpasm
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Craig Franklin
   Copyright (C) 2012 Borut Razem

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
#include "lst.h"

struct error_list {
  int value;
  struct error_list *next;
};

static struct error_list *errorcodes_list = NULL;

void
gperror_init(void) {
  if (state.errfile != OUT_NAMED) {
    snprintf(state.errfilename, sizeof(state.errfilename),
       "%s.err", state.basefilename);
  }

  if (state.errfile == OUT_SUPPRESS) {
    state.err.enabled = false;
    unlink(state.errfilename);
  } else {
    state.err.f = fopen(state.errfilename, "wt");

    if (state.err.f == NULL) {
      perror(state.errfilename);
      exit(1);
    }

    state.err.enabled = true;
  }
}

void
add_code(int code)
{
  struct error_list *new;
  struct error_list *list;

  if ((code <= -100) && (code >= -199)) {
    gpvwarning(GPW_DISABLE_ERROR, NULL);
  } else {
    new = (struct error_list *)GP_Malloc(sizeof(*new));
    new->value = code;
    new->next  = NULL;

    if (errorcodes_list != NULL) {
      /* the list has been started, scan the list for the end */
      list = errorcodes_list;
      while (list->next != NULL) {
        list = list->next;
      }
      list->next = new;  /* append the new value to the end of list */
    } else {
      errorcodes_list = new;  /* new list */
    }
  }
}

static gp_boolean
check_code(int code)
{
  struct error_list *p;
  gp_boolean print = true;

  p = errorcodes_list;

  while (p != NULL) {
    if (p->value == code) {
      print = true;
    } else if (p->value == -(code)) {
      print = false;
    }

    p = p->next;
  }

  return print;
}

typedef enum {
  ET_ERROR,
  ET_WARNING,
  ET_MESSAGE
} err_type_t;

static void
verr(err_type_t err_type, unsigned int code, const char *message, va_list ap)
{
  va_list ap0;

  /* standard output */
  if (!state.quiet) {
    const char *type;
    const char *gap;

    switch (err_type) {
      case ET_ERROR:
        type = "Error";
        gap  = "  ";
        break;

      case ET_WARNING:
        type = "Warning";
        gap  = "";
        break;

      case ET_MESSAGE:
      default:
        type = "Message";
        gap  = "";
        break;
    }

    if (state.src != NULL) {
      printf("%s:%d:%s[%03u] %s", state.src->name, state.src->line_number,
             type, code, gap);
    }
    else {
      printf("%s[%03u] %s", type, code, gap);
    }

    va_copy(ap0, ap);
    vprintf(message, ap0);
    va_end(ap0);
    putchar('\n');

    if (state.err.enabled) {
      if (state.src != NULL) {
        fprintf(state.err.f, "%s[%03u] %s%s %d : ", type, code, gap,
                state.src->name, state.src->line_number);
      }
      else {
        fprintf(state.err.f, "%s[%03u] %s", type, code, gap);
      }

      vfprintf(state.err.f, message, ap);
      putc('\n', state.err.f);
    }
  }
}

static void
err(err_type_t err_type, unsigned int code, const char *message)
{
  /* standard output */
  if (!state.quiet) {
    const char *type;
    const char *gap;

    switch (err_type) {
      case ET_ERROR:
        type = "Error";
        gap  = "  ";
        break;

      case ET_WARNING:
        type = "Warning";
        gap  = "";
        break;

      case ET_MESSAGE:
      default:
        type = "Message";
        gap  = "";
        break;
    }

    if (state.src != NULL) {
      printf("%s:%d:%s[%03u] %s%s\n", state.src->name, state.src->line_number,
             type, code, gap, message);
    }
    else {
      printf("%s[%03u] %s%s\n", type, code, gap, message);
    }

    if (state.err.enabled) {
      if (state.src != NULL) {
        fprintf(state.err.f, "%s[%03u] %s%s %d : %s\n", type, code, gap,
                state.src->name, state.src->line_number, message);
      }
      else {
        fprintf(state.err.f, "%s[%03u] %s%s\n", type, code, gap, message);
      }
    }
  }
}

static char *
gp_geterror(unsigned int code)
{
  switch(code) {
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
    return "The destination of the storage is not selected (W or F).";
  case GPE_NOA:
    return "The access of RAM is not selected (A or B).";

  default:
    return "UNKNOWN";
  }
}

void
gperror(unsigned int code, char *message)
{
  if (state.pass == 2) {
    if (message == NULL) {
      message = gp_geterror(code);
    }

    /* standard output */
    err(ET_ERROR, code, message);

    /* list file output */
    lst_line("Error[%03d]  : %s", code, message);

    state.num.errors++;
  }
}

void
gpverror(unsigned int code, const char *message, ...)
{
  if (state.pass == 2) {
    va_list(ap);
    const char *msg = gp_geterror(code);

    if (message != NULL && *message != '\0') {
      char buf[BUFSIZ];

      snprintf(buf, sizeof(buf), "%s %s", msg, message);
      msg = buf;
    }

    /* standard output */
    va_start(ap, message);
    verr(ET_ERROR, code, msg, ap);
    va_end(ap);

    /* list file output */
    va_start(ap, message);
    lst_err_line("Error", code, msg, ap);
    va_end(ap);

    state.num.errors++;
  }
}

static char *
gp_getwarning(unsigned int code)
{
  switch(code) {
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

  default:
    return "UNKNOWN";
  }
}

void
gpwarning(unsigned int code, char *message)
{
  if (state.pass == 2) {
    if ((state.error_level <= 1) && check_code(code)) {
      if (message == NULL) {
        message = gp_getwarning(code);
      }

      /* standard output */
      err(ET_WARNING, code, message);

      /* list file output */
      lst_line("Warning[%03d]: %s", code, message);

      state.num.warnings++;
    } else {
      state.num.warnings_suppressed++;
    }
  }
}

void
gpvwarning(unsigned int code, const char *message, ...)
{
  if (state.pass == 2) {
    if ((state.error_level <= 1) && check_code(code)) {
      va_list(ap);
      const char *msg = gp_getwarning(code);

      if (message != NULL && *message != '\0') {
        char buf[BUFSIZ];

        snprintf(buf, sizeof(buf), "%s %s", msg, message);
        msg = buf;
      }

      /* standard output */
      va_start(ap, message);
      verr(ET_WARNING, code, msg, ap);
      va_end(ap);

      va_start(ap, message);
      lst_err_line("Warning", code, msg, ap);
      va_end(ap);

      state.num.warnings++;
    } else {
      state.num.warnings_suppressed++;
    }
  }
}

static char *
gp_getmessage(unsigned int code)
{
  switch(code) {
  case GPM_USER:
    return "MESSAGE: \"%s\"";
  case GPM_BANK:
    return "Register in operand not in bank 0. Ensure that bank bits are correct.";
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
  case GPM_EXTPAGE:
    return "Page or Bank selection not needed for this device. No code generated.";
  case GPM_CBLOCK:
    return "CBLOCK constants will start with a value of 0.";
  case GPM_W_MODIFIED:
    return "W Register modified.";
  case GPM_SPECIAL_MNEMONIC:
    return "Special Instruction Mnemonic used.";
  case GPM_NOA:
    return "Using default access of 0 (Access Bank).";
  case GPM_UNKNOWN:
  default:
    return "UNKNOWN MESSAGE";
  }
}

void
gpmessage(unsigned int code, char *message)
{
  if (state.pass == 2) {
    if ((state.error_level == 0) && check_code(code)) {
      if (message == NULL) {
        message = gp_getmessage(code);
      }

      /* standard output */
      err(ET_MESSAGE, code, message);

      /* list file output */
      lst_line("Message[%03d]: %s", code, message);

      state.num.messages++;
    } else {
      state.num.messages_suppressed++;
    }
  }
}

void
gpvmessage(unsigned int code, const char *message, ...)
{
  if (state.pass == 2) {
    if ((state.error_level == 0) && check_code(code)) {
      va_list(ap);
      const char *msg = gp_getmessage(code);

      if (message != NULL && *message != '\0') {
        char buf[BUFSIZ];

        snprintf(buf, sizeof(buf), "%s %s", msg, message);
        msg = buf;
      }
      /* standard output */
      va_start(ap, message);
      verr(ET_MESSAGE, code, msg, ap);
      va_end(ap);

      /* list file output */
      va_start(ap, message);
      lst_err_line("Message", code, msg, ap);
      va_end(ap);

      state.num.messages++;
    } else {
      state.num.messages_suppressed++;
    }
  }
}

void
gperror_close(void)
{
  if (state.err.enabled) {
    fclose(state.err.f);
  }
}
