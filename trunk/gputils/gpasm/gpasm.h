/* Common definitions for gpasm
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003
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

#ifndef __GPASM_H__
#define __GPASM_H__

#define GPASM_VERSION_STRING ("gpasm-" VERSION " alpha")

/* This symbol will get placed into the symbol table for the 16bit cores
 * and thus allow compile-time selection of the proper macro set */
#define __16bit_core_	"__16bit_core_"

#define STRCMP(s1, s2)	(state.case_insensitive ? \
			 strcasecmp((s1), (s2)) : \
			 strcmp((s1), (s2)))
			 
typedef int gpasmVal; 		/* The type that internal arithmetic uses */
enum gpasmValTypes {gvt_constant, gvt_cblock, gvt_org, gvt_address, gvt_extern,
                    gvt_global, gvt_static};
enum state_types { _exitmacro, _include, _macro, _nochange, _section,
                   _substitution, _while };

enum outfile { normal, suppress, named };
enum file_types { ft_src, ft_hex, ft_lst, ft_cod, ft_other }; /* allowed file types */
enum gpasm_modes { absolute, relocatable };

extern struct gpasm_state {
  enum gpasm_modes mode;
  int radix;
  enum formats hex_format;
  int case_insensitive;
  int quiet;
  int error_level;		/* 0, 1, 2 */
  struct {			/* Command line override flags */
    int radix;			/* When 1, the value is specified by the */
    int hex_format;		/* command line */
    int error_level;
    int macro_expand;		
    int processor;
    int lst_force;
  } cmd_line;
  int pass;			/* 1 or 2 */
  unsigned int org;		/* Current code-generation point */
  int dos_newlines;		/* Use DOS newlines in hex file */
  int memory_dump;		/* Dump instruction memory to standard output */
  unsigned int maxram;		/* Highest legal memory location */
  enum outfile
    codfile,			/* Symbol output file control */
    hexfile,			/* Hex output file control */
    lstfile,			/* List output file control */
    objfile;			/* Relocatable object file control */
  struct {			/* Totals for errors, warnings, messages */
    int errors;
    int warnings;
    int messages;
    int warnings_suppressed;
    int messages_suppressed;
  } num;
  enum pic_processor processor;
  struct px *processor_info;    /* Processor identifiers (e.g. name) */
  int processor_chosen;		/* Nonzero after processor-specific init */
  struct {			/* Processor data */
    enum proc_class class;      /* Processor class */
    int core_size;		/* Processor core size  */
    int config_address;		/* configuration address */
    int id_location;		/* location for idlocs for 12 and 14 bit proc */
  } device;
  unsigned int c_memory_base;	/* Base address of configuration memory */
  char badram[MAX_RAM];		/* nonzero indicates illegal memory */
  struct symbol_table
  *stBuiltin,			/* Built-ins: instructions, pseudo-ops */
    *stDirective,               /* bottom half of Builtin with directives */
    *stGlobal,			/* Global symbols */
    *stTop,			/* Top of locals stack (stGlobal is base) */
    *stDefines,			/* Preprocessor #defines */
    *stTopDefines,		/* Macro #defines (stDefines is base) */
    *stMacros;			/* Macros */
  MemBlock *i_memory;		/* Instruction memory linked list */
  char *srcfilename,		/* Source (.asm) file name */
    basefilename[BUFSIZ],	/* basename for generating hex,list,symbol filenames */
    codfilename[BUFSIZ],	/* Symbol (.cod) file name */
    hexfilename[BUFSIZ],	/* Hex (.hex) file name */
    lstfilename[BUFSIZ],	/* List (.lst) file name */
    objfilename[BUFSIZ];	/* Object (.o) file name */
  struct {			/* List file state: */
    FILE *f;			/*   List file output */
    unsigned int
      lineofpage,		/*   What line are we at within the page */
      page,			/*   What page are we at */
      linesperpage,		/*   Lines per page */
      line_number,              /*   What line are we at within the file */
      memorymap,		/*   Memory Map dump enabled */
      symboltable;		/*   Symbol table dump enabled */
    struct {
      unsigned int was_org;	/*   value of state.org at start of line */
		    		/*   What kind of line was it? */
      enum { none,		/*     Nothing - blank line */
	     org,		/*     ORG pseudo-op */
	     dir,		/*     Directive, non-code generating */
	     idlocs,		/*     ID locations for 12 and 14 bit cores */
	     insn,		/*     Some other instruction or pseudo */
	     equ,		/*     An equate */
	     res,               /*     reserve memory */
	     sec,		/*     new coff section */
	     set,		/*     A SET or '=' */
	     config }           /*     A __config line */
        linetype;
    } line;
    char startdate[80];		/*   When assembly started */
    int enabled;		/*   nonzero if listing is enabled */
    int expand;			/*   1 if the macro listings are expanded */
    int force;                  /*   1 if ignoring nolist directives */
    int config_address;		/*   list config address for 16 bit devices */
    char title_name[80];	/*   given in TITLE directive */
    char subtitle_name[80];	/*   given in SUBTITLE directive */
    int tabstop;		/*   tab-stop distance */
    struct file_context *fc;    /*   Position in the file context stack */
  } lst;
  struct {			/* Symbol file state: */
    FILE *f;			/*   Symbol file output */
    int enabled;		/*   nonzero if symbol file is enabled */
    int emitting;               /*   flag indicating when an opcode is emitted */
  } cod;
  struct {			/* Object file state: */
    gp_object_type *object;	/*   Object file */
    gp_section_type *section;	/*   Current section */
    int enabled;		/*   nonzero if object file is enabled */
    char new_sec_name[80];	/*   new section name */
    int new_sec_addr;		/*   new section name */
    int new_sec_flags;		/*   new section name */
    int symbol_num;		/*   Current symbol number */
    int flags;			/*   Current section flags */
  } obj;
  struct source_context *src;	/* Top of the stack of source files */
  struct file_context *files;   /* Top of the stack of all files */
  struct amode *astack;		/* Stack of amodes (macros, etc) */
  gpasmVal cblock;		/* cblock constant */
  struct macro_head *mac_head;  /* Starting a macro... */
  struct macro_body **mac_prev; /* Stitching ptr */
  struct macro_body *mac_body;	/* While we're building a macro */
  struct macro_head *while_head;/* WHILEs work a lot like macros... */
  enum state_types next_state; 
  union {
    char *file;
    struct macro_head *macro;
  } next_buffer;
} state;

struct variable {
  int value;
  int coff_num;
  enum gpasmValTypes type;
  enum gpasmValTypes previous_type; /* can change from static to global */
};

#define MAX_INCLUDE_PATHS 100
extern char *include_paths[];
extern int n_include_paths;

/************************************************************************/

/* Parse node: created by the parser, interpreted by the 'backend' */

struct pnode {
  enum pnode_tag { constant, symbol, string, list, binop, unop } tag;
  union {
    int constant;
    char *symbol;
    struct {
      struct pnode *head, *tail;
    } list;
    struct {
      int op;
      struct pnode *p0, *p1;
    } binop;
    struct {
      int op;
      struct pnode *p0;
    } unop;
    char *string;
  } value;
};

/************************************************************************/

/* file_context: a structure to keep track of all files that have been
		 opened.  Used to create the list of project files
		 that can be found in the .cod file.  */

struct file_context {
  char *name;                     /* file name */
  unsigned int id;                /* Unique identifier */
  enum file_types ft;             /* allowed file types */
  struct file_context *prev;      /* Previous in list pointer */
  struct file_context *next;      /* Next in list pointer */
};

struct source_context {
  char *name;
  enum src_types { file, macro, substitution } type;
  FILE *f; 
  union {
    FILE *f;                  
    struct macro_body *m;  
  } lst; 
  struct yy_buffer_state *yybuf;
  unsigned int line_number;
  gp_symbol_type *file_symbol;
  struct file_context *fc;        /* Position in the file context stack */
  struct amode *astack;		  /* Stack of amodes when a macro was called */
  struct source_context *prev;
};

enum globalLife { TEMPORARY, PERMANENT };

void yyerror(char *s);
int stringtolong(char *string, int radix);
int gpasm_magic(char *);
char *convert_escape_chars(char *ps, int *value);
gpasmVal do_or_append_insn(char *op, struct pnode *parms);
void set_global(char *name,
		gpasmVal value,
		enum globalLife lifetime,
		enum gpasmValTypes type);
void select_errorlevel(int level);
void select_expand(char *expand);
void select_hexformat(char *format_name);
void select_radix(char *name);
struct file_context *add_file(unsigned int type, char *name);
void free_files(void);
void macro_append(void);
void hex_init(void);

/************************************************************************/

struct macro_head {
  int pass;			/* Pass in which macro was defined: 1 or 2 */
  struct pnode *parms;
  struct macro_body *body;
  int defined;			/* 1 macro has been defined so calls are valid */ 
  char *src_name;
  unsigned int line_number;
  gp_symbol_type *file_symbol;
};

struct macro_body {
  char *label;			/* Label for the line */
  char *op;			/* Operation (or NULL) */
  struct pnode *parms;		/* Parameters for op (or NULL) */
  char *src_line;		/* Original source line - for listing */
  struct macro_body *next;	/* Next line in listing */
};

struct amode {
  enum { in_then, in_else } mode;
  int enabled;	/* Are we currently enabled? */
  int prev_enabled;
  struct amode *prev;
};

#endif
