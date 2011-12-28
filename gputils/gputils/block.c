/* Displays contents of ".COD" files
   Copyright (C) 2001, 2002, 2003, 2004, 2005
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

void directory_block(void)
{
  char temp_buf[256];
  unsigned char *block;
  char *processor_name;
  int time;
  int bytes_for_address;

  block = main_dir.dir.block;

  printf("directory block \n");

  printf("COD file version  %d\n",
         gp_getl16(&block[COD_DIR_CODTYPE]));
  printf("Source file       %s\n",
         &block[COD_DIR_SOURCE]);
  printf("Date              %s\n",
         substr(temp_buf, sizeof(temp_buf), &block[COD_DIR_DATE],7));

  time = gp_getl16(&block[COD_DIR_TIME]);

  printf("Time              %02d:%02d\n", time / 100, time % 100);
  printf("Compiler version  %s\n",
         substr(temp_buf, sizeof(temp_buf), &block[COD_DIR_VERSION],19));
  printf("Compiler          %s\n",
         substr(temp_buf, sizeof(temp_buf), &block[COD_DIR_COMPILER],12));
  printf("Notice            %s\n",
         substr(temp_buf, sizeof(temp_buf), &block[COD_DIR_NOTICE],64));

  processor_name = substr(temp_buf,
                          sizeof(temp_buf),
                          &block[COD_DIR_PROCESSOR],
                          8);
  printf("Processor         %s\n", processor_name);

  bytes_for_address = gp_getl16(&block[COD_DIR_ADDRSIZE]);
  printf("Bytes for address: %d\n", bytes_for_address);
  if (bytes_for_address != 4) {
    printf("WARNING: address size looks suspicious\n");
  }

  printf("High word of 64k address %04x\n",
         gp_getl16(&block[COD_DIR_HIGHADDR]));

  printf("Short symbol table start block:  0x%04x  end block:  0x%04x\n",
         gp_getl16(&block[COD_DIR_SYMTAB]),
         gp_getl16(&block[COD_DIR_SYMTAB+2]));
  printf("Long symbol table start block:   0x%04x  end block:  0x%04x\n",
         gp_getl16(&block[COD_DIR_LSYMTAB]),
         gp_getl16(&block[COD_DIR_LSYMTAB+2]));
  printf("File name table start block:     0x%04x  end block:  0x%04x\n",
         gp_getl16(&block[COD_DIR_NAMTAB]),
         gp_getl16(&block[COD_DIR_NAMTAB+2]));
  printf("Source info table start block:   0x%04x  end block:  0x%04x\n",
         gp_getl16(&block[COD_DIR_LSTTAB]),
         gp_getl16(&block[COD_DIR_LSTTAB+2]));
  printf("Rom table start block:           0x%04x  end block:  0x%04x\n",
         gp_getl16(&block[COD_DIR_MEMMAP]),
         gp_getl16(&block[COD_DIR_MEMMAP+2]));
  printf("Local scope table start block:   0x%04x  end block:  0x%04x\n",
         gp_getl16(&block[COD_DIR_LOCALVAR]),
         gp_getl16(&block[COD_DIR_LOCALVAR+2]));
  printf("Debug messages start block:      0x%04x  end block:  0x%04x\n",
         gp_getl16(&block[COD_DIR_MESSTAB]),
         gp_getl16(&block[COD_DIR_MESSTAB+2]));

  printf("\nNext directory block");
  if(gp_getl16(&block[COD_DIR_NEXTDIR]))
    printf(":  %d\n",gp_getl16(&block[COD_DIR_NEXTDIR]));
  else
    printf(" is empty\n");

  /*
    Uncomment this section to get the address offsets within the directory structure
    NOTE: the offsets are compiler dependent. For gcc, use the -fpack-struct
    option to pack the structures with no gaps.

    printf("notice_strlen address %d\n",ptr_offset((char *)&dir->notice_strlen,block) );
    printf("notice address %d\n",ptr_offset((char *)&dir->notice,block) );
    printf("symtab address %d\n",ptr_offset((char *)&dir->symtab,block) );
    printf("processor address %d\n",ptr_offset((char *)&dir->processor,block) );
    printf("AddrSize address %d\n",ptr_offset((char *)&dir->AddrSize,block) );
    printf("HighAddr address %d\n",ptr_offset((char *)&dir->HighAddr,block) );
    printf("NextDir address %d\n",ptr_offset((char *)&dir->NextDir,block) );
    printf("memmap address %d\n",ptr_offset((char *)&dir->MemMapOFS,block) );
    printf("lsymtab address %d\n",ptr_offset((char *)&dir->Lsymtab,block) );
  */


}

void read_block(unsigned char * block, int block_number)
{

  fseek(codefile, block_number * COD_BLOCK_SIZE, SEEK_SET);
  fread(block, COD_BLOCK_SIZE, 1, codefile);
}

void create_block(Block *b)
{

  assert(b != NULL);

  b->block = malloc(COD_BLOCK_SIZE);
  gp_cod_clear(b);

}

void read_directory(void)
{
  DirBlockInfo *dbi;

  create_block(&main_dir.dir);
  read_block(main_dir.dir.block, 0);

  dbi = &main_dir;

  do {
    int next_dir_block = gp_getl16(&dbi->dir.block[COD_DIR_NEXTDIR]);

    if(next_dir_block) {
      dbi->next_dir_block_info = (DirBlockInfo *)malloc(sizeof(DirBlockInfo));
      dbi = dbi->next_dir_block_info;
      create_block(&dbi->dir);
      read_block(dbi->dir.block, next_dir_block);
    } else {
      dbi->next_dir_block_info = NULL;
      return;
    }
  } while(1);
}
