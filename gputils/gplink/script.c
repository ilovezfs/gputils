/* Linker script processing
   Copyright (C) 2001 Craig Franklin

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
#include "gplink.h"
#include "gpsymbol.h"
#include "scan.h"
#include "script.h"
#include "gpcofflink.h"

#define HEAD(L) (L)->value.list.head
#define TAIL(L) (L)->value.list.tail

struct command {
  char *name;
  long int address;
};

typedef int lkrfunc(char *name, struct pnode *parms);

/* FIXME: rename this function. */
void gplkrscr_error(char *messg, char *detail)
{
  state.num.errors++;
  if (state.quiet != 1) {
    if (detail == NULL) {
      printf("%s:%d:Error %s\n", 
             state.src->name, 
             state.src->line_number, 
             messg); 
    } else {
      printf("%s:%d:Error %s (%s)\n", 
             state.src->name, 
             state.src->line_number, 
             messg,
             detail);      
    }
  }
  

  return;
}

static int enforce_simple(struct pnode *p)
{
  if (p->tag == symbol) {
    return 1;
  } else {
    gplkrscr_error("illegal argument", NULL);
    return 0;
  }
}

static int evaluate(struct pnode *p)
{

  switch (p->tag) {
  case constant:
    return p->value.constant;
  default:
    gplkrscr_error("illegal argument", NULL);
    return 0;
  }

}

static int do_files(char *name, struct pnode *parms)
{
  struct pnode *p;
  
  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);
    if (enforce_simple(p)) {
      gplink_open_coff(p->value.symbol);
    }
  }

  return 0;
}

static int do_include(char *name, struct pnode *parms)
{
  struct pnode *p;

  /* FIXME: make sure only one argument is passed */
  p = HEAD(parms);
  if (enforce_simple(p)) {
    open_src(p->value.symbol, 1);
  }

  return 0;
}

/* FIXME: gplink doesn't distinguish between lib pathes and lkr pathes.  There
   is one pathes list.  This shouldn't cause any problems. */

static int do_path(char *name, struct pnode *parms)
{
  struct pnode *p;
  
  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);
    if (enforce_simple(p)) {
      gplink_add_path(p->value.symbol);
    }
  }

  return 0;
}

/* FIXME:  These functions were written to allow the user the greatest 
   flexibility.  The arguments can appear in any order.  In hind sight
   this may not have been necessary and resulted in overly complicated
   code. */

/* logical section definition */
static int do_logsec(char *name, struct pnode *parms)
{
  struct pnode *p;
  int found_secname   = 0;
  int found_ram       = 0;
  int found_rom       = 0;
  char *logical_section_name = NULL;
  char *section_name = NULL;
  struct linker_section *section = NULL;
  struct symbol *sym;
  
  /* read the options */
  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);
    if ((p->tag == binop) &&
	(p->value.binop.op == '=')) {
      if (enforce_simple(p->value.binop.p0)) {
	char *lhs;

	lhs = p->value.binop.p0->value.symbol;
	if (strcasecmp(lhs, "name") == 0) {
          if (enforce_simple(p->value.binop.p1)) {
            found_secname = 1;
            logical_section_name = p->value.binop.p1->value.symbol; 
          }
        } else if (strcasecmp(lhs, "ram") == 0) {
          if (enforce_simple(p->value.binop.p1)) {
            found_ram = 1;
            section_name = strdup(p->value.binop.p1->value.symbol);
	  }
        } else if (strcasecmp(lhs, "rom") == 0) {
          if (enforce_simple(p->value.binop.p1)) {
            found_rom = 1;
            section_name = strdup(p->value.binop.p1->value.symbol);
	  }
        } else {
          gplkrscr_error("illegal argument", lhs);        
        }       
      }
    } else {
      if (enforce_simple(p)) {
        gplkrscr_error("illegal argument", p->value.symbol);      
      }
    }
  }

  /* process the options */ 
  if (found_secname == 0) {
    gplkrscr_error("missing argument", "name");   
  } else if ((found_rom == 1) && (found_ram == 1)) {
    gplkrscr_error("too many arguments", "ram or rom");
  } else if ((found_rom == 0) && (found_ram == 0)) {
    gplkrscr_error("missing argument", "ram or rom");
  } else {
    sym = get_symbol(state.section.definition, section_name);      
    if (sym == NULL) {
      gplkrscr_error("undefined section", section_name);
    } else {
      section = get_symbol_annotation(sym);
      assert(section != NULL);
      if ((found_ram == 1) && (section->type == codepage)) {
        gplkrscr_error("invalid argument", "ram");
      } else if ((found_rom == 1) && (section->type != codepage)) {
        gplkrscr_error("invalid argument", "rom");      
      } else {
        sym = add_symbol(state.section.logical, logical_section_name);
        annotate_symbol(sym, section_name);	       
      }
    }
  }

  return 0;
}

/* general section definition processing */
static int do_secdef(char *name, struct pnode *parms)
{
  struct pnode *p;
  int found_secname   = 0;
  int found_start     = 0;
  int found_end       = 0;
  int found_fill      = 0;
  int found_protected = 0;
  char *section_name = NULL;
  int  start = 0;
  int  end = 0;
  int  fill = 0;
  struct symbol *sym;
  struct linker_section *section_def;

  /* read the options */
  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);
    if ((p->tag == binop) &&
	(p->value.binop.op == '=')) {
      if (enforce_simple(p->value.binop.p0)) {
	char *lhs;

	lhs = p->value.binop.p0->value.symbol;
	if (strcasecmp(lhs, "name") == 0) {
          if (enforce_simple(p->value.binop.p1)) {
            found_secname = 1;
            section_name = p->value.binop.p1->value.symbol; 
          }
        } else if (strcasecmp(lhs, "start") == 0) {
          found_start = 1;
          start = evaluate(p->value.binop.p1);
        } else if (strcasecmp(lhs, "end") == 0) {
          found_end = 1;
          end = evaluate(p->value.binop.p1);             
        } else if (strcasecmp(lhs, "fill") == 0) {
          found_fill = 1;
          fill = evaluate(p->value.binop.p1);
        } else {
          gplkrscr_error("illegal argument", lhs);        
        }       
      
      }
    } else {
      if (enforce_simple(p)) {
	if (strcasecmp(p->value.symbol, "protected") == 0) {
          found_protected = 1; 	   
	} else {
          gplkrscr_error("illegal argument", p->value.symbol);
        }
      }    
    }
  }

  /* process the options */
  if (found_secname == 0) {
    gplkrscr_error("missing argument", "name");
  } else if (found_start == 0) {
    gplkrscr_error("missing argument", "start");
  } else if (found_end == 0) {
    gplkrscr_error("missing argument", "end");
  } else {

    if ((strcasecmp(name, "codepage") != 0) && (found_fill == 1))
      gplkrscr_error("illegal argument", "fill");

    sym = get_symbol(state.section.definition, section_name);      
    if (sym != NULL) {
      gplkrscr_error("duplicate section definition", section_name);
    } else {
      sym = add_symbol(state.section.definition, section_name);
      section_def = (struct linker_section *)malloc(sizeof(*section_def));
      annotate_symbol(sym, section_def);

      section_def->start = start;
      section_def->end = end;
      section_def->fill = fill;
      section_def->use_fill = found_fill;
      section_def->protected = found_protected;
      section_def->next_address = 0;

      if (strcasecmp(name, "accessbank") == 0) {
        /* FIXME: accessbank is only valid for 18cxx devices */
        section_def->type = accessbank;
      } else if (strcasecmp(name, "codepage") == 0) {
        section_def->type = codepage;
      } else if (strcasecmp(name, "databank") == 0) {
        section_def->type = databank;
      } else if (strcasecmp(name, "sharebank") == 0) {
        section_def->type = sharebank;
      } else {
        gplkrscr_error("invalid definition type", name);      
      }
    }
  }

  return 0;
}

static int do_stack(char *name, struct pnode *parms)
{
  struct pnode *p;

  int found_size  = 0;
  int found_ram   = 0;
  char *ramname;
  int  size;

  /* FIXME: simplify this.  There are only two arguments */

  /* read the options */
  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);
    if ((p->tag == binop) &&
	(p->value.binop.op == '=')) {
      if (enforce_simple(p->value.binop.p0)) {
	char *lhs;

	lhs = p->value.binop.p0->value.symbol;
        if (strcasecmp(lhs, "size") == 0) {
          found_size = 1;
          size = evaluate(p->value.binop.p1);
        } else if (strcasecmp(lhs, "ram") == 0) {
          if (enforce_simple(p->value.binop.p1)) {
            found_ram = 1;
            ramname = p->value.binop.p1->value.symbol;   
          }
        } else {
          gplkrscr_error("illegal argument", lhs);        
        }       
      }
    } else {
      if (enforce_simple(p)) {
        gplkrscr_error("illegal argument", p->value.symbol);
      }
    }
  }

  /* process the options */
  if (found_size == 0) {
    gplkrscr_error("missing argument", "size");
  } else {

  }

  return 0;
}


static struct command commands[] = {
  { "accessbank", (long int)do_secdef     },
  { "codepage",   (long int)do_secdef     },
  { "databank",   (long int)do_secdef     },
  { "files",      (long int)do_files      },
  { "include",    (long int)do_include    },
  { "libpath",    (long int)do_path       },
  { "lkrpath",    (long int)do_path       },
  { "section",    (long int)do_logsec     },
  { "sharebank",  (long int)do_secdef     },
  { "stack",	  (long int)do_stack      }
};

#define NUM_COMMAND  (sizeof(commands) / sizeof(commands[0]))

int execute_command(char *name, struct pnode *parms)
{
  int value = 0;
  int i = 0;
  int found_command = 0;

  while (i < NUM_COMMAND) {
    if (strcasecmp(name, commands[i].name) == 0) {
      found_command = 1;
      break;
    }
    i++;
  }

  if (found_command == 0) {
    gplkrscr_error("invalid script command", name);
  } else {
    value = (*(lkrfunc*)commands[i].address)(name, parms);
  }

  return value;
}


