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
#include "gpcoff.h"
#include "gpreadobj.h"
#include "gpvo.h"
#include "gpdis.h"

struct gpvo_state state = {
    pic14,              /* processor type */  
    0,                  /* quiet */
    0                   /* print nothing */
};

char *fetch_string(unsigned long index)
{
  return &state.object->strtbl[index];
}

void print_f_header(struct filehdr *fileheader) 
{
  char *time = ctime(&fileheader->f_timdat);
  char *f_flags;

  /* strip the newline from time */
  time[strlen(time)-1] = '\0';

  switch(fileheader->f_flags) {
  case F_RELFLG:
    f_flags = "relocation info has been stripped";
    break;
  case F_EXEC:
    f_flags = "file is executable";
    break;
  case F_LNNO:
    f_flags = "line numbers have been stripped";
    break;
  case L_SYMS:
    f_flags = "local symbols have been stripped";
    break;
  case F_GENERIC:
    f_flags = "processor independant file for a core";
    break;
  default:
    f_flags = " ";
    break;
  }

  printf("COFF File Header\n");
  printf("Target Machine       %#x\n",  fileheader->f_magic);
  printf("Number of Sections   %i\n",   fileheader->f_nscns);
  printf("Time Stamp           %#lx (%s)\n", fileheader->f_timdat, time);
  printf("Symbol Table Pointer %#lx\n", fileheader->f_symptr);
  printf("Number of Symbols    %#lx\n", fileheader->f_nsyms);
  printf("Optional Header Size %#x\n",  fileheader->f_opthdr);
  printf("Characteristics      %#x (%s)\n", fileheader->f_flags, f_flags);
  printf("\n");
  return;
}

void print_opt_header(struct opthdr *optheader)
{
  printf("Optional Header\n");
  printf("Option Magic Number  %#x\n",  optheader->opt_magic);
  printf("Compiler Version     %#x\n",  optheader->vstamp);
  printf("Processor Type       %#lx\n", optheader->proc_type);
  printf("ROM Width            %#lx\n", optheader->rom_width_bits);
  printf("RAM Width            %#lx\n", optheader->ram_width_bits);
  printf("\n");
  return;
}

void print_reloc_list(struct reloc *relocations, int number)
{
  int i;
  struct reloc *current;

  printf("Relocations Table\n");

  for (i = 0; i < number; i++) {
    current = &relocations[i];
    printf("Address           ");
    printf("%#lx\n", current->r_vaddr);
    printf("Symbol Index      ");
    printf("%#lx\n", current->r_symndx);
    printf("Offset            ");
    printf("%#x\n", current->r_offset);
    printf("Type              ");
    printf("%#x\n", current->r_type);
    printf("\n");

  }

}

void print_linenum_list(struct lineno *linenumbers, int number)
{
  int i;
  struct lineno *current;

  printf("Line Number Table\n");

  for (i = 0; i < number; i++) {
    current = &linenumbers[i];
    printf("Source Symbol Index   ");
    printf("%#lx\n", current->l_srcndx);
    printf("Source Line Number    ");
    printf("%i\n", current->l_lnno);
    printf("Code Address          ");
    printf("%li\n", current->l_paddr);
    printf("Bit Flags             ");
    printf("%#x\n", current->l_flags);
    printf("Function Symbol Index ");
    printf("%#lx\n", current->l_fcnndx);
    printf("\n");

  }

}

void print_data(unsigned int *data, int number)
{
  int i = 0;
  int memory;
  char buffer[80];
  
  printf("Data\n");
  while (i < (number>>1)) {
    memory = data[i];    
    if (state.processor == pic12) {
      memory &= 0xfff;
      mem2asm12(memory, buffer);
    } else {
      memory &= 0x3fff;
      mem2asm14(memory, buffer);
    }
    printf("%06x:  %04x  %s\n", i, memory, buffer);
    i++;
  }
  printf("\n");

}

void print_sec_header(struct scnhdr *header)
{

  printf("Section Header\n");
  printf("Name                    ");
  if (header->s_name.ptr.s_zeros == 0) {
    printf("%s", fetch_string(header->s_name.ptr.s_offset));
  } else {
    int j;
    char c;

    for (j = 0; j < 8; j++) {
      c = header->s_name.name[j];
      putchar( (isprint(c)) ? c : ' ');
    }
  }
  printf("\n"); 
  printf("Physical Address        %#lx\n", header->s_paddr);
  printf("Virtual Address         %#lx\n", header->s_vaddr);
  printf("Size of Section         %#lx\n", header->s_size);
  printf("File pointer to Data    %#lx\n", header->s_scnptr);
  printf("Pointer to Relocations  %#lx\n", header->s_relptr);
  printf("Pointer to Line Numbers %#lx\n", header->s_lnnoptr);
  printf("Number of Relocations   %#x\n",  header->s_nreloc);
  printf("Number of Line Numbers  %#x\n",  header->s_nlnno);
  printf("Flags                   %#lx\n", header->s_flags); 
  printf("\n"); 

}

void print_sec_list(struct section *sections, int number)
{
  int i;
  struct section *current;

  for (i = 0; i < number; i++) {
    current = &sections[i];

    print_sec_header(&current->header);

    if (current->header.s_size) {
      print_data(current->data, current->header.s_size);
    }
    if (current->header.s_nreloc) {
      print_reloc_list(current->relocations, current->header.s_nreloc);
    }
    if (current->header.s_nlnno) {
      print_linenum_list(current->linenumbers, current->header.s_nlnno);
    }
    
  }

}

void print_sym_table (struct syment *symbol, int number)
{
  int i;
  struct syment *current;

  printf("Symbol Table\n");

  for (i = 0; i < number; i++) {
    current = &symbol[i];
    printf("Symbol            ");
    /* FIXME: use gp_fetch_symbol_name from gpreadobj.c */
    if (current->sym_name.ptr.s_zeros == 0) {
      printf("%s\n", fetch_string(current->sym_name.ptr.s_offset));
    } else {
      int j;
      char c;

      for (j = 0; j < 8; j++) {
        c = current->sym_name.name[j];
        putchar( (isprint(c)) ? c : ' ');
      }
      printf("\n");
    }
    printf("Value             ");
    printf("%#lx\n", current->value);
    printf("Section           ");
    printf("%#x\n", current->sec_num);
    printf("Type              ");
    printf("%#x\n", current->type);
    printf("Storage Class     ");
    printf("%#x\n", current->st_class);
    printf("Auxilary Symbols  ");
    printf("%#x\n", current->num_auxsym);
    printf("\n");

    /* In this version of coff there can only be 1 auxillary symbol */
    if (current->num_auxsym == 1) {
      /* increment the index and print the auxillary symbol */
      i++;




    }


  }

  return;
}

void print_strtbl(char *table)
{
  char *string;
  unsigned long length; 
  int address = 4;
  
  length = get_32((unsigned char *)table);
    
  printf("String Table\n");
  printf("Offset String\n");
  while (address < length) {
    string = &table[address];
    printf("[%#04x] %s\n", address, string);
    address += strlen(string) + 1;
  }
  printf("\n");

  return;
}

void print_binary(unsigned char *data, long int file_size) 
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
  printf("Usage: gpvo <options> <filename>\n");
  printf("Where <options> are:\n");
  printf("  -a, --auxilary             Auxilary record \n");
  printf("  -b, --binary               Print binary data \n");
  printf("  -f, --file                 File header \n");
  printf("  -h, --help                 Show this usage message \n");
  printf("  -o, --optional             Optional header \n");
  printf("  -p PROC, --processor PROC  Select processor \n");
  printf("  -s, --section              Section data \n");
  printf("  -t  --symbol               Symbol table \n");
  printf("  -v, --version              Show version\n");
  printf("\n");
  printf("Report bugs to:\n");
  printf("%s\n", BUG_REPORT_URL);
  exit(0);
}

#define GET_OPTIONS "?abfhop:stv"

  /* Used: himpsv */
  static struct option longopts[] =
  {
    { "auxilary",    0, 0, 'a' },
    { "binary",      0, 0, 'b' },
    { "file",        0, 0, 'f' },
    { "help",        0, 0, 'h' },
    { "optional",    0, 0, 'o' },
    { "processor",   1, 0, 'p' },
    { "section",     0, 0, 's' },
    { "symbol",      0, 0, 't' },
    { "version",     0, 0, 'v' },
    { 0, 0, 0, 0 }
  };

#define GETOPT_FUNC getopt_long(argc, argv, GET_OPTIONS, longopts, 0)

int main(int argc, char *argv[])
{
  extern char *optarg;
  extern int optind;
  int c;
  int usage = 0;
  char buffer[BUFSIZ];

  while ((c = GETOPT_FUNC) != EOF) {
    switch (c) {
    case '?':
    case 'h':
      usage = 1;
      break;
    case 'a':
      state.dump_flags |= PRINT_AUXREC;
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
    case 'p':
      if (strcasecmp(optarg, "pic12") == 0)
	state.processor = pic12;
      else if (strcasecmp(optarg, "pic14") == 0)
	state.processor = pic14;
      else {
	fprintf(stderr,
		"Error: unrecognised processor family \"%s\"\n",
		optarg);
        exit(1);
      }
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

  state.object = readobj(state.filename, &buffer[0]);
  if (state.object == NULL) {
    printf("%s\n", &buffer[0]);
    exit(1);
  }

  state.file   = readfile(state.filename, &buffer[0]);
  if (state.file == NULL) {
    printf("%s\n", &buffer[0]);
    exit(1);
  }
  
  if (state.dump_flags & PRINT_BINARY) {
    print_binary((unsigned char *)state.file->file, state.file->size);
  }

  if (state.dump_flags & PRINT_FILE_HEADER) {
    print_f_header(&state.object->file_header);
  }

  if (state.dump_flags & PRINT_OPT_HEADER) {
    print_opt_header(&state.object->opt_header);
  }

  if (state.dump_flags & PRINT_SECTIONS) {
    print_sec_list(state.object->sections, state.object->file_header.f_nscns);
  }

  if (state.dump_flags & PRINT_SYMTBL) {
    print_sym_table(state.object->symtbl, state.object->file_header.f_nsyms);
  }

  if (state.dump_flags & PRINT_AUXREC) {
    print_strtbl(state.object->strtbl);
  }

  return 0;

}
