/* Symbol table support
   Copyright (C) 1998,1999,2000,2001 James Bowman, Scott Dattalo

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

#include "gpasm.h"
#include "gpsymbol.h"
#include "lst.h"
#include "gpcod.h"
#include "cod.h"

DirBlockInfo main_dir;
static int cod_lst_line_number = 0;
static int blocks = 0;
extern int _16bit_core;

/************************************************************************/

/* copy a string to a cod block using the pascal convention, 
 * i.e. the string length occupies the first string location
 */

void cod_strncpy(char *dest, char *src, int max_len)
{


  *(dest-1) = ( (max_len>strlen(src)) ? strlen(src) : max_len );
  strncpy(dest, src, *(dest-1));

}

/*
 * get_short_int, put_short_int, and put_int
 * utility routines for accessing the cod blocks (char *)
 * using the proper endianess. (Using a union would be
 * cleaner, but less portable.
 */

int get_short_int(char * buff)
{
  return ( buff[0] + (buff[1] << 8));
}

void put_short_int(char * buff, int value)
{

  buff[0] = value & 0xff;
  buff[1] = (value >> 8) & 0xff;
}

void put_int(char * buff, int value)
{

  buff[0] = value & 0xff;
  buff[1] = (value >> 8) & 0xff;
  buff[2] = (value >> 16) & 0xff;
  buff[3] = (value >> 24) & 0xff;
}

/* reverse the endian type (big to little or little to big)
   note: there's probably a wonderful macro nestled deep in
   the bowels of the os that nicely handles this conversion... */

unsigned int endian_swap(unsigned int a)
{

  return
    ( (a>>24) & 0x000000ff) | 
    ( (a>>8)  & 0x0000ff00) |
    ( (a<<8)  & 0x00ff0000) |
    ( (a<<24) & 0xff000000);

}


/*
 * clear_cod_block - write zeroes to a code block, unless the
 * code block ptr is null.
 */
void clear_cod_block(Block *b)
{
  int i;

  if(b && b->block)
    for(i=0; i<COD_BLOCK_SIZE; i++)
      b->block[i] = 0;
  else
    assert(0);
}

void create_block(Block *b)
{

  assert(b != NULL);

  b->block = malloc(COD_BLOCK_SIZE);
  clear_cod_block(b);
  b->block_number = blocks++;

  
}

void delete_block(Block *b)
{

  if(b && b->block) {
    free(b->block);
    b->block = NULL;
  }
  else
    assert(0);

}

void get_next_block(Block *b)
{
  assert(b != NULL);
  clear_cod_block(b);
  b->block_number = blocks++;
}
void init_DirBlock(DirBlockInfo *a_dir)
{
  int i;

  /* create space for the directory block: */

  create_block(&a_dir->dir);

  a_dir->next_dir_block_info = NULL;

  /* The code blocks associated with this directory
     do not yet exist. */
  for(i=0; i<COD_CODE_IMAGE_BLOCKS; i++) {
    a_dir->cod_image_blocks[i].block = NULL;
    a_dir->cod_image_blocks[i].block_number = 0;
  }

  /* Initialize the directory block with known data. It'll be written
   * to the .cod file after everything else */

  cod_strncpy(&a_dir->dir.block[COD_DIR_VERSION], 
	      GPASM_VERSION_STRING,
	      COD_DIR_COMPILER - COD_DIR_VERSION);
  cod_strncpy(&a_dir->dir.block[COD_DIR_COMPILER], 
	      "gpasm",
	      COD_DIR_NOTICE - COD_DIR_COMPILER);
  cod_strncpy(&a_dir->dir.block[COD_DIR_NOTICE], 
	      "Copyright (c) 1998-2001 gnupic project",
	      COD_DIR_SYMTAB - COD_DIR_NOTICE);

  put_short_int(&a_dir->dir.block[COD_DIR_ADDRSIZE], 1 << _16bit_core);
  
}

/*
 * init_cod - initialize the cod file
 */

void cod_init(void)
{

  switch (state.codfile) {
  case suppress:
    state.cod.f = NULL;
    state.cod.enabled = 0;
    break;
  case normal:
    strcpy(state.codfilename, state.basefilename);
    strcat(state.codfilename, ".cod");
  case named:
    /* Don't need to do anything - name is already set up */
    state.cod.f = fopen(state.codfilename, "w");
    if (state.cod.f == NULL) {
      perror(state.codfilename);
      exit(1);
    }
    add_file(ft_cod,state.codfilename);
    state.cod.enabled = 1;
  }

  if(!state.cod.enabled)
    return;

  init_DirBlock(&main_dir);
  assert(main_dir.dir.block_number == 0);

  fseek(state.cod.f,COD_BLOCK_SIZE, SEEK_SET);

}

/*
 * write_cod_block - write a cod block to the .cod file and adjust
 * the cod block ptrs in the directory block.
 */

void write_cod_block(DirBlockInfo *dbp, int block_ptr_start, int block_ptr_end, Block *bptr)
{

  /* most of the cod blocks have a 'start' and 'end' pointer in the
   * directory block. These pointers are 16 bits wide. If the start
   * pointer is zero, then this is the first time a block of this
   * type has been written. In this case, the block pointer is written  
   * to both the start and end pointer locations. 
   */

  if(!get_short_int(&dbp->dir.block[block_ptr_start]))
    put_short_int(&dbp->dir.block[block_ptr_start], bptr->block_number);

  put_short_int(&dbp->dir.block[block_ptr_end], bptr->block_number);

  fseek(state.cod.f,COD_BLOCK_SIZE*bptr->block_number, SEEK_SET);
  fwrite(bptr->block, 1, COD_BLOCK_SIZE, state.cod.f);

}

/*
 * write_file_block - write a code block that contains a list of the
 * source files.
 */
void write_file_block(void)
{
  Block fb;
#define FILES_PER_BLOCK COD_BLOCK_SIZE/COD_FILE_SIZE
  int id_number=0;

  struct file_context *fc;

  create_block(&fb);

  if(!state.files)
    return;

  /* Find the head of the file list: */

  fc = state.files->prev;
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

    cod_strncpy(&fb.block[1 + COD_FILE_SIZE * id_number],
		fc->name,
		COD_FILE_SIZE-1);

    id_number++;

    if(id_number >= FILES_PER_BLOCK) {
      write_cod_block(&main_dir,COD_DIR_NAMTAB,COD_DIR_NAMTAB+2,&fb);
      id_number = 0;
      get_next_block(&fb);
    }

    fc = fc->next;

  }

  if(id_number)
    write_cod_block(&main_dir,COD_DIR_NAMTAB,COD_DIR_NAMTAB+2,&fb);

  delete_block(&fb);

}

/* cod_lst_line - add a line of information that cross references the
 * the opcode's address, the source file, and the list file.
 */

void cod_lst_line(int line_type)
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
      create_block(&lb);

    /* Ignore the first few line numbers */
    if(state.lst.line_number < COD_LST_FIRST_LINE)
      return;

    if(cod_lst_line_number >= COD_MAX_LINE_SYM) {
      write_cod_block(&main_dir,COD_DIR_LSTTAB,COD_DIR_LSTTAB+2,&lb);
      cod_lst_line_number = 0;
      get_next_block(&lb);
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

    lb.block[offset + COD_LS_SMOD]  = smod_flag;

    /* Write the source file line number corresponding to the list file line number */
    put_short_int(&lb.block[offset + COD_LS_SLINE], state.src->line_number);


    /* Write the address of the opcode. Note: the '-1' takes into account that
     * the address is incremented previous to calling this routine */
    /*put_short_int(&cod_block[offset + COD_LS_SLOC], state.org);*/
    put_short_int(&lb.block[offset + COD_LS_SLOC],state.lst.line.was_org );

    break;
  case COD_LAST_LST_LINE:
    if(lb.block) {
      write_cod_block(&main_dir,COD_DIR_LSTTAB,COD_DIR_LSTTAB+2,&lb);
      delete_block(&lb);
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

void cod_write_symbols(struct symbol **symbol_list, int num_symbols)
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
  char * s;
  Block sb;

  if(!state.cod.enabled)
    return;

  create_block(&sb);

  offset = 0;

  for(i=0; i<num_symbols; i++) {
    var = get_symbol_annotation(symbol_list[i]);
    s = get_symbol_name(symbol_list[i]);
    len = strlen(s);

    /* If this symbol extends past the end of the cod block
     * then write this block out */

    if((offset + len + COD_SYM_EXTRA) >= COD_BLOCK_SIZE) {
      write_cod_block(&main_dir,COD_DIR_LSYMTAB,COD_DIR_LSYMTAB+2,&sb);
      get_next_block(&sb);
      offset = 0;
    }

    cod_strncpy(&sb.block[offset +1], s, MAX_SYM_LEN);

    switch(var->type) {
    case gvt_cblock:
      type = COD_ST_C_SHORT;  /* byte craft's nomenclature for a memory byte. */
      break;
    case gvt_address:
      type = COD_ST_ADDRESS;
      break;
    case gvt_org:
      type = COD_ST_ADDRESS;
      break;
    case gvt_constant:
    default:
      type = COD_ST_CONSTANT;
    }

    put_short_int(&sb.block[offset+len+COD_SYM_TYPE], type);
    put_int(&sb.block[offset+len+COD_SYM_VALUE], endian_swap(var->value));
    offset += (len+COD_SYM_EXTRA);
  }

  if(offset)
    write_cod_block(&main_dir,COD_DIR_LSYMTAB,COD_DIR_LSYMTAB+2,&sb);

  delete_block(&sb);
}

/* cod_emit_opcode - write one opcode to a cod_image_block
 */
void  cod_emit_opcode(int address,int opcode)
{
  DirBlockInfo *dbi;
  int block_index;
  int found;
  int _64k_base;
  char * block;

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

  block_index = (address >> (COD_BLOCK_BITS-1)) & (COD_CODE_IMAGE_BLOCKS -1); 
  _64k_base = (address >> 15) & 0xffff;


  dbi = &main_dir;

  /* find the directory containing this 64k segment */
  found = 0;
  do {
    if(get_short_int(&dbi->dir.block[COD_DIR_HIGHADDR]) == _64k_base)
      found = 1;
    else {
    
      /* If the next directory block (in the linked list of directory
         blocks) is NULL, then this is the first time to encounter this
         _64k segment. So we need to create a new segment. */
      if(dbi->next_dir_block_info == NULL) {
	dbi->next_dir_block_info  = malloc(sizeof(DirBlockInfo));
	init_DirBlock(dbi->next_dir_block_info);
	put_short_int(&dbi->dir.block[COD_DIR_NEXTDIR], 
		      dbi->next_dir_block_info->dir.block_number);
	put_short_int(&dbi->next_dir_block_info->dir.block[COD_DIR_HIGHADDR], 
		      _64k_base);
	found = 1;
      }

      dbi = dbi->next_dir_block_info;
    }
  }
  while(!found);


  if(dbi->cod_image_blocks[block_index].block == NULL) {
    create_block(&dbi->cod_image_blocks[block_index]);
  }

  block = dbi->cod_image_blocks[block_index].block;

  put_short_int(&block[(address*2) & (COD_BLOCK_SIZE - 1)], opcode);

}

void write_cod_range_block(unsigned int address, Block *rb)
{

  DirBlockInfo *dbi = &main_dir;
  unsigned int _64k_base;

  _64k_base = (address >> 15) & 0xffff;

  do {

    if(get_short_int(&dbi->dir.block[COD_DIR_HIGHADDR]) == _64k_base) {
      write_cod_block(dbi,COD_DIR_MEMMAP,COD_DIR_MEMMAP+2,rb);
      return;
    }
    dbi = dbi->next_dir_block_info;

  }  while(dbi);

  assert(0);

}

/* cod_write_code - write all of the assembled pic code to the .cod file
 */
void cod_write_code(void)
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

    for(i=mem_base; (i-mem_base) <=MAX_I_MEM; i++) {
      if ((i_memory_get(state.i_memory, i) & MEM_USED_MASK) && ((i-mem_base)<MAX_I_MEM)) {
	cod_emit_opcode(i, i_memory_get(state.i_memory, i) & 0xffff);
	if(used_flag == 0) {
	  start_address = i;       /* Save the start address in a range of opcodes */
	  used_flag = 1;
	  if(rb.block == NULL) {
	    create_block(&rb);
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
	  put_short_int(&rb.block[offset], 2*start_address);
	  put_short_int(&rb.block[offset+2], 2*(i-1));

	  offset += 4;
	  if(offset>=COD_BLOCK_SIZE) {
	    /* If there are a whole bunch of non-contiguous pieces of 
	       code then we'll get here. But most pic apps will only need
	       one directory block (that will give you 64 ranges or non-
	       contiguous chunks of pic code). */
	    write_cod_range_block(start_address, &rb);
	    delete_block(&rb);
	    offset = 0;
	  }
	  used_flag = 0;
	}
      }
    }

    if(offset) {
      write_cod_range_block(start_address, &rb);
      delete_block(&rb);
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
	write_cod_block(dbi,COD_DIR_CODE+i*2,COD_DIR_CODE+i*2,&dbi->cod_image_blocks[i]);
	free(dbi->cod_image_blocks[i].block);
      }
    dbi = dbi->next_dir_block_info;
  } while(dbi);

}

void write_directory(void)
{
  DirBlockInfo *dbi;
  dbi = &main_dir;

  do {
    fseek(state.cod.f,COD_BLOCK_SIZE * dbi->dir.block_number, SEEK_SET);
    fwrite(dbi->dir.block, 1, COD_BLOCK_SIZE, state.cod.f);

    dbi = dbi->next_dir_block_info;
  } while(dbi);
}

/*
 */
void cod_close_file(void)
{

  if(!state.cod.enabled)
    return;

  cod_lst_line(COD_LAST_LST_LINE);

  write_file_block();

  cod_write_code();

  cod_strncpy(&main_dir.dir.block[COD_DIR_PROCESSOR], 
	      state.processor_info->names[2],
	      COD_DIR_LSYMTAB - COD_DIR_PROCESSOR);

  write_directory();
  fclose(state.cod.f);
  free(main_dir.dir.block);
}

