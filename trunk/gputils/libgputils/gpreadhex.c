/* Read ".HEX" files and store it in memory
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

#define LINESIZ         520

static char     linebuf[LINESIZ];
static char    *linept;
static uint8_t  checksum;
static FILE    *infile;

/*------------------------------------------------------------------------------------------------*/

/* Converts a single ASCII character into a number. */

static uint8_t
_a2n(uint8_t Character)
{
  uint8_t number;

  if (Character <= '9') {
    number = Character - '0';
  }
  else {
    /* Convert lower case to upper. */
    Character &= ~('a' - 'A');
    number = Character - ('A' - 10);
  } 
  return number;
}

/*------------------------------------------------------------------------------------------------*/

static uint8_t
_readbyte(void)
{
  uint8_t number;

  linept++;
  number  = _a2n(*linept) << 4;
  linept++;
  number |= _a2n(*linept);

  checksum += number;
  return number;
} 

/*------------------------------------------------------------------------------------------------*/

static uint16_t
_readword(void)
{
  uint16_t number;

  number  = (uint16_t)_readbyte();  
  number |= (uint16_t)_readbyte() << 8;
  return number; 
}

/*------------------------------------------------------------------------------------------------*/

static uint16_t
_swapword(uint16_t Input)
{
  uint16_t number;

  number = ((Input & 0xFF) << 8) | ((Input & 0xFF00) >> 8);
  return number;
}

/*------------------------------------------------------------------------------------------------*/

hex_data_t *
gp_readhex(const char *File_name, MemBlock_t *M)
{
  hex_data_t   *info;
  unsigned int  length;
  unsigned int  address;
  unsigned int  type;
  uint8_t       byte;
  unsigned int  i;
  unsigned int  page;

  info = GP_Malloc(sizeof(*info));
  info->hex_format = INHX8M;
  info->size       = 0;
  info->error      = false;

  /* Open the input file. */
  if ((infile = fopen(File_name, "rt")) == NULL) {
    perror(File_name);
    exit(1);
  }
    
  /* Go to the beginning of the file. */
  fseek(infile, 0L, SEEK_SET);

  /* Set the line pointer to the beginning of the line buffer. */
  linept = linebuf;

  /* Read a line of data from the file, if NULL stop. */
  while (fgets(linept, LINESIZ, infile) != NULL)
  {
    /* Set the line pointer to the beginning of the line buffer. */
    linept = linebuf;

    checksum = 0;

    /* Fetch the number of bytes. */
    length = _readbyte();
    if (length == 0) {
      fclose(infile);
      return info;
    }

    /* Fetch the address. */
    address = _swapword(_readword());

    if (info->hex_format == INHX16) {
      address *= 2;
      length  *= 2;
    }

    /* Read the type of record. */
    type = _readbyte();

    if (type == IHEX_RECTYPE_EXT_LIN_ADDR) {
      if (info->hex_format == INHX16) {
        printf("\nHex Format Error\n");
        fclose(infile);
        info->error = true;
        return info;      
      }

      /* INHX32 segment line. */
      page = ((_readbyte() << 8) + _readbyte()) << 16;
      info->hex_format = INHX32;
    }
    else {
      /* Read the data (skipping last byte if at odd address). */
      for (i = 0; i < length; ++i) {
	byte = _readbyte();

	if (info->hex_format == INHX16) {
	  b_memory_put(M, page | ((address + i) ^ 1), byte, File_name, NULL);
	}
	else {
	  b_memory_put(M, page | (address + i),       byte, File_name, NULL);
	}
      }

      info->size += length;
    }

    /* Read the checksum, data is thrown away. */
    _readbyte();

    if (checksum != 0) { 
      if (info->hex_format == INHX8M) {
        /*  First attempt at INHX8M failed, try INHX16. */
        fseek(infile, 0L, SEEK_SET);	  
        info->hex_format = INHX16;
        info->size       = 0;
        /* Data in i_memory is trash. */
        i_memory_free(M);
        M = i_memory_create();
      }
      else {
        printf("\nChecksum Error\n");
        fclose(infile);
        info->error = true;
        return info;
      }
    }

    /* Set the line pointer to the beginning of the line buffer. */
    linept = linebuf;
  }

  fclose(infile);

  return info;
}
