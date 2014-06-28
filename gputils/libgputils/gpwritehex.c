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

static int sum;
static char *newline;
static FILE *hex;
static MemBlock *memory;

static void
new_record(void)
{
  fprintf(hex, ":");
  sum = 0;
}

static void
write_byte(int b)
{
  sum += b;

  assert((0 <= b) && (b <= 255));
  fprintf(hex, "%02X", b);
}

/* Write big-endian word */
static void
write_bg_word(int w)
{
  write_byte((w >> 8) & 0xff);
  write_byte(w & 0xff);
}

/* Write little-endian word */
static void
write_word(int w)
{
  write_byte(w & 0xff);
  write_byte((w >> 8) & 0xff);
}

static void
start_record(int start, int len)
{
  new_record();
  write_byte(len);
  write_bg_word(start);
  write_byte(0);
}

static void
end_record(void)
{
  write_byte((-sum) & 0xff);
  fprintf(hex, "%s", newline);
}

static void
data_line(int start, int stop, enum mode_flags_e mode)
{
  if (mode == HMODE_ALL) {
    start_record(start, stop - start);
    while (start < stop) {
      unsigned char b;

      if (!b_memory_get(memory, start++, &b, NULL, NULL)) {
        b = 0xff;
      }
      write_byte(b);
    }
  }
  else if (mode == HMODE_SWAP) {
    /* MPLINK 2.40, 3.80, 4.11 and 4.3x do not support INHX16 format.
     * (FIXME I have no idea where it comes from or if it can be used
     * for whatever purpose it was written for.) */
    assert(((start % 2) == 0) && ((stop % 2) == 0));
    start_record(start / 2, (stop  - start) / 2);
    while (start < stop) {
      unsigned char b;

      if (!b_memory_get(memory, (start++) ^ 1, &b, NULL, NULL)) {
        b = 0xff;
      }
      write_byte(b);
    }
  }
  else {
    start_record(start, (stop - start) / (((mode == HMODE_LOW) || (HMODE_HIGH == mode)) ? 2 : 1));

    if (mode == HMODE_HIGH) {
      ++start;
    }

    while (start < stop) {
      unsigned char b;

      if (!b_memory_get(memory, start, &b, NULL, NULL)) {
        b = 0xff;
      }
      write_byte(b);
      start += 2;
    }
  }
  end_record();
}

static void
seg_address_line(int segment)
{
  new_record();
  write_byte(2);
  write_word(0);
  write_byte(4);
  write_bg_word(segment);
  end_record();
}

static void
last_line(void)
{
  new_record();
  write_byte(0);
  write_word(0);
  write_byte(1);
  end_record();
}

static void
write_i_mem(enum formats hex_format, enum mode_flags_e mode, unsigned int core_mask)
{
  MemBlock *m = memory;
  int i, j, maximum;

  while (m != NULL) {
    i = m->base << I_MEM_BITS;

    maximum = i + MAX_I_MEM;

    if (hex_format == INHX32) {
      /* FIXME would mode swap require division by 2? */
      seg_address_line(m->base);
    }
    else {
      assert(m->base == 0);
    }

    while (i < maximum) {
      unsigned char b;

      if (!b_memory_get(memory, i, &b, NULL, NULL)) {
        ++i;
      }
      else {
        j = i;
        while (b_memory_get(memory, i, &b, NULL, NULL)) {
          ++i;
          if ((((mode == HMODE_ALL) || (mode == HMODE_SWAP)) && ((i & 0xf) == 0)) || ((i & 0x1f) == 0)) {
            break;
          }
        }
#if 0
        /* disabled for MPASM compatibility,
         * regression test gpasm.mchip/asmfiles/szee16.asm */
        if (core_mask > 0xFF) {
          /* Write complete instructions, so move start down and stop up
             to even address. */
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
        /* To be bug-for-bug compatible with MPASM 5.34 we ignore
           negative addresses. */
        if (j >= 0) {
          data_line(j, i, mode);
        }
      }
    }
    m = m->next;
  }

  last_line();
}

int
writehex(const char *basefilename, MemBlock *m, enum formats hex_format,
         int numerrors, int dos_newlines, unsigned int core_mask)
{
  char hexfilename[BUFSIZ];
  char lowhex[BUFSIZ];
  char highhex[BUFSIZ];

  memory = m;

  newline = dos_newlines ? "\r\n" : "\n";

   /* build file names */
  snprintf(hexfilename, sizeof(hexfilename), "%s.hex", basefilename);
  snprintf(lowhex, sizeof(lowhex), "%s.hxl", basefilename);
  snprintf(highhex, sizeof(highhex), "%s.hxh", basefilename);

  if (numerrors > 0) {
    /* Remove the hex files (if any) */
    unlink(hexfilename);
    unlink(lowhex);
    unlink(highhex);
    return 0;
  }

  /* No error: overwrite the hex file */
  if (hex_format == INHX8S) {
    /* Write the low memory */
    hex = fopen(lowhex, "wt");

    if (hex == NULL) {
      perror(lowhex);
      exit(1);
    }

    write_i_mem(hex_format, HMODE_LOW, core_mask);
    fclose(hex);

    /* Write the high memory */
    hex = fopen(highhex, "wt");

    if (hex == NULL) {
      perror(highhex);
      exit(1);
    }

    write_i_mem(hex_format, HMODE_HIGH, core_mask);
    fclose(hex);
  }
  else if (hex_format == INHX16) {
    hex = fopen(hexfilename, "wt");

    if (hex == NULL) {
      perror(hexfilename);
      exit(1);
    }

    write_i_mem(hex_format, HMODE_SWAP, core_mask);
    fclose(hex);
  }
  else {
    hex = fopen(hexfilename, "wt");

    if (hex == NULL) {
      perror(hexfilename);
      exit(1);
    }

    write_i_mem(hex_format, HMODE_ALL, core_mask);
    fclose(hex);
  }

  return 0;
}

/* scan the memory to see if it exceeds 32kB limit on INHX8M limit */
int
check_writehex(MemBlock *m, enum formats hex_format)
{
  int error = 0;

  if (hex_format != INHX32) {
    while (m != NULL) {
      if (m->base > 0) {
        error = 1;
      }
      m = m->next;
    }
  }

  return error;
}
