/* Displays contents of ".COD" files
   Copyright (C) 2001, 2002, 2003, 2004
   Scott Dattalo

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
#include "gpvc.h"
#include "dump.h"
#include "block.h"

int number_of_source_files = 0;

char *substr(char *a, size_t sizeof_a, char *b, size_t n)
{
  strncpy(a, b, sizeof_a);
  if (n < sizeof_a)
    a[n] = 0;
  return a;

}

/* a little utility routine that's not actually used in the
   program, but was useful in computing offsets into structures. */

int ptr_offset(char *a, char *b)
{

  return(  ( (long int)a) - ((long int)b) );

}

/*
  fget_line - read a line from a file.
*/

char *fget_line(int line, char *s, int size, FILE *pFile)
{
  static FILE *plastFile=NULL;
  static int lastline=-1;
  static long lastPos=-1;

  if(!pFile)
    return NULL;

  /*
    If we read a line from the same file the last time we were called
    then see if we can take advantage of the file state:
   */
  if((pFile != plastFile) ||       /* if the file is the same */
     (line < (lastline-1)) ||      /* and the line is past the previous */
     (ftell(pFile) != lastPos) ) { /* and the file hasn't been touched */

    plastFile = pFile;
    lastline = 1;
    rewind(pFile);
    lastPos = -1;
  }

  while(line >= ++lastline) 
    fgets(s, size, plastFile);

  fgets(s, size, plastFile);

  lastPos = ftell(plastFile);

  return s;

}

/*

*/

void dump_hex(char *chunk, int length)
{

  int i,j,all_zero_line;

  printf("\n");

  i = 0;
  do {

    for(j=0, all_zero_line=1; j<16; j++)
      if(chunk[i+j]) all_zero_line = 0;

    if(all_zero_line)
      i+=16;
    else {
      printf("\n%04x:  ",i);

      for(j=0; j<16; j++)
	printf("%02x ",(unsigned char)chunk[i+j]);

      printf("   ");

      for(j=0; j<16; j++)
	if(isprint(chunk[i+j]))
	  putchar(chunk[i+j]);
	else
	  putchar('.');

      i+=16;
    }

  }while (i<length);

  printf("\n");

}

/*---------------------------------------------*/
/* 
 * ROM usage information
 */

void dump_memmap( void)
{
  typedef struct _REC_maptab {
    unsigned short start, last;
  } _REC_maptab;

  unsigned short _64k_base;
  DirBlockInfo *dbi;

  _REC_maptab *mt;
  unsigned short i,j,start_block,end_block;
  int first = 1;

  dbi = &main_dir;

  do {

    _64k_base = gp_getu16(&dbi->dir.block[COD_DIR_HIGHADDR]) << 16;
    start_block = gp_getu16(&dbi->dir.block[COD_DIR_MEMMAP]);

    if(start_block) {

      end_block   = gp_getu16(&dbi->dir.block[COD_DIR_MEMMAP+2]);

      if(first) {
	printf("\n\nROM Usage\n");
	printf("--------------------------------\n");
	first = 0;
      }
      for(j=start_block; j<=end_block; j++) {

	read_block(temp, j);

	mt = (_REC_maptab *)temp;

	for(i=0; i< 128; i++)
	  if( !((mt[i].start == 0) && (mt[i].last == 0) ))
	    printf("using ROM 0x%06x to 0x%06x\n",
		   addrsize*(_64k_base+mt[i].start)/2,
		   addrsize*(_64k_base+mt[i].last)/2  );
	  else if( i == 0 ) {
            /* The ROM Usage information in .cod files has a bug.
               If you declare only one word in a block at the very first 
               position then the .start and .last indicators are both zero, 
               just like they are for an empty block. So, let's read the 
               actual block and see if there's any information there... */
	    char t[COD_BLOCK_SIZE];
	    unsigned short index = gp_getu16(&dbi->dir.block[2*(COD_DIR_CODE)]);
	    if (index != 0) {
	      read_block(t, index);
	      if( gp_getu16(&t[0]) )
		printf("using ROM 0x%06x to 0x%06x\n",addrsize*_64k_base/2,addrsize*_64k_base/2);

	    }
	  }
      }
    } else if(first)
      printf("    No ROM usage information available.\n");
    
    dbi = dbi->next_dir_block_info;

  } while(dbi);
}

/*---------------------------------------------*/
/*
 * Dump all of the machine code in the .cod file 
 */

void dump_code(void)
{
  unsigned short _64k_base;
  unsigned short i,j,k,all_zero_line,index;
  DirBlockInfo *dbi;

  dump_memmap();

  printf("\n\nFormatted Code Dump\n");
  printf("-------------------\n");

  dbi = &main_dir;

  do {
    _64k_base = gp_getu16(&dbi->dir.block[COD_DIR_HIGHADDR]) << 15;
    for (k = 0; k <= 127; k++) {
      index = gp_getu16(&dbi->dir.block[2*(COD_DIR_CODE + k)]);
      if (index != 0) {
	read_block(temp, index);

	printf("\n");

	i = 0;
	do {

	  for(j=0, all_zero_line=1; j<8; j++)
	    if( gp_getu16(&temp[(i+j)*2]) ) all_zero_line = 0;

	  if(all_zero_line)
	    i+=8;
	  else {
	    printf("\n%06x:  ",addrsize*(_64k_base+i+k*256));

	    for(j=0; j<8; j++)
	      printf("%04x ",gp_getu16(&temp[2*i++]));
	  }

	}while (i<COD_BLOCK_SIZE/2);

	printf("\n");
      }
    }
    dbi = dbi->next_dir_block_info;

  } while(dbi);

}

/*---------------------------------------------*/
/*
 * Dump all of the (short) Symbol Table stuff in the .cod file 
 */


void dump_symbols( void )
{
#define SSYMBOL_SIZE  16
#define SYMBOLS_PER_BLOCK COD_BLOCK_SIZE/SSYMBOL_SIZE

#define SR_LEN       0
#define SR_NAME      1
#define SR_TYPE      13
#define SR_VALUE     14

  unsigned short i,j,start_block,end_block;
  char b[16];

  start_block = gp_getu16(&directory_block_data[COD_DIR_SYMTAB]);

  if(start_block) {

    end_block = gp_getu16(&directory_block_data[COD_DIR_SYMTAB+2]);

    printf("\nSymbol Table Information\n");
    printf("------------------------\n\n");

    for(j=start_block; j<=end_block; j++) {

      read_block(temp, j);

      for(i=0; i<SYMBOLS_PER_BLOCK; i++) {

	if(temp[i*SSYMBOL_SIZE + SR_NAME])
	  printf("%s = %x, type = %s\n",
		 substr(b, sizeof(b), &temp[i*SSYMBOL_SIZE + SR_NAME],12), 
		 gp_getu16(&temp[i*SSYMBOL_SIZE + SR_VALUE]), 
		 SymbolType4[(unsigned char)temp[i*SSYMBOL_SIZE + SR_TYPE]]
		 );
      }
    }
  }
  else
    printf("No symbol table info\n");

}

/*---------------------------------------------*/

/*
 * Dump all of the Long Symbol Table stuff in the .cod file 
 */

void dump_lsymbols( void )
{
  char *s,length;
  short type;
  unsigned short i,j,start_block,end_block, value;
  char b[256];

  start_block = gp_getu16(&directory_block_data[COD_DIR_LSYMTAB]);

  if(start_block) {

    end_block = gp_getu16(&directory_block_data[COD_DIR_LSYMTAB+2]);

    printf("\nLong Symbol Table Information\n");
    printf("------------------------\n\n");

    for(j=start_block; j<=end_block; j++) {

      read_block(temp, j);

      for(i=0; i<COD_BLOCK_SIZE;) {
	s =  &temp[i];

	if(*s==0)
	  break;

	length = *s;
	type  = *(short *)&s[length+1];
	if(type>128)
	  type = 0;
	/* read big endian */
	value = gp_getb32(&s[length+3]);

	printf("%s = %x, type = %s\n",
	       substr(b, sizeof(b), &s[1],length),
	       value,
	       SymbolType4[type]);
	i += (length + 7);
      }
    }
  }else
      printf("No long symbol table info\n");

}

/*---------------------------------------------*/
/*
 * Source files
 */

void dump_source_files( void )
{
#define FILE_SIZE  64
#define FILES_PER_BLOCK COD_BLOCK_SIZE/FILE_SIZE

  unsigned short i,j,start_block,end_block,offset;
  char b[FILE_SIZE];

  start_block = gp_getu16(&directory_block_data[COD_DIR_NAMTAB]);

  if(start_block) {

    end_block = gp_getu16(&directory_block_data[COD_DIR_NAMTAB+2]);

    printf("\nSource File Information\n");
    printf("------------------------\n\n");

    for(j=start_block; j<=end_block; j++) {

      read_block(temp, j);

      for(i=0; i<FILES_PER_BLOCK; i++) {

	offset = i*FILE_SIZE;
	substr(b, sizeof(b), &temp[offset+1],FILE_SIZE);

	if(temp[offset]) {
	  source_file_names[number_of_source_files] = strdup(b);
	  if (!source_file_names[number_of_source_files]) {
	    fprintf(stderr, " system error\n");
	    exit(1);
          }
	  printf("%s\n",source_file_names[number_of_source_files]);
	  source_files[number_of_source_files] = 
	    fopen(source_file_names[number_of_source_files],"rt");
	  if (source_files[number_of_source_files] == NULL) {
	    perror(source_file_names[number_of_source_files]);
	    exit(1);
	  }
	  number_of_source_files++;
	  if (number_of_source_files >= MAX_SOURCE_FILES) {
	    fprintf(stderr, " too many source files; increase MAX_SOURCE_FILES and recompile\n");
            exit(1);
	  }
	}
      }
    }
  }else
    printf("No source file info\n");
}

char * smod_flags(int smod)
{
  static char f[9];

  f[0] = (smod & 0x80) ? 'C':'.';
  f[1] = (smod & 0x40) ? 'F':'.';
  f[2] = (smod & 0x20) ? 'I':'.';
  f[3] = (smod & 0x10) ? 'D':'.';
  f[4] = (smod & 0x08) ? 'C':'.';
  f[5] = (smod & 0x04) ? 'L':'.';
  f[6] = (smod & 0x02) ? 'N':'.';
  f[7] = (smod & 0x01) ? 'A':'.';
  f[8] = 0;

  return(f);
}

/*---------------------------------------------*/
/*
 * Line number info from the source files
 */

void dump_line_symbols(void)
{
  static int lst_line_number = 1;
  static int last_src_line = 0;
  char buf[2048];
  unsigned short i,j,start_block,end_block;

  LineSymbol *ls;

  start_block = gp_getu16(&directory_block_data[COD_DIR_LSTTAB]);

  if(start_block) {

    end_block = gp_getu16(&directory_block_data[COD_DIR_LSTTAB+2]);

    printf("\n\nLine Number Information\n");
    printf(" LstLn  SrcLn  Addr   Flags         FileName\n");
    printf(" -----  -----  ----   -----------   ---------------------------------------\n");

    for(j=start_block; j<=end_block; j++) {

      read_block(temp, j);

      ls = (LineSymbol *) temp;

      for(i=0; i<84; i++) {

	if((ls[i].smod & 4) == 0) {
	  assert(ls[i].sfile < number_of_source_files);
	  printf(" %5d  %5d %06X   %2x %s   %-50s\n",
		 lst_line_number++,
		 ls[i].sline,
		 ls[i].sloc*addrsize,
		 ls[i].smod,
		 smod_flags(ls[i].smod),
		 source_file_names[ls[i].sfile]);
	}
	if(source_files[ls[i].sfile] && (ls[i].sline != last_src_line)) {
	  /*fgets(buf, sizeof(buf), source_files[ls[i].sfile]);*/
	  fget_line(ls[i].sline, buf, sizeof(buf), source_files[ls[i].sfile]);
	  printf("%s",buf);
	}
	last_src_line = ls[i].sline;
      }
    }
  } else
    printf("No line number info\n");
}
/*---------------------------------------------*/
/*
 * Debug Message area
 */

void dump_message_area(void)
{
#define MAX_STRING_LEN  255 /* Maximum length of a debug message */
  char DebugType,DebugMessage[MAX_STRING_LEN];

  unsigned short i,j,start_block,end_block, len;
  unsigned short laddress;

  start_block = gp_getu16(&directory_block_data[COD_DIR_MESSTAB]);

  if(start_block) {

    end_block = gp_getu16(&directory_block_data[COD_DIR_MESSTAB+2]);

    printf("\n\nDebug Message area\n");
    printf("     Addr  Cmd  Message\n");
    printf(" --------  ---  -------------------------------------\n");

    for(i=start_block; i<=end_block; i++) {
      read_block(temp, i);
    
      j = 0;

      while (j < 504) {

	/* read big endian */
	laddress = gp_getb32(&temp[j]);
        
        j += 4;

	DebugType = temp[j++];

	if (DebugType == 0) {
	  break;
	}

	len = temp[j++];

	substr(DebugMessage, sizeof(DebugMessage), &temp[j], MAX_STRING_LEN);
	j += strlen(DebugMessage);

	printf(" %8x    %c  %s\n",laddress, DebugType, DebugMessage);
      }
    }
  } else
    printf("    No Debug Message information available.\n");
    
}
/*---------------------------------------------*/
/* 
 * Display the local symbol table information
 */

void dump_local_vars(void)
{
#define SSYMBOL_SIZE  16
#define SYMBOLS_PER_BLOCK COD_BLOCK_SIZE/SSYMBOL_SIZE

  typedef struct symbol_record {
    char name_strlen;
    char name[12];         /* Not Symbol Length because */
    unsigned char stype;   /* this record is 16 bytes   */
    unsigned short svalue;
  } symbol_record;

  typedef struct scope_head {
    char name_strlen;
    char name[7];
    unsigned int start;
    unsigned int stop;
  } scope_head;

  symbol_record *sr;
  scope_head  *sh;

  unsigned short i,j,start_block,end_block;

  j=0;

  start_block = gp_getu16(&directory_block_data[COD_DIR_LOCALVAR]);

  if(start_block) {

    end_block = gp_getu16(&directory_block_data[COD_DIR_LOCALVAR+2]);

    printf("\n\nLocal Symbol Scoping Information\n");
    printf("--------------------------------\n");

    for(i=start_block; i<=end_block; i++) {
      read_block(temp, i);

      for(i=0; i<SYMBOLS_PER_BLOCK; i++) {
	sh = (scope_head *)&temp[i*SSYMBOL_SIZE];

	if(sh->name[0]) {
	  if( 0 == strncmp(sh->name,"__LOCAL",7)) {
	    printf("Local symbols between %06x and %06x:  ",sh->start,sh->stop);
	  } else {
	    sr = (symbol_record *)sh;
	    printf("%s = 0x%x, type = %s\n",sr->name, sr->svalue, SymbolType4[sr->stype]);
	  }
	}
      }
    }
  }else
    printf("    No local variable scoping info available.\n");

}

