/* Error handling for gpasm
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
#include "gperror.h"
#include "lst.h"

struct error_list {
  int value;
  struct error_list *next;
};

static struct error_list *errorcodes_list = NULL;

void add_code(int code)
{
  struct error_list *new;
  struct error_list *list;

  if ((code <= -100) && (code >= -199)) {
    gpwarning(GPW_DISABLE_ERROR, NULL);
  } else {
    new = (struct error_list *)malloc(sizeof(*new));
    new->value = code;
    new->next  = NULL;

    if (errorcodes_list) {
      /* the list has been started, scan the list for the end */ 
      list = errorcodes_list;
      while(list->next) {
        list = list->next;
      }
      list->next = new;  /* append the new value to the end of list */
    } else {
      errorcodes_list = new;  /* new list */
    }
  }
}

static int check_code(int code)
{
  struct error_list *p;
  int print = 1;

  p = errorcodes_list;

  while(p) {

    if (p->value == code){
        print = 1;
    } else if (p->value == -(code)) {
	print = 0;
    }

    p = p->next;
  }

  return print;
}

char *gp_geterror(unsigned int code)
{

  switch(code) {
  case GPE_OPENPAR:
    return "Unmatched (";
  case GPE_CLOSEPAR:
    return "Unmatched )";
  case GPE_BADCHAR:
    return "Illegal character.";
  case GPE_NOSYM:
    return "Symbol not previously defined.";
  case GPE_DIVBY0:
    return "Divide by zero.";
  case GPE_DUPLAB:
    return "Duplicate label or redefining symbol that cannot be redefined.";
  case GPE_DIFFLAB:
    return "Address label duplicated or different in second pass.";
  case GPE_ADDROVF:
    return "Address wrapped around 0. ";
  case GPE_ADDROVR:
    return "Overwriting previous address contents.";
  case GPE_BAD_CALL_ADDR:
    return "Call or jump not allowed at this address (must be in low half of page)";
  case GPE_ILLEGAL_LABEL:
    return "Illegal label.";
  case GPE_ILLEGAL_DIR:
    return "Illegal directive (Not Valid for this processor).";
  case GPE_ILLEGAL_ARGU:
    return "Illegal argument.";
  case GPE_ILLEGAL_COND:
    return "Illegal condition.";
  case GPE_RANGE:
    return "Argument out of range.";
  case GPE_TOO_MANY_ARGU:
    return "Too many arguments.";
  case GPE_MISSING_ARGU:
    return "Missing argument(s).";
  case GPE_EXPECTED:
    return "Expected";
  case GPE_EXTRA_PROC:
    return "Processor type previously defined.";
  case GPE_UNDEF_PROC:
    return "Processor type is undefined.";
  case GPE_UNKNOWN_PROC:
    return "Unknown processor.";
  case GPE_IHEX:
    return "Hex file format INHX32 required.";
  case GPE_NO_MACRO_NAME:
    return "Macro name missing.";
  case GPE_DUPLICATE_MACRO:
    return "Duplicate macro name.";
  case GPE_BAD_WHILE_LOOP:
    return "WHILE must terminate within 256 iterations.";
  case GPE_UNMATCHED_ENDM:
    return "Unmatched ENDM.";
  case GPE_FILL_ODD:
    return "Cannot use FILL Directive with odd number of bytes.";
  case GPE_UNKNOWN:
  default:
    return "UNKNOWN";
  }
}

void gperror(unsigned int code,
	     char *message)
{
  char full_message[BUFSIZ];

  if (state.pass == 2) {
    if(message == NULL)
      message = gp_geterror(code);

#ifndef GP_USER_ERROR
    /* standard output */
    if (!state.quiet) {
      if (state.src)
        sprintf(full_message,
	        "%s:%d:Error [%03d] %s",
	        state.src->name,
	        state.src->line_number,
	        code,
	        message);
      else
        sprintf(full_message,
	        "Error [%03d] %s",
	        code,
	        message);

      printf("%s\n", full_message);
    }
#else
    user_error(code, message);
#endif

    /* list file output */
    sprintf(full_message,
	    "Error [%03d] : %s",
	    code,
	    message);

    lst_line(full_message);

    state.num.errors++;
  }
}

char *gp_getwarning(unsigned int code)
{
  switch(code) {
  case GPW_NOT_DEFINED:
    return "Symbol not previously defined.";
  case GPW_RANGE:
    return "Argument out of range. Least significant bits used.";
  case GPW_OP_COLUMN_ONE:
    return "Found opcode in column 1.";
  case GPW_DIR_COLUMN_ONE:
    return "Found directive in column 1.";
  case GPW_MACRO_COLUMN_ONE:
    return "Found call to macro in column 1.";
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
  case GPW_DISABLE_ERROR:
    return "Error messages cannot be disabled.";
  case GPW_REDEFINING_PROC:
    return "Redefining processor.";
  case GPW_NOT_RECOMMENDED:
    return "Use of this instruction is not recommended.";
  case GPW_UNKNOWN:
  default:
    return "UNKNOWN";
  }
}

void gpwarning(unsigned int code,
	       char *message)
{
  char full_message[BUFSIZ];

  if (state.pass ==2) {

    if ((state.error_level <= 1) && check_code(code)) {
      if(message == NULL)
        message = gp_getwarning(code);

#ifndef GP_USER_WARNING
      /* standard output */
      if (!state.quiet) {
        if (state.src)
          sprintf(full_message,
	          "%s:%d:Warning [%03d] %s",
	          state.src->name,
	          state.src->line_number,
	          code,
	          message);
        else
          sprintf(full_message,
	          "Warning [%03d] %s",
	          code,
	          message);

        printf("%s\n", full_message);
      } 
#else
      user_warning(code, message);
#endif

      /* list file output */
      sprintf(full_message,
	      "Warning [%03d] : %s",
	      code,
	      message);

      lst_line(full_message);

      state.num.warnings++;
    } else {
      state.num.warnings_suppressed++;
    }
  }
}

char *gp_getmessage(unsigned int code)
{
  switch(code) {
  case GPM_USER:
    return "MESSAGE:";
  case GPM_BANK:
    return "Register in operand not in bank 0. Ensure bank bits are correct.";
  case GPM_RANGE:
    return "Program word too large. Truncated to core size.";
  case GPM_IDLOC:
    return "ID Locations value too large. Last four hex digits used.";
  case GPM_NOF:
    return "Using default destination of 1 (file).";
  case GPM_PAGE:
    return "Crossing page boundary -- ensure page bits are set.";
  case GPM_PAGEBITS:
    return "Setting page bits.";
  case GPM_SUPVAL:
    return "Warning level superceded by command line value.";
  case GPM_SUPLIN:
    return "Macro expansion superceded by command line value.";
  case GPM_SUPRAM:
    return "Superceding current maximum RAM and RAM map.";
  case GPM_EXTPAGE:
    return "Page bits are not needed.";
  case GPM_CBLOCK:
    return "CBLOCK will begin at address 0.";
  case GPM_SPECIAL_MNEMONIC:
    return "Special Instruction Mnemonic used."; 
  case GPM_UNKNOWN:
  default:
    return "UNKNOWN MESSAGE";
  }
}

void gpmessage(unsigned int code,
	       char *message)
{
  char full_message[BUFSIZ];

  if (state.pass==2) {

    if ((state.error_level == 0) && check_code(code)){
      if(message == NULL)
        message = gp_getmessage(code);

#ifndef GP_USER_MESSAGE
      /* standard output */
      if (!state.quiet) {
        if (state.src)
          sprintf(full_message,
	          "%s:%d:Message [%03d] %s",
	          state.src->name,
	          state.src->line_number,
	          code,
	          message);
        else
          sprintf(full_message,
	          "Message [%03d] %s",
	          code,
	          message);

        printf("%s\n", full_message);
      }
#else
      user_message(code, message);
#endif

      /* list file output */
      sprintf(full_message,
	      "Message [%03d] : %s",
	      code,
	      message);

      lst_line(full_message);

      state.num.messages++;
    } else {
      state.num.messages_suppressed++;
    }
  }
}
