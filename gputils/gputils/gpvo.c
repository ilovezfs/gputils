/* GNU PIC view object
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
#include "gpvo.h"

struct gpvo_state state;

void print_header(gp_object_type *object)
{
#define NELEM(x)  (sizeof(x) / sizeof(*(x)))

  static struct magic_s {
    unsigned short magic_num;
    const char *magic_name;
  } magic[] = {
    { 0x1234, "MICROCHIP_MAGIC_v1" },
    { 0x1240, "MICROCHIP_MAGIC_v2" }
  };

  char *time = ctime(&object->time);
  const char *processor_name = gp_processor_name(object->processor, 2);
  int i;

  /* strip the newline from time */
  time[strlen(time)-1] = '\0';

  printf("COFF File and Optional Headers\n");

  for (i = 0; i < NELEM(magic); ++i) {
    if (magic[i].magic_num == object->version) {
      break;
    }
  }
  printf("COFF version         %#x: %s\n", object->version,
         (i < NELEM(magic)) ? magic[i].magic_name : "unknown");
  printf("Processor Type       %s\n",   processor_name);
  printf("Time Stamp           %s\n",   time);
  printf("Number of Sections   %li\n",  object->num_sections);
  printf("Number of Symbols    %li\n",  object->num_symbols);
  printf("Characteristics      %#x\n",  object->flags);

  if (object->flags & F_RELFLG) {
    printf("  Relocation info has been stripped.\n");
  }
  if (object->flags & F_EXEC) {
    printf("  File is executable.\n");
  }
  if (object->flags & F_LNNO) {
    printf("  Line numbers have been stripped.\n");
  }
  if (object->flags & F_ABSOLUTE) {
    printf("  The MPASM assembler object file is from absolute assembly code.\n");
  }
  if (object->flags & L_SYMS) {
    printf("  Local symbols have been stripped.\n");
  }
  if (object->flags & F_EXTENDED18) {
    printf("  The COFF file produced utilizing the Extended mode.\n");
  }
  if (object->flags & F_GENERIC) {
    printf("  Processor independent file for a core.\n");
  }

  printf("\n");
}

void print_reloc_list(proc_class_t class, gp_reloc_type *relocation)
{
  int word_addr = class->org_to_byte_shift;

  printf("Relocations Table\n");
  printf("Address    Offset     Type   Symbol\n");

  while (relocation != NULL) {
    printf("%#-10lx %#-10x %#-6x %-s\n",
           relocation->address >> word_addr,
           relocation->offset,
           relocation->type,
           relocation->symbol->name);

    relocation = relocation->next;
  }

  printf("\n");
}

void print_linenum_list(proc_class_t class, gp_linenum_type *linenumber)
{
  const char *filename;

  printf("Line Number Table\n");
  printf("Line     Address  Symbol\n");

  while (linenumber != NULL) {
    if (state.suppress_names) {
      filename = linenumber->symbol->name;
    } else {
      filename = linenumber->symbol->aux_list->_aux_symbol._aux_file.filename;
    }

    printf("%-8i %#-8x %s\n",
           linenumber->line_number,
           gp_processor_byte_to_org(class, linenumber->address),
           filename);

    linenumber = linenumber->next;
  }

  printf("\n");
}

void print_data(proc_class_t class, gp_section_type *section)
{
  char buffer[BUFSIZ];
  int address;
  int num_words = 1;

  address = section->address;

  buffer[0] = '\0';

  printf("Data\n");
  while (1) {
    if ((section->flags & STYP_TEXT) && class->find_insn != NULL) {
      unsigned short memory;
      if (!class->i_memory_get(section->data, address, &memory))
        break;
      num_words = gp_disassemble(section->data,
                                 address,
                                 class,
                                 buffer,
                                 sizeof(buffer));
      printf("%06x:  %04x  %s\n", gp_processor_byte_to_org(class, address), memory, buffer);
      if (num_words != 1) {
        class->i_memory_get(section->data, address + 2, &memory);
        printf("%06x:  %04x\n", gp_processor_byte_to_org(class, address + 2), memory);
      }
      address += 2 * num_words;
    } else if (section->flags & STYP_DATA_ROM || class == PROC_CLASS_EEPROM16) {
      unsigned short memory;
      if (!class->i_memory_get(section->data, address, &memory))
        break;
      printf("%06x:  %04x\n", gp_processor_byte_to_org(class, address), memory);
      address += 2;
    } else {
      /* STYP_DATA or STYP_ACTREC, or EEPROM8 */
      unsigned char b;
      if (!b_memory_get(section->data, address, &b))
        break;
      printf("%06x:  %02x\n", address, b);
      ++address;
    }
  }
  printf("\n");
}

void print_sec_header(proc_class_t class, gp_section_type *section)
{
  int org_to_byte_shift;

  if (section->flags & (STYP_TEXT|STYP_DATA_ROM))
    org_to_byte_shift = class->org_to_byte_shift;
  else
    org_to_byte_shift = 0;

  printf("Section Header\n");
  printf("Name                    %s\n",   section->name);
  printf("Physical address        %#x\n",  gp_byte_to_org(org_to_byte_shift, section->address));
  printf("Virtual address         %#x\n",  gp_byte_to_org(org_to_byte_shift, section->virtual_address));
  printf("Size of Section         %li\n",  section->size);
  printf("Number of Relocations   %i\n",   section->num_reloc);
  printf("Number of Line Numbers  %i\n",   section->num_lineno);
  printf("Flags                   %#lx\n", section->flags);
  if (section->flags & STYP_TEXT) {
    printf("  Executable code.\n");
  }
  if (section->flags & STYP_DATA) {
    printf("  Initialized data.\n");
  }
  if (section->flags & STYP_BSS) {
    printf("  Uninitialized data.\n");
  }
  if (section->flags & STYP_DATA_ROM) {
    printf("  Initialized data for ROM.\n");
  }
  if (section->flags & STYP_ABS) {
    printf("  Absolute.\n");
  }
  if (section->flags & STYP_SHARED) {
    printf("  Shared across banks.\n");
  }
  if (section->flags & STYP_OVERLAY) {
    printf("  Overlaid with other sections from different objects modules.\n");
  }
  if (section->flags & STYP_ACCESS) {
    printf("  Available using access bit.\n");
  }
  if (section->flags & STYP_ACTREC) {
    printf("  Contains the activation record for a function.\n");
  }

  printf("\n");
}

void print_sec_list(gp_object_type *object)
{
  gp_section_type *section = object->sections;

  while (section != NULL) {
    print_sec_header(object->class, section);

    if (section->size && section->data_ptr) {
      print_data(object->class, section);
    }
    if (section->num_reloc) {
      print_reloc_list(object->class, section->relocations);
    }
    if (section->num_lineno) {
      print_linenum_list(object->class, section->line_numbers);
    }

    section = section->next;
  }
}

void coff_type(int type, char *buffer, size_t sizeof_buffer)
{
  switch (type) {
  case T_NULL:
    /* null */
    snprintf(buffer, sizeof_buffer, "NULL");
    break;
  case T_VOID:
    /* void */
    snprintf(buffer, sizeof_buffer, "void");
    break;
  case T_CHAR:
    /* character */
    snprintf(buffer, sizeof_buffer, "char");
    break;
  case T_SHORT:
    /* short integer */
    snprintf(buffer, sizeof_buffer, "short");
    break;
  case T_INT:
    /* integer */
    snprintf(buffer, sizeof_buffer, "int");
    break;
  case T_LONG:
    /* long integer */
    snprintf(buffer, sizeof_buffer, "long int");
    break;
  case T_FLOAT:
    /* floating point */
    snprintf(buffer, sizeof_buffer, "float");
    break;
  case T_DOUBLE:
    /* double length floating point */
    snprintf(buffer, sizeof_buffer, "double");
    break;
  case T_STRUCT:
    /* structure */
    snprintf(buffer, sizeof_buffer, "struct");
    break;
  case T_UNION:
    /* union */
    snprintf(buffer, sizeof_buffer, "union");
    break;
  case T_ENUM:
    /* enumeration */
    snprintf(buffer, sizeof_buffer, "enum");
    break;
  case T_MOE:
    /* member of enumeration */
    snprintf(buffer, sizeof_buffer, "enum");
    break;
  case T_UCHAR:
    /* unsigned character */
    snprintf(buffer, sizeof_buffer, "unsigned char");
    break;
  case T_USHORT:
    /* unsigned short */
    snprintf(buffer, sizeof_buffer, "unsigned short");
    break;
  case T_UINT:
    /* unsigned integer */
    snprintf(buffer, sizeof_buffer, "unsigned int");
    break;
  case T_ULONG:
    /* unsigned long */
    snprintf(buffer, sizeof_buffer, "unsigned long");
    break;
  case T_LNGDBL:
    /* long double floating point */
    snprintf(buffer, sizeof_buffer, "long double");
    break;
  case T_SLONG:
    /* short long */
    snprintf(buffer, sizeof_buffer, "short long");
    break;
  case T_USLONG:
    /* unsigned short long */
    snprintf(buffer, sizeof_buffer, "unsigned short long");
    break;
  default:
    snprintf(buffer, sizeof_buffer, "unknown(%d)", type);
    break;
  }
}

static const char *format_sym_type(int type, char *buffer, size_t sizeof_buffer)
{
  static const char * const type_str[] = {
    "T_NULL",
    "T_VOID",
    "T_CHAR",
    "T_SHORT",
    "T_INT",
    "T_LONG",
    "T_FLOAT",
    "T_DOUBLE",
    "T_STRUCT",
    "T_UNION",
    "T_ENUM",
    "T_MOE",
    "T_UCHAR",
    "T_USHORT",
    "T_UINT",
    "T_ULONG",
    "T_LNGDBL",
    "T_SLONG",
    "T_USLONG"
  };
  if (type < sizeof(type_str)/sizeof(type_str[0]))
    return type_str[type];
  snprintf(buffer, sizeof_buffer, "%d", type);
  return buffer;
}

static const char *format_sym_derived_type(int type, char *buffer, size_t sizeof_buffer)
{
  static const char * const type_str[] = {
    "DT_NON",
    "DT_PTR",
    "DT_FCN",
    "DT_ARY",
    "DT_ROMPTR",
    "DT_FARROMPTR",
  };
  if (type < sizeof(type_str)/sizeof(type_str[0]))
    return type_str[type];
  snprintf(buffer, sizeof_buffer, "%d", type);
  return buffer;
}

static const char *format_sym_class(int cls, char *buffer, size_t sizeof_buffer)
{
  switch(cls) {
  case C_EFCN: return "C_EFCN";
  case C_NULL: return "C_NULL";
  case C_AUTO: return "C_AUTO";
  case C_EXT: return "C_EXT";
  case C_STAT: return "C_STAT";
  case C_REG: return "C_REG";
  case C_EXTDEF: return "C_EXTDEF";
  case C_LABEL: return "C_LABEL";
  case C_ULABEL: return "C_ULABEL";
  case C_MOS: return "C_MOS";
  case C_ARG: return "C_ARG";
  case C_STRTAG: return "C_STRTAG";
  case C_MOU: return "C_MOU";
  case C_UNTAG: return "C_UNTAG";
  case C_TPDEF: return "C_TPDEF";
  case C_USTATIC: return "C_USTATIC";
  case C_ENTAG: return "C_ENTAG";
  case C_MOE: return "C_MOE";
  case C_REGPARM: return "C_REGPARM";
  case C_FIELD: return "C_FIELD";
  case C_AUTOARG: return "C_AUTOARG";
  case C_LASTENT: return "C_LASTENT";
  case C_BLOCK: return "C_BLOCK";
  case C_FCN: return "C_FCN";
  case C_EOS: return "C_EOS";
  case C_FILE: return "C_FILE";
  case C_LINE: return "C_LINE";
  case C_ALIAS: return "C_ALIAS";
  case C_HIDDEN: return "C_HIDDEN";
  case C_EOF: return "C_EOF";
  case C_LIST: return "C_LIST";
  case C_SECTION: return "C_SECTION";
  default:
    snprintf(buffer, sizeof_buffer, "%d", cls);
    return buffer;
  }
}

void print_sym_table (gp_object_type *object)
{
  gp_symbol_type *symbol;
  gp_aux_type *aux;
  char *section;
  int i;
  int idx = 1;
  char buffer_type[8], buffer_derived_type[8], buffer_class[8];

  symbol = object->symbols;

  printf("Symbol Table\n");
  printf("Idx  Name                     Section          Value      Type     DT           Class     NumAux \n");

  while (symbol != NULL) {
    if (symbol->section_number == N_DEBUG) {
      section = "DEBUG";
    } else if (symbol->section_number == N_ABS) {
      section = "ABSOLUTE";
    } else if (symbol->section_number == N_UNDEF) {
      section = "UNDEFINED";
    } else {
      assert(symbol->section != NULL);
      section = symbol->section->name;
    }

    printf("%04d %-24s %-16s %#-10lx %-8s %-12s %-9s %-4i\n",
           idx,
           symbol->name,
           section,
           symbol->value,
           format_sym_type(symbol->type, buffer_type, sizeof(buffer_type)),
           format_sym_derived_type(symbol->derived_type, buffer_derived_type, sizeof(buffer_derived_type)),
           format_sym_class(symbol->class, buffer_class, sizeof(buffer_class)),
           symbol->num_auxsym);

    aux = symbol->aux_list;
    while (aux != NULL) {

#define AUX_INDENT "      "

      switch (aux->type) {
      case AUX_DIRECT:
        printf(AUX_INDENT "command = \"%c\"\n",
               aux->_aux_symbol._aux_direct.command);
        printf(AUX_INDENT "string = \"%s\"\n",
               aux->_aux_symbol._aux_direct.string);
        break;
      case AUX_FILE:
        if (!state.suppress_names) {
          printf(AUX_INDENT "file = %s\n",
                 aux->_aux_symbol._aux_file.filename);
        }
        printf(AUX_INDENT "line included = %li\n",
               aux->_aux_symbol._aux_file.line_number);
        printf(AUX_INDENT "flags = %x\n",
               aux->_aux_symbol._aux_file.flags);
        break;
      case AUX_IDENT:
        printf(AUX_INDENT "string = \"%s\"\n",
               aux->_aux_symbol._aux_ident.string);
        break;
      case AUX_SCN:
        printf(AUX_INDENT "length = %li\n",
               aux->_aux_symbol._aux_scn.length);
        printf(AUX_INDENT "number of relocations = %i\n",
               aux->_aux_symbol._aux_scn.nreloc);
        printf(AUX_INDENT "number of line numbers = %i\n",
               aux->_aux_symbol._aux_scn.nlineno);
        break;
      case AUX_FCN_CALLS: {
        struct gp_symbol_type *callee = aux->_aux_symbol._aux_fcn_calls.callee;
        printf(AUX_INDENT "callee = %s\n",
               callee != NULL ? callee->name : "higher order");
        printf(AUX_INDENT "is_interrupt = %lu\n",
               aux->_aux_symbol._aux_fcn_calls.is_interrupt);
        break;
      }
      default:
        printf("%ld  ", aux->type);
        for (i = 0; i < object->symbol_size; i++) {
          printf("%02x", aux->_aux_symbol.data[i] & 0xFF);
          if (i & 1) {
            printf(" ");
          }
        }
        for (i = 0; i < object->symbol_size; i++) {
          int c = aux->_aux_symbol.data[i] & 0xFF;
          putchar( (isprint(c)) ? c : '.');
        }
        putchar('\n');
      }
      aux = aux->next;
      ++idx;
    }

    symbol = symbol->next;
    ++idx;
  }

  printf("\n");
}

void export_sym_table(gp_object_type *object)
{
  gp_symbol_type *symbol;
  char buffer[BUFSIZ];

  symbol = object->symbols;

  while (symbol != NULL) {

    if ((state.export.enabled) &&
        (symbol->class == C_EXT) &&
        (symbol->section_number > 0)) {

      coff_type(symbol->type, buffer, sizeof(buffer));
      fprintf(state.export.f, "  extern %s ; %s\n",
              symbol->name,
              buffer);
    }

    symbol = symbol->next;
  }
}

void print_binary(unsigned char *data, long int file_size)
{

  long int i, j;
  int memory;
  int c;

  printf("\nObject file size = %li bytes\n", file_size);

  printf("\nBinary object file contents:");
  for (i = 0; i < file_size; i += 16) {
    printf("\n%06lx", i);

    for(j = 0; j < 16; j += 2) {
      memory = (data[i + j] << 8) | data[i+j+1];
      if ((i+j) >= file_size) {
        printf("     ");
      } else {
        printf(" %04x", memory);
      }
    }

    printf(" ");

    for(j = 0; j < 16; j += 2) {
      if ((i+j) < file_size) {
        c = data[i + j];
        putchar( (isprint(c)) ? c : '.');

        c = data[i + j + 1];
        putchar( (isprint(c)) ? c : '.');
      }
    }

  }

  printf("\n\n");
}

void show_usage(void)
{
  printf("Usage: gpvo [options] file\n");
  printf("Options: [defaults in brackets after descriptions]\n");
  printf("  -b, --binary               Print binary data.\n");
  printf("  -c, --mnemonics            Decode special mnemonics.\n");
  printf("  -f, --file                 File header.\n");
  printf("  -h, --help                 Show this usage message.\n");
  printf("  -n, --no-names             Suppress filenames.\n");
  printf("  -s, --section              Section data.\n");
  printf("  -t  --symbol               Symbol table.\n");
  printf("  -v, --version              Show version.\n");
  printf("  -x FILE, --export FILE     Export symbols to include file.\n");
  printf("  -y, --extended             Enable 18xx extended mode.\n");
  printf("\n");
  printf("Report bugs to:\n");
  printf("%s\n", PACKAGE_BUGREPORT);
  exit(0);
}

#define GET_OPTIONS "?bcfhnstvx:y"

  /* Used: himpsv */
  static struct option longopts[] =
  {
    { "binary",      0, 0, 'b' },
    { "mnemonics",   0, 0, 'c' },
    { "file",        0, 0, 'f' },
    { "help",        0, 0, 'h' },
    { "no-names",    0, 0, 'n' },
    { "section",     0, 0, 's' },
    { "symbol",      0, 0, 't' },
    { "version",     0, 0, 'v' },
    { "export",      1, 0, 'x' },
    { "extended",    0, 0, 'y' },
    { 0, 0, 0, 0 }
  };

#define GETOPT_FUNC getopt_long(argc, argv, GET_OPTIONS, longopts, 0)

int main(int argc, char *argv[])
{
  extern int optind;
  int c;
  int usage = 0;
  char buffer[BUFSIZ];

  gp_init();

  /* initalize */
  state.dump_flags = 0;
  state.suppress_names = false;
  state.export.enabled = false;

  while ((c = GETOPT_FUNC) != EOF) {
    switch (c) {
    case '?':
    case 'h':
      usage = 1;
      break;
    case 'b':
      state.dump_flags |= PRINT_BINARY;
      break;
    case 'c':
      gp_decode_mnemonics = true;
      break;
    case 'f':
      state.dump_flags |= PRINT_HEADER;
      break;
    case 'n':
      state.suppress_names = true;
      break;
    case 's':
      state.dump_flags |= PRINT_SECTIONS;
      break;
    case 't':
      state.dump_flags |= PRINT_SYMTBL;
      break;
    case 'y':
      gp_decode_extended = true;
      break;
    case 'x':
      state.export.enabled = true;
      state.export.filename = optarg;
      break;
    case 'v':
      fprintf(stderr, "%s\n", GPVO_VERSION_STRING);
      exit(0);
    }
    if (usage)
      break;
  }

  if ((optind + 1) == argc) {
    state.filename = argv[optind];
  } else {
    usage = 1;
  }

  if (usage) {
    show_usage();
  }

  if (!state.dump_flags) {
    /* no command line flags were set so print everything */
    state.dump_flags = PRINT_HEADER | PRINT_SECTIONS | PRINT_SYMTBL;
  }

  if (gp_identify_coff_file(state.filename) != object_file_v2 &&
      gp_identify_coff_file(state.filename) != object_file) {
    gp_error("\"%s\" is not a valid object file", state.filename);
    exit(1);
  }

  state.object = gp_read_coff(state.filename);
  state.file = gp_read_file(state.filename);

  if (state.export.enabled) {
    state.export.f = fopen(state.export.filename, "w");
    if (state.export.f == NULL) {
      perror(state.export.filename);
      exit(1);
    }

    gp_date_string(buffer, sizeof(buffer));

    fprintf(state.export.f, "; %s\n", state.export.filename);
    fprintf(state.export.f, "; generated by %s on %s\n",
            GPVO_VERSION_STRING,
            buffer);
    fprintf(state.export.f, "; from %s\n\n",
            state.filename);

    export_sym_table(state.object);

    fclose(state.export.f);

    /* suppress normal output */
    state.dump_flags = 0;
  }

  if (state.dump_flags & PRINT_HEADER) {
    print_header(state.object);
  }

  if (state.dump_flags & PRINT_SECTIONS) {
    print_sec_list(state.object);
  }

  if (state.dump_flags & PRINT_SYMTBL) {
    print_sym_table(state.object);
  }

  if (state.dump_flags & PRINT_BINARY) {
    print_binary(state.file->file, state.file->size);
  }

  return EXIT_SUCCESS;
}
