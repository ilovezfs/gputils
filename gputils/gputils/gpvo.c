/* GNU PIC view object
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

#include "libgputils.h"
#include "gpvo.h"

struct gpvo_state state;

void print_f_header(struct filehdr *fileheader) 
{
  char *time = ctime(&fileheader->f_timdat);

  /* strip the newline from time */
  time[strlen(time)-1] = '\0';

  printf("COFF File Header\n");
  printf("Target Machine       %#x\n",  fileheader->f_magic);
  printf("Number of Sections   %i\n",   fileheader->f_nscns);
  printf("Time Stamp           %#lx (%s)\n", fileheader->f_timdat, time);
  printf("Symbol Table Pointer %#lx\n", fileheader->f_symptr);
  printf("Number of Symbols    %li\n",  fileheader->f_nsyms);
  printf("Optional Header Size %#x\n",  fileheader->f_opthdr);
  printf("Characteristics      %#x\n",  fileheader->f_flags);

  if (fileheader->f_flags & F_RELFLG) {
    printf("  Relocation info has been stripped.\n");
  }
  if (fileheader->f_flags & F_EXEC) {
    printf("  File is executable.\n");
  }
  if (fileheader->f_flags & F_LNNO) {
    printf("  Line numbers have been stripped.\n");
  }
  if (fileheader->f_flags & L_SYMS) {
    printf("  Local symbols have been stripped.\n");
  }
  if (fileheader->f_flags & F_GENERIC) {
    printf("  Processor independant file for a core.\n");
  }

  printf("\n");
  return;
}

void print_opt_header(struct opthdr *optheader)
{
  printf("Optional Header\n");
  printf("Option Magic Number  %#x\n",  optheader->opt_magic);
  printf("Compiler Version     %#x\n",  optheader->vstamp);
  printf("Processor Type       %#lx (%s)\n", 
         optheader->proc_type,
         state.processor_name);
  printf("ROM Width            %li\n", optheader->rom_width_bits);
  printf("RAM Width            %li\n", optheader->ram_width_bits);
  printf("\n");
  
  return;
}

void print_reloc_list(gp_reloc_type *relocations)
{
  int i = 0;
  struct reloc *current;

  printf("Relocations Table\n");
  printf("Number   Address  Symbol   Offset   Type\n");

  while (relocations != NULL) {
    current = &relocations->relocation;
    printf("%-8i %#-8lx %-8li %#-8x %#-4x\n", 
           i,
           current->r_vaddr,
           current->r_symndx,
           current->r_offset,
           current->r_type);
    
    i++;
    relocations = relocations->next;
  }

  printf("\n");

}

void print_linenum_list(gp_linenum_type *linenumbers)
{
  int i = 0;
  struct lineno *current;

  printf("Line Number Table\n");
  printf("Number   Symbol   Line     Address  Flags    Function\n");

  while (linenumbers != NULL) {
    current = &linenumbers->linenumber;
    printf("%-8i %-8li %-8i %#-8lx %#-8x %#-8lx\n", 
           i,
           current->l_srcndx,
           current->l_lnno,
           current->l_paddr,
           current->l_flags,
           current->l_fcnndx);
  
    i++;
    linenumbers = linenumbers->next;
  }

  printf("\n");

}

void print_data(MemBlock *data, int org)
{
  int memory;
  char buffer[BUFSIZ];
  
  printf("Data\n");
  while (1) {
    memory = i_memory_get(data, org);
    if ((memory && MEM_USED_MASK) == 0)
      break;
    
    gp_disassemble(data, &org, state.class, buffer);
    printf("%06x:  %04x  %s\n", org, memory & 0xffff, buffer);
    org++;
  }
  printf("\n");

}

void print_sec_header(struct scnhdr *header)
{
  char name[BUFSIZ];

  gp_coffgen_fetch_section_name(state.object, header, &name[0]);

  printf("Section Header\n");
  printf("Name                    %s\n",   name);
  printf("Physical Address        %#lx\n", header->s_paddr);
  printf("Virtual Address         %#lx\n", header->s_vaddr);
  printf("Size of Section         %li\n",  header->s_size);
  printf("File pointer to Data    %#lx\n", header->s_scnptr);
  printf("Pointer to Relocations  %#lx\n", header->s_relptr);
  printf("Pointer to Line Numbers %#lx\n", header->s_lnnoptr);
  printf("Number of Relocations   %i\n",   header->s_nreloc);
  printf("Number of Line Numbers  %i\n",   header->s_nlnno);
  printf("Flags                   %#lx\n", header->s_flags); 
  if (header->s_flags & STYP_TEXT) {
    printf("  Executable code.\n");
  }
  if (header->s_flags & STYP_DATA) {
    printf("  Initialized data.\n");
  }
  if (header->s_flags & STYP_BSS) {
    printf("  Uninitialized data.\n");
  }
  if (header->s_flags & STYP_DATA_ROM) {
    printf("  Initialized data for ROM.\n");
  }
  if (header->s_flags & STYP_ABS) {
    printf("  Absolute.\n");
  }
  if (header->s_flags & STYP_SHARED) {
    printf("  Shared across banks.\n");
  }
  if (header->s_flags & STYP_OVERLAY) {
    printf("  Overlaid with other sections from different objects modules.\n");
  }
  if (header->s_flags & STYP_ACCESS) {
    printf("  Available using access bit.\n");
  }
  if (header->s_flags & STYP_ACTREC) {
    printf("  Contains the activation record for a function.\n");
  }

  printf("\n"); 

}

void print_sec_list(gp_section_type *section)
{

  while (section != NULL) {
    print_sec_header(&section->header);

    if ((section->header.s_size) && (section->header.s_scnptr)) {
      print_data(section->data, section->header.s_paddr);
    }
    if ((section->header.s_nreloc) && (section->header.s_relptr)) {
      print_reloc_list(section->relocations);
    }
    if ((section->header.s_nlnno) && (section->header.s_lnnoptr)) {
      print_linenum_list(section->line_numbers);
    }
    
    section = section->next;
  }

}

void print_sym_table (gp_symbol_type *symbol)
{
  int i = 0;
  struct syment *current;
  char name[BUFSIZ];

  printf("Symbol Table\n");
  printf("Number   Value    Section  Type     StClass  NumAux   Name\n");

  while (symbol != NULL) {
    current = &symbol->symbol;
    gp_coffgen_fetch_symbol_name(state.object, current, &name[0]);
    printf("%-8i %#-8lx %-8i %#-8x %#-8x %-8i %s\n", 
           i,
           current->value,
           current->sec_num,
           current->type,
           current->st_class,
           current->num_auxsym,
           name);

    if (current->num_auxsym != 0) {
      /* increment the index and print the auxillary symbol */
      i += current->num_auxsym;
      symbol = symbol->next;

      /* FIXME: print the aux symbol */
    }

    i++;
    symbol = symbol->next;
  }

  printf("\n"); 

  return;
}

void print_strtbl(char *table)
{
  char *string;
  unsigned long length; 
  int address = 4;
  
  length = _get_32(table);
    
  printf("String Table\n");
  printf("Offset   String\n");
  while (address < length) {
    string = &table[address];
    printf("%#-8x %s\n", address, string);
    address += strlen(string) + 1;
  }
  printf("\n");

  return;
}

void print_binary(char *data, long int file_size) 
{

  long int i, j;
  int memory;
  char c;

  printf("\nObject file size = %li bytes\n", file_size);

  printf("\nBinary object file contents:");
  for (i = 0; i < file_size; i += 16) {
    printf("\n%06lx", i);

    for(j = 0; j < 16; j += 2) {
      memory = data[i + j];
      memory = ((memory << 8) & 0xff00) | (data[i+j+1] & 0xff); 
      if ((i+j) >= file_size) {
        printf("     ");      
      } else {
        printf(" %04x", memory);
      }
    }

    printf(" ");
    
    for(j = 0; j < 16; j += 2) {
      if ((i+j) < file_size) {      
        c = data[i + j] & 0xff;
        putchar( (isprint(c)) ? c : '.');

        c = data[i + j + 1] & 0xff;
        putchar( (isprint(c)) ? c : '.');
      }
    }
    
  }

  printf("\n\n");

  return;
}

void show_usage(void)
{
  printf("Usage: gpvo [options] file\n");
  printf("Options: [defaults in brackets after descriptions]\n");
  printf("  -a, --auxilary             Auxilary record.\n");
  printf("  -b, --binary               Print binary data.\n");
  printf("  -f, --file                 File header.\n");
  printf("  -h, --help                 Show this usage message.\n");
  printf("  -o, --optional             Optional header.\n");
  printf("  -s, --section              Section data.\n");
  printf("  -t  --symbol               Symbol table.\n");
  printf("  -v, --version              Show version.\n");
  printf("\n");
  printf("Report bugs to:\n");
  printf("%s\n", BUG_REPORT_URL);
  exit(0);
}

#define GET_OPTIONS "?bfhorstv"

  /* Used: himpsv */
  static struct option longopts[] =
  {
    { "binary",      0, 0, 'b' },
    { "file",        0, 0, 'f' },
    { "help",        0, 0, 'h' },
    { "optional",    0, 0, 'o' },
    { "string",      0, 0, 'r' },
    { "section",     0, 0, 's' },
    { "symbol",      0, 0, 't' },
    { "version",     0, 0, 'v' },
    { 0, 0, 0, 0 }
  };

#define GETOPT_FUNC getopt_long(argc, argv, GET_OPTIONS, longopts, 0)

int main(int argc, char *argv[])
{
  extern int optind;
  int c;
  int usage = 0;
  unsigned long coff_type = 0;

  /* initalize */
  state.processor = no_processor;
  state.quiet = 0;
  state.dump_flags = 0;

  while ((c = GETOPT_FUNC) != EOF) {
    switch (c) {
    case '?':
    case 'h':
      usage = 1;
      break;
    case 'b':
      state.dump_flags |= PRINT_BINARY;
      break;
    case 'f':
      state.dump_flags |= PRINT_FILE_HEADER;
      break;
    case 'o':
      state.dump_flags |= PRINT_OPT_HEADER;
      break;
    case 'r':
      state.dump_flags |= PRINT_STRTBL;
      break;
    case 's':
      state.dump_flags |= PRINT_SECTIONS;
      break;
    case 't':
      state.dump_flags |= PRINT_SYMTBL;
      break;
    case 'v':
      fprintf(stderr, "%s\n", GPVO_VERSION_STRING);
      exit(0);
    }
    if (usage)
      break;
  }

  if (optind < argc) {
    state.filename = argv[optind];
  } else {
    usage = 1;
  }

  if (usage) {
    show_usage();
  }

  if (!state.dump_flags) {
    /* no command line flags were set so print everything */
    state.dump_flags = 0xff;
  }

  if (gp_identify_coff_file(state.filename) != object_file) {
    printf("error: \"%s\" is not a valid object file", state.filename);
    exit(1);
  }

  state.object = gp_read_coff(state.filename);
  state.file = gp_read_file(state.filename);

  /* determine the processor */
  coff_type = state.object->opt_header.proc_type;
  state.processor = gp_processor_coff_proc(coff_type);
  if (state.processor == no_processor) {
    /* FIXME: error invalid processor */  
  }
  state.class = gp_processor_class(state.processor);
  state.processor_name = gp_processor_coff_name(coff_type, 1);
  if (state.processor_name == NULL) {
    /* FIXME: error invalid processor name*/  
  }  
  
  if (state.dump_flags & PRINT_BINARY) {
    print_binary(state.file->file, state.file->size);
  }

  if (state.dump_flags & PRINT_FILE_HEADER) {
    print_f_header(&state.object->file_header);
  }

  if (state.dump_flags & PRINT_OPT_HEADER) {
    print_opt_header(&state.object->opt_header);
  }

  if (state.dump_flags & PRINT_SECTIONS) {
    print_sec_list(state.object->sections);
  }

  if (state.dump_flags & PRINT_SYMTBL) {
    print_sym_table(state.object->sym_table);
  }

  if (state.dump_flags & PRINT_STRTBL) {
    print_strtbl(&state.object->string_table[0]);
  }

  return 0;

}
