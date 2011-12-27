/* Common definitions for gpasm
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

#ifndef __GPASM_H__
#define __GPASM_H__

#define GPASM_VERSION_STRING ("gpasm-" VERSION " (" __DATE__ ")")

/* This symbol will get placed into the symbol table for the 16bit cores
 * and thus allow compile-time selection of the proper macro set */
#define __16bit_core_   "__16bit_core_"

#define STRCMP(s1, s2)  (state.case_insensitive ? \
                         strcasecmp((s1), (s2)) : \
                         strcmp((s1), (s2)))

#define MAX_PATHS 100

enum gpasmValTypes {
  gvt_constant,
  gvt_cblock,
  gvt_org,
  gvt_address,
  gvt_extern,
  gvt_global,
  gvt_static,
  gvt_debug,
  gvt_absolute
};

enum state_types {
  state_nochange,
  state_exitmacro,
  state_include,
  state_macro,
  state_section,
  state_substitution,
  state_while
};

enum outfile {
  normal,
  suppress,
  named
};

enum file_types {
  ft_src,
  ft_other
};

enum gpasm_modes {
  absolute,
  relocatable
};

struct range_pair {
  long start, end;
  struct range_pair *next;
};

extern struct gpasm_state {
  enum gpasm_modes mode;
  gp_boolean extended_pic16e;
  int radix;
  enum formats hex_format;
  gp_boolean case_insensitive;
  gp_boolean quiet;
  gp_boolean use_absolute_path;
  int error_level;              /* 0, 1, 2 */
  gp_boolean debug_info;        /* use debug directives for coff outputs */
  int path_num;                 /* number of paths in the list */
  char *paths[MAX_PATHS];       /* the list of include paths */
  struct {                      /* Command line override flags */
    gp_boolean radix;           /* value is specified by the command line */
    gp_boolean hex_format;
    gp_boolean error_level;
    gp_boolean macro_expand;
    gp_boolean processor;
    gp_boolean lst_force;
  } cmd_line;
  int pass;                     /* 1 or 2 */
  unsigned int org;             /* Current code-generation point */
  gp_boolean dos_newlines;      /* Use DOS newlines in hex file */
  gp_boolean memory_dump;       /* Dump instruction memory to standard output */
  gp_boolean found_config;      /* config directive in source code */
  gp_boolean found_devid;       /* config directive in source code */
  gp_boolean found_idlocs;      /* idlocs directive in source code */
  gp_boolean found_end;         /* end directive in source code */
  unsigned int maxram;          /* Highest legal memory location */
  long maxrom;                  /* Highest legal program memory location */
  struct range_pair *badrom;
  enum outfile
    codfile,                    /* Symbol output file control */
    depfile,                    /* Dependency output file control */
    hexfile,                    /* Hex output file control */
    lstfile,                    /* List output file control */
    objfile;                    /* Relocatable object file control */
  struct {                      /* Totals for errors, warnings, messages */
    int errors;
    int warnings;
    int messages;
    int warnings_suppressed;
    int messages_suppressed;
  } num;
  pic_processor_t processor;
  int processor_chosen;         /* Nonzero after processor-specific init */
  struct {                      /* Processor data */
    proc_class_t class;         /* Processor class */
    int id_location;            /* address of last __idlocs */
    int bsr_boundary;           /* 18xx bsr boundary location */
  } device;
  unsigned int c_memory_base;   /* Base address of configuration memory */
  char badram[MAX_RAM];         /* nonzero indicates illegal memory */
  struct symbol_table
  *stBuiltin,                   /* Built-ins: instructions, pseudo-ops */
    *stDirective,               /* bottom half of Builtin with directives */
    *stGlobal,                  /* Global symbols */
    *stTop,                     /* Top of locals stack (stGlobal is base) */
    *stDefines,                 /* Preprocessor #defines */
    *stTopDefines,              /* Macro #defines (stDefines is base) */
    *stMacros;                  /* Macros */
  MemBlock *i_memory;           /* Instruction memory linked list */
  MemBlock *c_memory;           /* Configuration memory linked list (shadow) */
  char *srcfilename,            /* Source (.asm) file name */
    basefilename[BUFSIZ],       /* basename for generating hex,list,symbol filenames */
    codfilename[BUFSIZ],        /* Symbol (.cod) file name */
    depfilename[BUFSIZ],        /* Dependency (.d) file name */
    lstfilename[BUFSIZ],        /* List (.lst) file name */
    objfilename[BUFSIZ];        /* Object (.o) file name */
  struct {                      /* Symbol file state: */
    FILE *f;                    /*   Symbol file output */
    gp_boolean enabled;         /*   true if symbol file is enabled */
    int emitting;               /*   flag indicating when an opcode is emitted */
  } cod;
  struct {                      /* Dep file state: */
    FILE *f;                    /*   Dep file output */
    gp_boolean enabled;         /*   true if dep file is enabled */
  } dep;
  struct {                      /* List file state: */
    FILE *f;                    /*   List file output */
    unsigned int
      lineofpage,               /*   What line are we at within the page */
      page,                     /*   What page are we at */
      linesperpage,             /*   Lines per page */
      line_number,              /*   What line are we at within the file */
      memorymap,                /*   Memory Map dump enabled */
      symboltable;              /*   Symbol table dump enabled */
      enum { in_none,
             in_mem,
             in_symtab }
        lst_state;              /*   Listing state */
    struct {
      unsigned int was_org;     /*   value of state.org at start of line */
                                /*   What kind of line was it? */
      enum { none,              /*     Nothing - blank line */
             org,               /*     ORG pseudo-op */
             dir,               /*     Directive, non-code generating */
             idlocs,            /*     ID locations for 12 and 14 bit cores */
             insn,              /*     Some other instruction or pseudo */
             equ,               /*     An equate */
             data,              /*     Data */
             res,               /*     reserve memory */
             sec,               /*     new coff section */
             set,               /*     A SET or '=' */
             config }           /*     A __config line */
        linetype;
    } line;
    char startdate[80];         /*   When assembly started */
    gp_boolean enabled;         /*   listing is enabled */
    gp_boolean expand;          /*   macro listings are expanded */
    gp_boolean force;           /*   ignoring nolist directives */
    int config_address;         /*   list config address for 16 bit devices */
    char title_name[80];        /*   given in TITLE directive */
    char subtitle_name[80];     /*   given in SUBTITLE directive */
    int tabstop;                /*   tab-stop distance */
  } lst;
  struct {                      /* Object file state: */
    gp_object_type *object;     /*   Object file */
    gp_section_type *section;   /*   Current section */
    int section_num;            /*   Current section number */
    gp_boolean enabled;         /*   true if object file is enabled */
    char new_sec_name[80];      /*   new section name */
    int new_sec_addr;           /*   new section name */
    int new_sec_flags;          /*   new section name */
    int symbol_num;             /*   Current symbol number */
    int flags;                  /*   Current section flags */
    gp_symbol_type *debug_file; /*   Debug information for high level langs */
    unsigned int debug_line;
    int newcoff;
  } obj;
  struct source_context *src;   /* Top of the stack of source files */
  struct file_context *files;   /* Top of the stack of all files */
  struct amode *astack;         /* Stack of amodes (macros, etc) */
  gpasmVal cblock;              /* cblock constant */
  int cblock_defined;
  struct macro_head *mac_head;  /* Starting a macro... */
  struct macro_body **mac_prev; /* Stitching ptr */
  struct macro_body *mac_body;  /* While we're building a macro */
  struct macro_head *while_head;/* WHILEs work a lot like macros... */
  enum state_types next_state;
  union {
    char *file;
    struct macro_head *macro;
  } next_buffer;
} state;

enum globalLife { TEMPORARY, PERMANENT };

struct variable {
  int value;
  int coff_num;
  int coff_section_num;
  enum gpasmValTypes type;
  enum gpasmValTypes previous_type; /* can change from static to global */
  enum globalLife lifetime;
};

/************************************************************************/

/* Parse node: created by the parser, interpreted by the 'backend' */

struct pnode {
  enum pnode_tag { constant, offset, symbol, string, list, binop, unop } tag;
  union {
    int constant;
    struct pnode *offset;
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

enum src_types {
  src_unknown,
  src_file,
  src_macro,
  src_substitution,
  src_while
};

struct source_context {
  char *name;
  enum src_types type;
  FILE *f;
  struct macro_head *h;
  union {
    FILE *f;
    struct macro_body *m;
  } lst;
  struct yy_buffer_state *yybuf;
  unsigned int line_number;
  unsigned int loop_number;       /* Loop number for while loops */
  gp_symbol_type *file_symbol;
  struct file_context *fc;        /* Position in the file context stack */
  struct amode *astack;           /* Stack of amodes when a macro was called */
  gp_boolean last_char_is_nl;     /* If the last read character is a newline */
  struct source_context *prev;
};

void yyerror(char *s);
int stringtolong(const char *string, int radix);
int gpasm_magic(const char *);
const char *convert_escape_chars(const char *ps, int *value);
char *convert_escaped_char(char *str, char c);
void coerce_str1(struct pnode *exp);
gpasmVal do_or_append_insn(char *op, struct pnode *parms);
void set_global(const char *name,
                gpasmVal value,
                enum globalLife lifetime,
                enum gpasmValTypes type);
void purge_temp_symbols(struct symbol_table *table);
void select_errorlevel(int level);
void select_expand(const char *expand);
void select_hexformat(const char *format_name);
void select_radix(const char *name);
struct file_context *add_file(unsigned int type, const char *name);
void free_files(void);
void macro_append(void);
void hex_init(void);
void add_path(const char *path);

/************************************************************************/

struct macro_head {
  int pass;                     /* Pass in which macro was defined: 1 or 2 */
  struct pnode *parms;
  struct macro_body *body;
  int defined;                  /* 1 macro has been defined so calls are valid */
  char *src_name;
  unsigned int line_number;
  gp_symbol_type *file_symbol;
};

struct macro_body {
  char *src_line;               /* Original source line - for listing */
  struct macro_body *next;      /* Next line in listing */
};

struct amode {
  enum { in_then, in_else } mode;
  int enabled;  /* Are we currently enabled? */
  int prev_enabled;
  struct amode *prev;
};

#endif
