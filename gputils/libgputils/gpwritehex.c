/* ".HEX" file output for gputils
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

#include "stdhdr.h"
#include "libgputils.h"

/* mode flags */
#define all   0
#define low   1
#define high  2
#define swap  3  /* swap bytes for inhx16 format */
#define byte  4  /* write byte sized words */

static int sum;
static char *newline;
static FILE *hex;
MemBlock *memory;

static void new_record()
{
  fprintf(hex, ":");
  sum = 0;
}

static void write_byte(int b)
{
  sum += b;
    
  assert((0 <= b) && (b <= 255));
  fprintf(hex, "%02X", b);
}

/* Write big-endian word */
static void write_bg_word(int w)
{
  write_byte((w >> 8) & 0xff);
  write_byte(w & 0xff);
}

/* Write little-endian word */
static void write_word(int w)
{
  write_byte(w & 0xff);
  write_byte((w >> 8) & 0xff);
}

static void end_record()
{
  write_byte((-sum) & 0xff);
  fprintf(hex, newline);
}

void data_line(int start, int stop, int mode)
{
  new_record();
  if (mode == all) {
    write_byte(2 * (stop - start));
    write_bg_word(2 * start);
    write_byte(0);
    while (start < stop){ 
      write_word(i_memory_get(memory, start++));
    }
  } else if (mode == byte) {
    write_byte(stop - start);
    write_bg_word(start);
    write_byte(0);
    while (start < stop) {
      write_byte((i_memory_get(memory, start++)) & 0xff);
    }
  } else if (mode == swap) {
    write_byte(stop - start);
    write_bg_word(start);
    write_byte(0);
    while (start < stop){ 
      write_bg_word(i_memory_get(memory, start++));
    }
  } else {
    write_byte(stop - start);
    write_bg_word(start);
    write_byte(0);
    while (start < stop) { 
      if (mode == low) {
        write_byte((i_memory_get(memory, start++)) & 0xff);
      } else {
        write_byte(((i_memory_get(memory, start++)) & 0xff00) >> 8);
      }
    }
  } 
  end_record();
}
 
void seg_address_line(int segment)
{
  new_record();
  write_byte(2);
  write_word(0);
  write_byte(4);
  write_bg_word(segment);  
  end_record();
}

void last_line()
{
  new_record();
  write_byte(0);
  write_word(0);
  write_byte(1);
  end_record();
}

void write_i_mem(enum formats hex_format, int mode)
{
  MemBlock *m = memory;
  int i, j, maximum;

  while(m) {
    i = m->base << I_MEM_BITS;

    maximum = i + MAX_I_MEM;

    if (hex_format == inhx32) {
      seg_address_line(m->base);
    }

    while (i < maximum) {
      if ((i_memory_get(memory, i) & MEM_USED_MASK) == 0) {
	++i;
      } else {
	j = i;
	while ((i_memory_get(memory, i) & MEM_USED_MASK)) {
	  ++i;
	  if (((mode == all) || (mode == swap))  && ((i & 0x7) == 0))
	    break;
	  if ((i & 0xf) == 0)
	    break;
	}
	/* Now we have a run of (i - j) occupied memory locations. */
        /* Write the data to the file */
        data_line(j, i, mode);
      }
    }
    m = m->next;
  }

  last_line();
}

int writehex (char *basefilename, 
              MemBlock *m,   
              enum formats hex_format,
              int numerrors,
              int byte_words,
              int dos_newlines)
{
  char hexfilename[BUFSIZ];
  char lowhex[BUFSIZ];
  char highhex[BUFSIZ];

  memory = m;

  if (dos_newlines) {
    newline = "\r\n";  
  } else {
    newline = "\n";  
  }

   /* build file names */
  strcpy(hexfilename, basefilename);
  strcat(hexfilename, ".hex");
  strcpy(lowhex, basefilename);
  strcat(lowhex, ".hxl");
  strcpy(highhex, basefilename);
  strcat(highhex, ".hxh");

  if (numerrors) {
    /* Remove the hex files (if any) */
    unlink(hexfilename);
    unlink(lowhex);
    unlink(highhex);
    return 0;
  }

  /* No error: overwrite the hex file */
  if (hex_format == inhx8s) {

    /* Write the low memory */
    hex = fopen(lowhex, "wt");
    write_i_mem(hex_format, low);
    fclose(hex);

    /* Write the high memory */
    hex = fopen(highhex, "wt");
    write_i_mem(hex_format, high);
    fclose(hex);

  } else if (hex_format == inhx16) {

    hex = fopen(hexfilename, "wt");
    write_i_mem(hex_format, swap);
    fclose(hex);

  } else {

    hex = fopen(hexfilename, "wt");
    if (byte_words)
      write_i_mem(hex_format, byte);
    else
      write_i_mem(hex_format, all);
    fclose(hex);

  }

  return 0;
}

/* scan the memory to see if it exceeds 32K limit on inhx8m limit */
int check_writehex(MemBlock *m,   
                   enum formats hex_format)
{

int error = 0;

  if (hex_format == inhx8m) {

    while(m) {
      if (m->base > 0) {
        error = 1;
      }
      m = m->next;
    }
  }

  return error;
}
