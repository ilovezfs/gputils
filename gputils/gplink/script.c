/* Linker script processing
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Craig Franklin

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
#include "gplink.h"
#include "scan.h"
#include "script.h"

#define HEAD(L) (L)->value.list.head
#define TAIL(L) (L)->value.list.tail

struct command {
  char *name;
  long int address;
};

typedef int lkrfunc(char *name, struct pnode *parms);

void
script_error(char *messg, char *detail)
{

  gp_num_errors++;
  if (!gp_quiet) {
    if (state.src == NULL) {
      printf("%s\n", messg); 
    } else if (detail == NULL) {
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
    script_error("illegal argument", NULL);
    return 0;
  }
}

static int evaluate(struct pnode *p)
{

  switch (p->tag) {
  case constant:
    return p->value.constant;
  default:
    script_error("illegal argument", NULL);
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

int
add_path(struct pnode *parms)
{
  struct pnode *p;
  
  for (; parms != NULL; parms = TAIL(parms)) {
    p = HEAD(parms);
    if (enforce_simple(p)) {
      /* gplink doesn't distinguish between lib pathes and lkr pathes.  There
         is one pathes list.  This shouldn't cause any problems. */
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
  gp_boolean found_secname   = false;
  gp_boolean found_ram       = false;
  gp_boolean found_rom       = false;
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
            found_secname = true;
            logical_section_name = p->value.binop.p1->value.symbol; 
          }
        } else if (strcasecmp(lhs, "ram") == 0) {
          if (enforce_simple(p->value.binop.p1)) {
            found_ram = true;
            section_name = strdup(p->value.binop.p1->value.symbol);
	  }
        } else if (strcasecmp(lhs, "rom") == 0) {
          if (enforce_simple(p->value.binop.p1)) {
            found_rom = true;
            section_name = strdup(p->value.binop.p1->value.symbol);
	  }
        } else {
          script_error("illegal argument", lhs);        
        }       
      }
    } else {
      if (enforce_simple(p)) {
        script_error("illegal argument", p->value.symbol);      
      }
    }
  }

  /* process the options */ 
  if (!found_secname) {
    script_error("missing argument", "name");   
  } else if (found_rom && found_ram) {
    script_error("too many arguments", "ram or rom");
  } else if ((!found_rom) && (!found_ram)) {
    script_error("missing argument", "ram or rom");
  } else {
    sym = get_symbol(state.section.definition, section_name);      
    if (sym == NULL) {
      script_error("undefined section", section_name);
    } else {
      section = get_symbol_annotation(sym);
      assert(section != NULL);
      if ((found_ram == 1) && (section->type == codepage)) {
        script_error("invalid argument", "ram");
      } else if ((found_rom == 1) && (section->type != codepage)) {
        script_error("invalid argument", "rom");      
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
  gp_boolean found_secname   = false;
  gp_boolean found_start     = false;
  gp_boolean found_end       = false;
  gp_boolean found_fill      = false;
  gp_boolean found_protected = false;
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
            found_secname = true;
            section_name = p->value.binop.p1->value.symbol; 
          }
        } else if (strcasecmp(lhs, "start") == 0) {
          found_start = true;
          start = evaluate(p->value.binop.p1);
        } else if (strcasecmp(lhs, "end") == 0) {
          found_end = true;
          end = evaluate(p->value.binop.p1);             
        } else if (strcasecmp(lhs, "fill") == 0) {
          found_fill = true;
          fill = evaluate(p->value.binop.p1);
        } else {
          script_error("illegal argument", lhs);        
        }       
      
      }
    } else {
      if (enforce_simple(p)) {
	if (strcasecmp(p->value.symbol, "protected") == 0) {
          found_protected = true; 	   
	} else {
          script_error("illegal argument", p->value.symbol);
        }
      }    
    }
  }

  /* process the options */
  if (!found_secname) {
    script_error("missing argument", "name");
  } else if (!found_start) {
    script_error("missing argument", "start");
  } else if (!found_end) {
    script_error("missing argument", "end");
  } else {
    sym = get_symbol(state.section.definition, section_name);      
    if (sym == NULL) {
      sym = add_symbol(state.section.definition, section_name);
      section_def = (struct linker_section *)malloc(sizeof(*section_def));
      annotate_symbol(sym, section_def);

      if (strcasecmp(name, "accessbank") == 0) {
        if (state.class != PROC_CLASS_PIC16E) {
          script_error("accessbank only valid with 18xx devices", name);      
        }        
        section_def->type = accessbank;
      } else if (strcasecmp(name, "codepage") == 0) {
        section_def->type = codepage;
	start = gp_processor_org_to_byte(state.class, start);
	end = gp_processor_org_to_byte(state.class, end + 1) - 1;
      } else if (strcasecmp(name, "databank") == 0) {
        section_def->type = databank;
      } else if (strcasecmp(name, "sharebank") == 0) {
        section_def->type = sharebank;
      } else {
        script_error("invalid definition type", name);      
      }

      section_def->start = start;
      section_def->end = end;
      section_def->protected = found_protected;

      if (section_def->type == codepage) {
        if ((!state.fill_enable) || found_protected) {
          section_def->fill = fill;
          section_def->use_fill = found_fill;
        } else {
          section_def->fill = state.fill_value;
          section_def->use_fill = true;
        }
      } else if (found_fill == 1) {
        script_error("illegal argument", "fill");        
      }
    } else if (strcasecmp(name, "sharebank") != 0) {
      script_error("duplicate section definition", section_name);
    }

  }

  return 0;
}

static int do_stack(char *name, struct pnode *parms)
{
  struct pnode *p;

  gp_boolean found_size  = false;
  gp_boolean found_ram   = false;
  char *ram_name = NULL;
  struct symbol *sym;

  if (state.has_stack) {
    script_error("multiple stack definitions", NULL);
    return 0;
  } else {
    state.has_stack = true;
  }

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
          found_size = true;
          state.stack_size = evaluate(p->value.binop.p1);
        } else if (strcasecmp(lhs, "ram") == 0) {
          if (enforce_simple(p->value.binop.p1)) {
            found_ram = true;
            ram_name = p->value.binop.p1->value.symbol;   
          }
        } else {
          script_error("illegal argument", lhs);        
        }       
      }
    } else {
      if (enforce_simple(p)) {
        script_error("illegal argument", p->value.symbol);
      }
    }
  }

  /* process the options */
  if (!found_size) {
    script_error("missing argument", "size");
  } else if (ram_name != NULL) {
    sym = add_symbol(state.section.logical, strdup(".stack"));
    annotate_symbol(sym, ram_name);	       
  }

  return 0;
}


static struct command commands[] = {
  { "accessbank", (long int)do_secdef     },
  { "codepage",   (long int)do_secdef     },
  { "databank",   (long int)do_secdef     },
  { "files",      (long int)do_files      },
  { "include",    (long int)do_include    },
  { "section",    (long int)do_logsec     },
  { "sharebank",  (long int)do_secdef     },
  { "stack",	  (long int)do_stack      }
};

#define NUM_COMMAND  (sizeof(commands) / sizeof(commands[0]))

int execute_command(char *name, struct pnode *parms)
{
  int value = 0;
  int i = 0;
  gp_boolean found_command = false;

  while (i < NUM_COMMAND) {
    if (strcasecmp(name, commands[i].name) == 0) {
      found_command = true;
      break;
    }
    i++;
  }

  if (!found_command) {
    script_error("invalid script command", name);
  } else {
    value = (*(lkrfunc*)commands[i].address)(name, parms);
  }

  return value;
}


