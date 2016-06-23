/* ".HEX" file output for gputils
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
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
enum mode_flags_e {
  HMODE_ALL,
  HMODE_LOW,
  HMODE_HIGH,
  HMODE_SWAP  /* swap bytes for INHX16 format */
};

static int         checksum;
static const char *newline;
static FILE       *hex;
static MemBlock_t *memory;

/*------------------------------------------------------------------------------------------------*/

static void
_new_record(void)
{
  fprintf(hex, ":");
  checksum = 0;
}

/*------------------------------------------------------------------------------------------------*/

static void
_write_byte(uint8_t Value)
{
  checksum += (int)Value;
  fprintf(hex, "%02X", Value);
}

/*------------------------------------------------------------------------------------------------*/

/* Write big-endian word. */

static void
_write_bg_word(uint16_t Word)
{
  _write_byte((Word >> 8) & 0xff);
  _write_byte(Word & 0xff);
}

/*------------------------------------------------------------------------------------------------*/

/* Write little-endian word. */

static void
_write_word(uint16_t Word)
{
  _write_byte(Word & 0xff);
  _write_byte((Word >> 8) & 0xff);
}

/*------------------------------------------------------------------------------------------------*/

static void
_start_record(unsigned int Start, unsigned int Len)
{
  _new_record();
  _write_byte(Len);
  _write_bg_word(Start);
  _write_byte(0);
}

/*------------------------------------------------------------------------------------------------*/

static void
_end_record(void)
{
  _write_byte((-checksum) & 0xff);
  fprintf(hex, "%s", newline);
}

/*------------------------------------------------------------------------------------------------*/

static void
_data_line(unsigned int Start, unsigned int Stop, enum mode_flags_e Mode)
{
  uint8_t byte;

  if (Mode == HMODE_ALL) {
    _start_record(Start, Stop - Start);
    while (Start < Stop) {
      if (!b_memory_get(memory, Start++, &byte, NULL, NULL)) {
        byte = 0xff;
      }
      _write_byte(byte);
    }
  }
  else if (Mode == HMODE_SWAP) {
    /* MPLINK 2.40, 3.80, 4.11 and 4.3x do not support INHX16 format.
     * (FIXME I have no idea where it comes from or if it can be used
     * for whatever purpose it was written for.) */
    assert(((Start % 2) == 0) && ((Stop % 2) == 0));
    _start_record(Start / 2, (Stop  - Start) / 2);
    while (Start < Stop) {
      if (!b_memory_get(memory, (Start++) ^ 1, &byte, NULL, NULL)) {
        byte = 0xff;
      }
      _write_byte(byte);
    }
  }
  else {
    _start_record(Start, (Stop - Start) / (((Mode == HMODE_LOW) || (Mode == HMODE_HIGH)) ? 2 : 1));

    if (Mode == HMODE_HIGH) {
      ++Start;
    }

    while (Start < Stop) {
      if (!b_memory_get(memory, Start, &byte, NULL, NULL)) {
        byte = 0xff;
      }
      _write_byte(byte);
      Start += 2;
    }
  }
  _end_record();
}

/*------------------------------------------------------------------------------------------------*/

static void
_seg_address_line(unsigned int Segment)
{
  _new_record();
  _write_byte(2);
  _write_word(0);
  _write_byte(4);
  _write_bg_word(Segment);
  _end_record();
}

/*------------------------------------------------------------------------------------------------*/

static void
_last_line(void)
{
  _new_record();
  _write_byte(0);
  _write_word(0);
  _write_byte(1);
  _end_record();
}

/*------------------------------------------------------------------------------------------------*/

static void
_write_i_mem(enum formats Hex_format, enum mode_flags_e Mode, unsigned int Core_mask)
{
  MemBlock_t   *m;
  unsigned int  i;
  unsigned int  j;
  unsigned int  maximum;
  uint8_t       byte;

  m = memory;
  while (m != NULL) {
    i = IMemAddrFromBase(m->base);

    maximum = i + I_MEM_MAX;

    if (Hex_format == INHX32) {
      /* FIXME would mode swap require division by 2? */
      _seg_address_line(m->base);
    }
    else {
      assert(m->base == 0);
    }

    while (i < maximum) {
      if (!b_memory_get(memory, i, &byte, NULL, NULL)) {
        ++i;
      }
      else {
        j = i;
        while (b_memory_get(memory, i, &byte, NULL, NULL)) {
          ++i;
          if ((((Mode == HMODE_ALL) || (Mode == HMODE_SWAP)) && ((i & 0xf) == 0)) || ((i & 0x1f) == 0)) {
            break;
          }
        }
#if 0
        /* disabled for MPASM compatibility,
         * regression test gpasm.mchip/asmfiles/szee16.asm */
        if (Core_mask > 0xFF) {
          /* Write complete instructions, so move start down and stop up to even address. */
          if (j & 1) {
            --j;
          }

          if (i & 1) {
            ++i;
          }
        }
#endif
        /* Now we have a run of (i - j) occupied memory locations. */
        /* Write the data to the file */
        /* To be bug-for-bug compatible with MPASM 5.34 we ignore negative addresses. */
        if (j >= 0) {
          _data_line(j, i, Mode);
        }
      }
    }
    m = m->next;
  }

  _last_line();
}

/*------------------------------------------------------------------------------------------------*/

gp_boolean
writehex(const char *Base_filename, MemBlock_t *M, enum formats Hex_format, int Num_errors,
         gp_boolean Dos_newlines, unsigned int Core_mask)
{
  char hex_filename[BUFSIZ];
  char low_hex[BUFSIZ];
  char high_hex[BUFSIZ];

  memory = M;

  newline = Dos_newlines ? "\r\n" : "\n";

   /* build file names */
  snprintf(hex_filename, sizeof(hex_filename), "%s.hex", Base_filename);
  snprintf(low_hex, sizeof(low_hex), "%s.hxl", Base_filename);
  snprintf(high_hex, sizeof(high_hex), "%s.hxh", Base_filename);

  if (Num_errors > 0) {
    /* Remove the hex files (if any). */
    unlink(hex_filename);
    unlink(low_hex);
    unlink(high_hex);
    return false;
  }

  /* No error: overwrite the hex file */
  if (Hex_format == INHX8S) {
    /* Write the low memory */
    hex = fopen(low_hex, "wt");

    if (hex == NULL) {
      perror(low_hex);
      return false;
    }

    _write_i_mem(Hex_format, HMODE_LOW, Core_mask);
    fclose(hex);

    /* Write the high memory. */
    hex = fopen(high_hex, "wt");

    if (hex == NULL) {
      perror(high_hex);
      return false;
    }

    _write_i_mem(Hex_format, HMODE_HIGH, Core_mask);
    fclose(hex);
  }
  else if (Hex_format == INHX16) {
    hex = fopen(hex_filename, "wt");

    if (hex == NULL) {
      perror(hex_filename);
      return false;
    }

    _write_i_mem(Hex_format, HMODE_SWAP, Core_mask);
    fclose(hex);
  }
  else {
    hex = fopen(hex_filename, "wt");

    if (hex == NULL) {
      perror(hex_filename);
      return false;
    }

    _write_i_mem(Hex_format, HMODE_ALL, Core_mask);
    fclose(hex);
  }

  return true;
}

/*------------------------------------------------------------------------------------------------*/

/* Scan the memory to see if it exceeds 32kB limit on INHX8M limit. */

gp_boolean
check_writehex(MemBlock_t *M, enum formats Hex_format)
{
  gp_boolean ok = true;

  if (Hex_format != INHX32) {
    while (M != NULL) {
      if (M->base > 0) {
        ok = false;
        /* Superfluous to watch the further blocks. */
        break;
      }
      M = M->next;
    }
  }

  return ok;
}
