/* GNU PIC view object
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Craig Franklin

    Copyright (C) 2016 Molnar Karoly <molnarkaroly@users.sf.net>

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

enum {
  OPT_STRICT_OPTIONS = 0x100
};

#define GET_OPTIONS "bcfhnstvx:y"

/* Used: himpsv */
static struct option longopts[] =
{
  { "binary",         no_argument,       NULL, 'b' },
  { "mnemonics",      no_argument,       NULL, 'c' },
  { "file",           no_argument,       NULL, 'f' },
  { "help",           no_argument,       NULL, 'h' },
  { "no-names",       no_argument,       NULL, 'n' },
  { "section",        no_argument,       NULL, 's' },
  { "strict-options", no_argument,       NULL, OPT_STRICT_OPTIONS },
  { "symbol",         no_argument,       NULL, 't' },
  { "version",        no_argument,       NULL, 'v' },
  { "export",         required_argument, NULL, 'x' },
  { "extended",       no_argument,       NULL, 'y' },
  { NULL,             no_argument,       NULL, '\0'}
};

/*------------------------------------------------------------------------------------------------*/

static void
_show_usage(void)
{
  printf("Usage: gpvo [options] file\n");
  printf("Options: [defaults in brackets after descriptions]\n");
  printf("  -b, --binary            Print binary data.\n");
  printf("  -c, --mnemonics         Decode special mnemonics.\n");
  printf("  -f, --file              File header.\n");
  printf("  -h, --help              Show this usage message.\n");
  printf("  -n, --no-names          Suppress filenames.\n");
  printf("  -s, --section           Section data.\n");
  printf("      --strict-options    If this is set, then an option may not be parameter\n"
         "                            of an another option. For example: -x --symbol\n");
  printf("  -t  --symbol            Symbol table.\n");
  printf("  -v, --version           Show version.\n");
  printf("  -x FILE, --export FILE  Export symbols to include file.\n");
  printf("  -y, --extended          Enable 18xx extended mode.\n\n");
  printf("Report bugs to:\n");
  printf("%s\n", PACKAGE_BUGREPORT);
  exit(0);
}

/*------------------------------------------------------------------------------------------------*/

static void
_print_header(const gp_object_t *Object)
{
  static struct magic_s {
    uint16_t    magic_num;
    const char *magic_name;
  } magic[] = {
    { 0x1234, "MICROCHIP_MAGIC_v1" },
    { 0x1240, "MICROCHIP_MAGIC_v2" }
  };

  time_t      time;
  const char *processor_name;
  int         i;
#ifdef HAVE_LOCALE_H
  char        time_str[256];
#else
  char       *time_str;
#endif

  time = (time_t)Object->time;

#ifdef HAVE_LOCALE_H
  setlocale(LC_ALL, "");
  strftime(time_str, sizeof(time_str), "%c", localtime(&time));
#else
  time_str = ctime(&time);
  /* strip the newline from time */
  time_str[strlen(time_str) - 1] = '\0';
#endif

  processor_name = gp_processor_name(Object->processor, 2);

  printf("COFF File and Optional Headers\n");

  for (i = 0; i < ARRAY_SIZE(magic); ++i) {
    if (magic[i].magic_num == Object->version) {
      break;
    }
  }

  printf("COFF version         %#x: %s\n", Object->version,
         (i < ARRAY_SIZE(magic)) ? magic[i].magic_name : "unknown");
  printf("Processor Type       %s\n",  processor_name);
  printf("Time Stamp           %s\n",  time_str);
  printf("Number of Sections   %zu\n", Object->section_list.num_nodes);
  printf("Number of Symbols    %u\n",  Object->num_symbols);
  printf("Characteristics      %#x\n", Object->flags);

  if (Object->flags & F_RELFLG) {
    printf("  Relocation info has been stripped.\n");
  }

  if (Object->flags & F_EXEC) {
    printf("  File is executable.\n");
  }

  if (Object->flags & F_LINENO) {
    printf("  Line numbers have been stripped.\n");
  }

  if (Object->flags & F_ABSOLUTE) {
    printf("  The MPASM assembler object file is from absolute assembly code.\n");
  }

  if (Object->flags & L_SYMS) {
    printf("  Local symbols have been stripped.\n");
  }

  if (Object->flags & F_EXTENDED18) {
    printf("  The COFF file produced utilizing the Extended mode.\n");
  }

  if (Object->flags & F_GENERIC) {
    printf("  Processor independent file for a core.\n");
  }

  printf("\n");
}

/*------------------------------------------------------------------------------------------------*/

static const char *
_format_reloc_type(uint16_t Type, char *Buffer, size_t Sizeof_buffer)
{
  snprintf(Buffer, Sizeof_buffer, "0x%04x %-20s", Type, gp_coffgen_reloc_type_to_str(Type));
  return Buffer;
}

/*------------------------------------------------------------------------------------------------*/

static void
_print_relocation_list(proc_class_t Class, const gp_reloc_t *Relocation, const char *Column_gap)
{
  char        buffer[32];
  int         addr_digits;

  addr_digits = Class->addr_digits;

  printf("Relocations Table\n"
         "Address     Offset    Type                        Symbol\n");

  while (Relocation != NULL) {
    printf("0x%0*x%s    0x%0*x%s  %-25s %-s\n",
           addr_digits, gp_processor_insn_from_byte_c(Class, Relocation->address), Column_gap,
           addr_digits, Relocation->offset, Column_gap,
           _format_reloc_type(Relocation->type, buffer, sizeof(buffer)),
           Relocation->symbol->name);

    Relocation = Relocation->next;
  }

  printf("\n");
}

/*------------------------------------------------------------------------------------------------*/

static void
_print_linenum_list(proc_class_t Class, const gp_linenum_t *Linenumber, const char *Column_gap)
{
  const char *filename;
  int         addr_digits;

  addr_digits = Class->addr_digits;

  printf("Line Number Table\n"
         "Line      Address     Symbol\n");

  while (Linenumber != NULL) {
    if (state.suppress_names) {
      filename = Linenumber->symbol->name;
    }
    else {
      filename = Linenumber->symbol->aux_list.first->_aux_symbol._aux_file.filename;
    }

    printf("%-8i  0x%0*x%s    %s\n",
           Linenumber->line_number,
           addr_digits, gp_processor_insn_from_byte_c(Class, Linenumber->address), Column_gap,
           filename);

    Linenumber = Linenumber->next;
  }

  printf("\n");
}

/*------------------------------------------------------------------------------------------------*/

static void
_print_data(pic_processor_t Processor, const gp_section_t *Section)
{
  proc_class_t class;
  char         buffer[BUFSIZ];
  unsigned int address;
  int          num_words;
  int          addr_digits;
  unsigned int bsr_boundary;
  uint16_t     word;
  uint8_t      byte;

  class        = Processor->class;
  address      = Section->address;
  bsr_boundary = gp_processor_bsr_boundary(Processor);
  addr_digits  = class->addr_digits;

  buffer[0] = '\0';

  printf("Data\n");
  while (true) {
    if ((Section->flags & STYP_TEXT) && (class->find_insn != NULL)) {
      if (class->i_memory_get(Section->data, address, &word, NULL, NULL) != W_USED_ALL) {
        break;
      }

      num_words = gp_disassemble(Section->data, address, class, bsr_boundary, 0,
                                 GPDIS_SHOW_ALL_BRANCH, buffer, sizeof(buffer), 0);
      printf("%0*x:  %04x  %s\n",
             addr_digits, gp_processor_insn_from_byte_c(class, address), word, buffer);

      if (num_words != 1) {
        if (class->i_memory_get(Section->data, address + WORD_SIZE, &word, NULL, NULL) != W_USED_ALL) {
          break;
        }

        printf("%0*x:  %04x\n",
               addr_digits, gp_processor_insn_from_byte_c(class, address + WORD_SIZE), word);
      }

      address += num_words * WORD_SIZE;
    }
    else if ((Section->flags & STYP_DATA_ROM) || (class == PROC_CLASS_EEPROM16)) {
      if (class->i_memory_get(Section->data, address, &word, NULL, NULL) != 0) {
        printf("%0*x:  %04x\n", addr_digits, gp_processor_insn_from_byte_c(class, address), word);
        address += WORD_SIZE;
      }
      else {
        if (gp_mem_b_get(Section->data, address, &byte, NULL, NULL)) {
          printf("%0*x:  %02x\n", addr_digits, gp_processor_insn_from_byte_c(class, address), byte);
        }

        break;
      }
    }
    else {
      /* STYP_DATA or STYP_ACTREC, or EEPROM8 */
      if (!gp_mem_b_get(Section->data, address, &byte, NULL, NULL)) {
        break;
      }

      printf("%0*x:  %02x\n", addr_digits, address, byte);
      ++address;
    }
  }

  printf("\n");
}

/*------------------------------------------------------------------------------------------------*/

static void
_print_section_header(proc_class_t class, const gp_section_t *section)
{
  unsigned int org_to_byte_shift;

  org_to_byte_shift = (section->flags & STYP_ROM_AREA) ? class->org_to_byte_shift : 0;

  printf("Section Header\n");
  printf("Name                    %s\n",  section->name);
  printf("Physical address        %#x\n", gp_insn_from_byte(org_to_byte_shift, section->address));
  printf("Virtual address         %#x\n", gp_insn_from_byte(org_to_byte_shift, section->virtual_address));
  printf("Size of Section         %u\n",  section->size);
  printf("Number of Relocations   %zu\n", section->relocation_list.num_nodes);
  printf("Number of Line Numbers  %zu\n", section->line_number_list.num_nodes);
  printf("Flags                   %#x\n", section->flags);

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

/*------------------------------------------------------------------------------------------------*/

static void
_print_section_list(const gp_object_t *Object)
{
  proc_class_t        class;
  const gp_section_t *section;
  char                column_gap[16];
  unsigned int        i;

  class = Object->class;
  i     = class->addr_digits;

  if (i > 6) {
    i = 6;
  }

  i = 6 - i;

  if (i >= (sizeof(column_gap) - 1)) {
    i = (sizeof(column_gap) - 1);
  }

  if (i > 0) {
    memset(column_gap, ' ', i);
  }

  column_gap[i] = '\0';

  section = Object->section_list.first;
  while (section != NULL) {
    _print_section_header(class, section);

    if ((section->size > 0) && (section->data_ptr > 0)) {
      _print_data(Object->processor, section);
    }

    if (section->relocation_list.num_nodes > 0) {
      _print_relocation_list(class, section->relocation_list.first, column_gap);
    }

    if (section->line_number_list.num_nodes > 0) {
      _print_linenum_list(class, section->line_number_list.first, column_gap);
    }

    section = section->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

static void
_coff_type(unsigned int Type, char *Buffer, size_t Sizeof_buffer)
{
  const char *str;

  switch (Type) {
    case T_NULL:   str = "NULL";                break; /* null */
    case T_VOID:   str = "void";                break; /* void */
    case T_CHAR:   str = "char";                break; /* character */
    case T_SHORT:  str = "short";               break; /* short integer */
    case T_INT:    str = "int";                 break; /* integer */
    case T_LONG:   str = "long int";            break; /* long integer */
    case T_FLOAT:  str = "float";               break; /* floating point */
    case T_DOUBLE: str = "double";              break; /* double length floating point */
    case T_STRUCT: str = "struct";              break; /* structure */
    case T_UNION:  str = "union";               break; /* union */
    case T_ENUM:   str = "enum";                break; /* enumeration */
    case T_MOE:    str = "enum";                break; /* member of enumeration */
    case T_UCHAR:  str = "unsigned char";       break; /* unsigned character */
    case T_USHORT: str = "unsigned short";      break; /* unsigned short */
    case T_UINT:   str = "unsigned int";        break; /* unsigned integer */
    case T_ULONG:  str = "unsigned long";       break; /* unsigned long */
    case T_LNGDBL: str = "long double";         break; /* long double floating point */
    case T_SLONG:  str = "short long";          break; /* short long */
    case T_USLONG: str = "unsigned short long"; break; /* unsigned short long */
    default:       str = NULL;                  break;
  }

  if (str != NULL) {
    snprintf(Buffer, Sizeof_buffer, str);
  }
  else {
    snprintf(Buffer, Sizeof_buffer, "unknown(%u)", Type);
  }
}

/*------------------------------------------------------------------------------------------------*/

static const char *
_format_sym_type(unsigned int Type, char *Buffer, size_t Sizeof_buffer)
{
  const char *str;

  str = gp_coffgen_symbol_type_to_str(Type);
  if (str != NULL) {
    return str;
  }

  snprintf(Buffer, Sizeof_buffer, "%u", Type);
  return Buffer;
}

/*------------------------------------------------------------------------------------------------*/

static const char *
_format_sym_derived_type(unsigned int Type, char *Buffer, size_t Sizeof_buffer)
{
  const char *str;

  str = gp_coffgen_symbol_derived_type_to_str(Type);
  if (str != NULL) {
    return str;
  }

  snprintf(Buffer, Sizeof_buffer, "%u", Type);
  return Buffer;
}

/*------------------------------------------------------------------------------------------------*/

static const char *
_format_sym_class(unsigned int Class, char *Buffer, size_t Sizeof_buffer)
{
  const char *str;

  str = gp_coffgen_symbol_class_to_str(Class);
  if (str != NULL) {
    return str;
  }

  snprintf(Buffer, Sizeof_buffer, "%u", Class);
  return Buffer;
}

/*------------------------------------------------------------------------------------------------*/

#define AUX_INDENT              "      "

static void
_print_symbol_table(const gp_object_t *Object)
{
  static char  buf[64];

  gp_symbol_t *symbol;
  gp_symbol_t *callee;
  gp_aux_t    *aux;
  const char  *section;
  int          c;
  int          i;
  int          idx = 1;
  char         buffer_type[8];
  char         buffer_derived_type[8];
  char         buffer_class[8];

  printf("Symbol Table\n");
  printf("Idx  Name                     Section          Value      Type     DT           Class     NumAux\n");

  symbol = Object->symbol_list.first;
  while (symbol != NULL) {
    if (symbol->section_number == N_DEBUG) {
      section = "DEBUG";
    }
    else if (symbol->section_number == N_ABS) {
      section = "ABSOLUTE";
    }
    else if (symbol->section_number == N_UNDEF) {
      section = "UNDEFINED";
    }
    else {
      if (symbol->section != NULL) {
        section = symbol->section->name;
      }
      else {
        snprintf(buf, sizeof(buf), "Bad num.: %u", symbol->section_number);
        section = buf;
      }
    }

    printf("%04d %-24s %-16s %#-10lx %-8s %-12s %-9s %-4zu\n",
           idx,
           symbol->name,
           section,
           symbol->value,
           _format_sym_type(symbol->type, buffer_type, sizeof(buffer_type)),
           _format_sym_derived_type(symbol->derived_type, buffer_derived_type, sizeof(buffer_derived_type)),
           _format_sym_class(symbol->class, buffer_class, sizeof(buffer_class)),
           symbol->aux_list.num_nodes);

    aux = symbol->aux_list.first;
    while (aux != NULL) {
      switch (aux->type) {
        case AUX_DIRECT:
          printf(AUX_INDENT "command = \"%c\"\n", aux->_aux_symbol._aux_direct.command);
          printf(AUX_INDENT "string  = \"%s\"\n", aux->_aux_symbol._aux_direct.string);
          break;

        case AUX_FILE: {
          if (!state.suppress_names) {
            printf(AUX_INDENT "file = %s\n", aux->_aux_symbol._aux_file.filename);
          }
          printf(AUX_INDENT "line included = %u\n", aux->_aux_symbol._aux_file.line_number);
          printf(AUX_INDENT "flags         = %x\n", aux->_aux_symbol._aux_file.flags);
          break;
        }

        case AUX_IDENT:
          printf(AUX_INDENT "string = \"%s\"\n", aux->_aux_symbol._aux_ident.string);
          break;

        case AUX_SCN:
          printf(AUX_INDENT "length                 = %u\n", aux->_aux_symbol._aux_scn.length);
          printf(AUX_INDENT "number of relocations  = %u\n", aux->_aux_symbol._aux_scn.nreloc);
          printf(AUX_INDENT "number of line numbers = %u\n", aux->_aux_symbol._aux_scn.nlineno);
          break;

        case AUX_FCN_CALLS: {
          callee = aux->_aux_symbol._aux_fcn_calls.callee;
          printf(AUX_INDENT "callee       = %s\n", (callee != NULL) ? callee->name : "higher order");
          printf(AUX_INDENT "is_interrupt = %u\n", aux->_aux_symbol._aux_fcn_calls.is_interrupt);
          break;
        }

        default: {
          printf("%u  ", aux->type);
          for (i = 0; i < Object->symbol_size; i++) {
            printf("%02x", aux->_aux_symbol.data[i] & 0xFF);

            if (i & 1) {
              putchar(' ');
            }
          }
          for (i = 0; i < Object->symbol_size; i++) {
            c = aux->_aux_symbol.data[i] & 0xFF;
            putchar((isprint(c)) ? c : '.');
          }
          putchar('\n');
        }
      } /* switch (aux->type) */

      aux = aux->next;
      ++idx;
    }

    symbol = symbol->next;
    ++idx;
  }

  putchar('\n');
}

/*------------------------------------------------------------------------------------------------*/

static void
_export_symbol_table(gp_object_t *Object)
{
  gp_symbol_t *symbol;
  char         buffer[BUFSIZ];

  symbol = Object->symbol_list.first;
  while (symbol != NULL) {
    if ((state.export.enabled) && (symbol->class == C_EXT) && (symbol->section_number > N_UNDEF)) {
      _coff_type(symbol->type, buffer, sizeof(buffer));
      fprintf(state.export.f, "  extern %s ; %s\n", symbol->name, buffer);
    }

    symbol = symbol->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

#define BIN_DATA_LINE_SIZE    16

static void
_print_binary(const uint8_t *Data, long File_size)
{
  long         i;
  long         j;
  unsigned int memory;
  int          c;

  printf("\nObject file size = %li bytes\n", File_size);

  printf("\nBinary object file contents:");
  for (i = 0; i < File_size; i += BIN_DATA_LINE_SIZE) {
    printf("\n%06lx", i);

    for (j = 0; j < BIN_DATA_LINE_SIZE; j += 2) {
      memory = (Data[i + j] << 8) | Data[i + j + 1];

      if ((i + j) >= File_size) {
        printf("     ");
      }
      else {
        printf(" %04x", memory);
      }
    }

    putchar(' ');

    for (j = 0; j < BIN_DATA_LINE_SIZE; j += 2) {
      if ((i + j) < File_size) {
        c = Data[i + j];
        putchar((isprint(c)) ? c : '.');

        c = Data[i + j + 1];
        putchar((isprint(c)) ? c : '.');
      }
    }
  }

  printf("\n\n");
}

/*------------------------------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
  int         option_index;
  int         c;
  const char *command;
  gp_boolean  strict_options = false;
  gp_boolean  usage          = false;
  char        buffer[BUFSIZ];

  gp_init();

  /* initalize */
  state.dump_flags     = 0;
  state.suppress_names = false;
  state.export.enabled = false;

  /* Scan through the options for the --strict-options flag. */
  while ((c = getopt_long(argc, argv, GET_OPTIONS, longopts, NULL)) != EOF) {
    if (c == OPT_STRICT_OPTIONS) {
      strict_options = true;
      break;
    }
  }

  /* Restores the getopt_long index. */
  optind = 1;
  while (true) {
    /* This is necessary for the gp_exit_is_excluded_arg() function. */
    option_index = -1;
    command = argv[optind];
    if ((c = getopt_long(argc, argv, GET_OPTIONS, longopts, &option_index)) == EOF) {
      break;
    }

    if (strict_options) {
      gp_exit_if_arg_an_option(longopts, ARRAY_SIZE(longopts), option_index, optarg, c, command);
    }

    switch (c) {
    case '?':
    case 'h':
      usage = true;
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

    case OPT_STRICT_OPTIONS:
      /* do nothing */
      break;
    }

    if (usage)
      break;
  }

  if ((optind + 1) == argc) {
    state.filename = argv[optind];
  }
  else {
    usage = true;
  }

  if (usage) {
    _show_usage();
  }

  if (!state.dump_flags) {
    /* no command line flags were set so print everything */
    state.dump_flags = PRINT_HEADER | PRINT_SECTIONS | PRINT_SYMTBL;
  }

  if ((gp_identify_coff_file(state.filename) != GP_COFF_OBJECT_V2) &&
      (gp_identify_coff_file(state.filename) != GP_COFF_OBJECT)) {
    gp_error("\"%s\" is not a valid object file", state.filename);
    exit(1);
  }

  state.object = gp_read_coff(state.filename);
  state.file   = gp_read_file(state.filename);

  if (state.export.enabled) {
    state.export.f = fopen(state.export.filename, "w");

    if (state.export.f == NULL) {
      perror(state.export.filename);
      exit(1);
    }

    gp_date_string(buffer, sizeof(buffer));

    fprintf(state.export.f, "; %s\n", state.export.filename);
    fprintf(state.export.f, "; generated by %s on %s\n", GPVO_VERSION_STRING, buffer);
    fprintf(state.export.f, "; from %s\n\n", state.filename);

    _export_symbol_table(state.object);

    fclose(state.export.f);

    /* suppress normal output */
    state.dump_flags = 0;
  }

  if (state.dump_flags & PRINT_HEADER) {
    _print_header(state.object);
  }

  if (state.dump_flags & PRINT_SECTIONS) {
    _print_section_list(state.object);
  }

  if (state.dump_flags & PRINT_SYMTBL) {
    _print_symbol_table(state.object);
  }

  if (state.dump_flags & PRINT_BINARY) {
    _print_binary(state.file->file, state.file->size);
  }

  return EXIT_SUCCESS;
}
