/* ".COD" file output for gpasm
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Scott Dattalo

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
#include "gpasm.h"
#include "lst.h"
#include "cod.h"

static DirBlockInfo main_dir;
static int cod_lst_line_number = 0;
static int blocks = 0;

static void
init_DirBlock(DirBlockInfo *a_dir)
{
  int i;

  /* create space for the directory block: */

  gp_cod_create(&a_dir->dir, &blocks);

  a_dir->next_dir_block_info = NULL;

  /* The code blocks associated with this directory
     do not yet exist. */
  for(i=0; i<COD_CODE_IMAGE_BLOCKS; i++) {
    a_dir->cod_image_blocks[i].block = NULL;
    a_dir->cod_image_blocks[i].block_number = 0;
  }

  /* Initialize the directory block with known data. It'll be written
   * to the .cod file after everything else */
  gp_cod_strncpy(&a_dir->dir.block[COD_DIR_SOURCE], 
	         state.srcfilename,
	         COD_DIR_DATE - COD_DIR_SOURCE);
  gp_cod_date(&a_dir->dir.block[COD_DIR_DATE], 
              COD_DIR_TIME - COD_DIR_DATE);
  gp_cod_time(&a_dir->dir.block[COD_DIR_TIME], 
              COD_DIR_VERSION - COD_DIR_TIME);
  gp_cod_strncpy(&a_dir->dir.block[COD_DIR_VERSION], 
	         VERSION,
	         COD_DIR_COMPILER - COD_DIR_VERSION);
  gp_cod_strncpy(&a_dir->dir.block[COD_DIR_COMPILER], 
	         "gpasm",
	         COD_DIR_NOTICE - COD_DIR_COMPILER);
  gp_cod_strncpy(&a_dir->dir.block[COD_DIR_NOTICE], 
	         GPUTILS_COPYRIGHT_STRING,
	         COD_DIR_SYMTAB - COD_DIR_NOTICE);

  /* The address is always two shorts or 4 bytes long */
  gp_putl16(&a_dir->dir.block[COD_DIR_ADDRSIZE], 4);

}

/*
 * init_cod - initialize the cod file
 */

void
cod_init(void)
{

  if (state.codfile != named) {
    strncpy(state.codfilename, state.basefilename, sizeof(state.codfilename));
    strncat(state.codfilename, ".cod", sizeof(state.codfilename));  
  }

  if (state.codfile == suppress) {
    state.cod.f = NULL;
    state.cod.enabled = false;
    unlink(state.codfilename);
  } else {
    state.cod.f = fopen(state.codfilename, "wb");
    if (state.cod.f == NULL) {
      perror(state.codfilename);
      exit(1);
    }
    state.cod.enabled = true;
  }

  if(!state.cod.enabled)
    return;

  init_DirBlock(&main_dir);
  assert(main_dir.dir.block_number == 0);

  fseek(state.cod.f, COD_BLOCK_SIZE, SEEK_SET);

}

/*
 * write_cod_block - write a cod block to the .cod file and adjust
 * the cod block ptrs in the directory block.
 */

static void
write_cod_block(DirBlockInfo *dbp, 
                int block_ptr_start, 
                int block_ptr_end, 
                Block *bptr)
{

  /* most of the cod blocks have a 'start' and 'end' pointer in the
   * directory block. These pointers are 16 bits wide. If the start
   * pointer is zero, then this is the first time a block of this
   * type has been written. In this case, the block pointer is written  
   * to both the start and end pointer locations. 
   */

  if(!gp_getl16(&dbp->dir.block[block_ptr_start]))
    gp_putl16(&dbp->dir.block[block_ptr_start], bptr->block_number);

  gp_putl16(&dbp->dir.block[block_ptr_end], bptr->block_number);

  fseek(state.cod.f, COD_BLOCK_SIZE*bptr->block_number, SEEK_SET);
  fwrite(bptr->block, 1, COD_BLOCK_SIZE, state.cod.f);

}

/*
 * write_file_block - write a code block that contains a list of the
 * source files.
 */
static void
write_file_block(void)
{
  Block fb;
#define FILES_PER_BLOCK COD_BLOCK_SIZE/COD_FILE_SIZE
  int id_number=0;

  struct file_context *fc;

  gp_cod_create(&fb, &blocks);

  if(!state.files)
    return;

  /* Find the head of the file list: */

  fc = state.files;
  while(fc->prev && id_number++ < 1000) {
    fc = fc->prev;
  }

  if(id_number>=1000) {
    /* Too many files to handle in the .cod file */
    assert(0);
  }

  id_number = 0;

  while(fc != NULL) {

    /* The file id is used to define the index at which the file
     * name is written within the file code block. (The id's are
     * sequentially assigned when the files are opened.) If there
     * are too many files, then gpasm will abort. note: .cod files 
     * can handle larger file lists...
     */

    gp_cod_strncpy(&fb.block[1 + COD_FILE_SIZE * id_number],
		   fc->name,
		   COD_FILE_SIZE-1);

    id_number++;

    if(id_number >= FILES_PER_BLOCK) {
      write_cod_block(&main_dir, COD_DIR_NAMTAB, COD_DIR_NAMTAB+2, &fb);
      id_number = 0;
      gp_cod_next(&fb, &blocks);
    }

    fc = fc->next;

  }

  if(id_number)
    write_cod_block(&main_dir, COD_DIR_NAMTAB, COD_DIR_NAMTAB+2, &fb);

  gp_cod_delete(&fb);

}

/* cod_lst_line - add a line of information that cross references the
 * the opcode's address, the source file, and the list file.
 */

void
cod_lst_line(int line_type)
{
#define COD_LST_FIRST_LINE  7
  unsigned char smod_flag = 0xff;
  static int first_time = 1;
  static Block lb={NULL,0};
  
  int offset;

  if(!state.cod.enabled)
    return;

  switch(line_type) {
  case COD_FIRST_LST_LINE:
  case COD_NORMAL_LST_LINE:

    /* Don't start until after the source is open */
    if(state.src == NULL)
      return;

    /* If we don't have a block yet then create one: */
    if(lb.block == NULL)
      gp_cod_create(&lb, &blocks);

    /* Ignore the first few line numbers */
    if(state.lst.line_number < COD_LST_FIRST_LINE)
      return;

    if(cod_lst_line_number >= COD_MAX_LINE_SYM) {
      write_cod_block(&main_dir, COD_DIR_LSTTAB, COD_DIR_LSTTAB+2, &lb);
      cod_lst_line_number = 0;
      gp_cod_next(&lb, &blocks);
    }

    assert(state.src->fc != NULL);
    offset = cod_lst_line_number++ * COD_LINE_SYM_SIZE;
    lb.block[offset + COD_LS_SFILE] = state.src->fc->id;

    if(state.cod.emitting != 0)
      smod_flag = 0x080;
    else
      smod_flag = 0x90;

    if(first_time != 0) {
      first_time = 0;
      smod_flag  = 0xff;
    }

    lb.block[offset + COD_LS_SMOD] = smod_flag;

    /* Write the source file line number corresponding to the list file line 
       number */
    gp_putl16(&lb.block[offset + COD_LS_SLINE], state.src->line_number);

    /* Write the address of the opcode. */
    gp_putl16(&lb.block[offset + COD_LS_SLOC],
              gp_processor_byte_to_org(state.device.class, state.lst.line.was_org));

    break;
  case COD_LAST_LST_LINE:
    if(lb.block) {
      write_cod_block(&main_dir, COD_DIR_LSTTAB, COD_DIR_LSTTAB+2, &lb);
      gp_cod_delete(&lb);
    }

    break;
  default:
    assert(0);
  }

}

/* cod_write_symbols - write the symbol table to the .cod file
 *
 * This routine will read the symbol table that gpasm has created
 * and convert it into a format suitable for .cod files. So far, only
 * three variable types are supported: address, register, and constants.
 *
 */

void
cod_write_symbols(struct symbol **symbol_list, int num_symbols)
{
  /* Each symbol is written as a dynamically sized structure to the 
   * .cod file. Its format is like this:
   * position  0               length of the symbol name
   * positions 1 to len        the symbol name
   * positions len+1 & len+2   Type of symbol (16 bits)
   * positions len+3 to len+7  Value of symbol
   */
#define COD_SYM_TYPE  1   /* type info is 1 byte after the length */
#define COD_SYM_VALUE 3   /* value info is 3 bytes after the length */
#define COD_SYM_EXTRA 7   /* symbol name length + 7 is total structure size */
#define MAX_SYM_LEN   255 /* Maximum length of a symbol name */

  int i,offset,len,type;
  struct variable *var;
  const char * s;
  Block sb;

  if(!state.cod.enabled)
    return;

  gp_cod_create(&sb, &blocks);

  offset = 0;

  for(i=0; i<num_symbols; i++) {
    int value;
    var = get_symbol_annotation(symbol_list[i]);
    s = get_symbol_name(symbol_list[i]);
    len = strlen(s);

    /* If this symbol extends past the end of the cod block
     * then write this block out */

    if((offset + len + COD_SYM_EXTRA) >= COD_BLOCK_SIZE) {
      write_cod_block(&main_dir, COD_DIR_LSYMTAB, COD_DIR_LSYMTAB+2, &sb);
      gp_cod_next(&sb, &blocks);
      offset = 0;
    }

    gp_cod_strncpy(&sb.block[offset +1], s, MAX_SYM_LEN);

    value = var->value;
    switch(var->type) {
    case gvt_cblock:
      type = COD_ST_C_SHORT;  /* byte craft's nomenclature for a memory byte. */
      break;
    case gvt_address:
      type = COD_ST_ADDRESS;
      /* FIXME: gvt_org never used, should it be and should the value
	 be divided by two there? */
      value /= 2;
      break;
    case gvt_org:
      type = COD_ST_ADDRESS;
      break;
    case gvt_constant:
    default:
      type = COD_ST_CONSTANT;
    }

    gp_putl16(&sb.block[offset+len+COD_SYM_TYPE], type);

    /* write 32 bits, big endian */
    gp_putb32(&sb.block[offset+len+COD_SYM_VALUE], value);
    offset += (len+COD_SYM_EXTRA);
  }

  if(offset)
    write_cod_block(&main_dir, COD_DIR_LSYMTAB, COD_DIR_LSYMTAB+2, &sb);

  gp_cod_delete(&sb);
}

/* cod_emit_opcode - write one opcode to a cod_image_block
 */
static void
cod_emit_opcode(int address,int opcode)
{
  DirBlockInfo *dbi;
  int block_index;
  int found;
  int _64k_base;
  unsigned char * block;

  if(!state.cod.enabled)
    return;

  /* The code image blocks are handled in a different manner than the
   * other cod blocks. In theory, it's possible to emit opcodes in a
   * non-sequential manner. Furthermore, it's possible that there may
   * be gaps in the program memory. These cases are handled by an array
   * of code blocks. The lower 8 bits of the opcode's address form an
   * index into the code block, while bits 9-15 are an index into the
   * array of code blocks. The code image blocks are not written until
   * all of the opcodes have been emitted.
   */

  block_index = (address >> COD_BLOCK_BITS) & (COD_CODE_IMAGE_BLOCKS -1); 
  _64k_base = (address >> 16) & 0xffff;


  dbi = &main_dir;

  /* find the directory containing this 64k segment */
  found = 0;
  do {
    if(gp_getl16(&dbi->dir.block[COD_DIR_HIGHADDR]) == _64k_base)
      found = 1;
    else {
    
      /* If the next directory block (in the linked list of directory
         blocks) is NULL, then this is the first time to encounter this
         _64k segment. So we need to create a new segment. */
      if(dbi->next_dir_block_info == NULL) {
	dbi->next_dir_block_info  = malloc(sizeof(DirBlockInfo));
	init_DirBlock(dbi->next_dir_block_info);
	gp_putl16(&dbi->dir.block[COD_DIR_NEXTDIR], 
		  dbi->next_dir_block_info->dir.block_number);
	gp_putl16(&dbi->next_dir_block_info->dir.block[COD_DIR_HIGHADDR], 
		  _64k_base);
	found = 1;
      }

      dbi = dbi->next_dir_block_info;
    }
  }
  while(!found);

  if(dbi->cod_image_blocks[block_index].block == NULL) {
    gp_cod_create(&dbi->cod_image_blocks[block_index], &blocks);
  }

  block = dbi->cod_image_blocks[block_index].block;

  gp_putl16(&block[address & (COD_BLOCK_SIZE - 1)], opcode);

}

static void
write_cod_range_block(unsigned int address, Block *rb)
{

  DirBlockInfo *dbi = &main_dir;
  unsigned int _64k_base;

  _64k_base = (address >> 16) & 0xffff;

  do {

    if(gp_getl16(&dbi->dir.block[COD_DIR_HIGHADDR]) == _64k_base) {
      write_cod_block(dbi, COD_DIR_MEMMAP, COD_DIR_MEMMAP+2, rb);
      return;
    }
    dbi = dbi->next_dir_block_info;

  }  while(dbi);

  assert(0);

}

/* cod_write_code - write all of the assembled pic code to the .cod file
 */
static void
cod_write_code(void)
{
  MemBlock *m = state.i_memory;
  int mem_base;

  int i,offset;
  int start_address = 0, used_flag = 0;
  DirBlockInfo *dbi;
  Block rb = {NULL, 0};

  offset = 0;

  while(m) {
    mem_base = m->base << I_MEM_BITS;

    for(i=mem_base; (i-mem_base) <= MAX_I_MEM; i += 2) {
      unsigned short insn;
      if (i - mem_base < MAX_I_MEM &&
	  state.device.class->i_memory_get(state.i_memory, i, &insn)) {
	cod_emit_opcode(i, insn);
	if(used_flag == 0) {
          /* Save the start address in a range of opcodes */
          start_address = i;
	  used_flag = 1;
	  if(rb.block == NULL) {
	    gp_cod_create(&rb, &blocks);
	  }
	}
      } 
      else {

	/* No code at address i, but we need to check if this is the
	   first empty address after a range of address. */
	if(used_flag == 1) {

	  /* We need to update dir map indicating a range of memory that
	     is needed. This is done by writing the start and end address to
	     the directory map. */
	  gp_putl16(&rb.block[offset], start_address);
	  gp_putl16(&rb.block[offset+2], i-1);

	  offset += 4;
	  if(offset>=COD_BLOCK_SIZE) {
	    /* If there are a whole bunch of non-contiguous pieces of 
	       code then we'll get here. But most pic apps will only need
	       one directory block (that will give you 64 ranges or non-
	       contiguous chunks of pic code). */
	    write_cod_range_block(start_address, &rb);
	    gp_cod_delete(&rb);
	    offset = 0;
	  }
	  used_flag = 0;
	}
      }
    }

    if(offset) {
      write_cod_range_block(start_address, &rb);
      gp_cod_delete(&rb);
      offset = 0;
      used_flag = 0;
    }

    m = m->next;
  }

  /* write the code image blocks */

  dbi = &main_dir;
  do {
    for(i=0; i<COD_CODE_IMAGE_BLOCKS; i++)
      if(dbi->cod_image_blocks[i].block) {
	write_cod_block(dbi,
                        COD_DIR_CODE+i*2,
                        COD_DIR_CODE+i*2,
                        &dbi->cod_image_blocks[i]);
	free(dbi->cod_image_blocks[i].block);
      }
    dbi = dbi->next_dir_block_info;
  } while(dbi);

}

static void
write_directory(void)
{
  DirBlockInfo *dbi;
  dbi = &main_dir;

  do {
    fseek(state.cod.f,COD_BLOCK_SIZE * dbi->dir.block_number, SEEK_SET);
    fwrite(dbi->dir.block, 1, COD_BLOCK_SIZE, state.cod.f);

    dbi = dbi->next_dir_block_info;
  } while(dbi);
}

void
cod_close_file(void)
{

  if(!state.cod.enabled)
    return;

  cod_lst_line(COD_LAST_LST_LINE);

  write_file_block();

  cod_write_code();

  gp_cod_strncpy(&main_dir.dir.block[COD_DIR_PROCESSOR], 
	         gp_processor_name(state.processor, 2),
	         COD_DIR_LSYMTAB - COD_DIR_PROCESSOR);

  write_directory();
  fclose(state.cod.f);
  free(main_dir.dir.block);
}

