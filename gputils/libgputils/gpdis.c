/* Convert one word from memory into an equivalent assembly instruction
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

#include "gpdis.h"

#define byte_op  0
#define bit_op   1
#define branch   2
#define literals 3

/* splices off a section of bits from the insn word */
/* the least significant bit is at '0', s1 >= s2 */
#define slice(s1, s2)  ((insn >> s2) & ((1<<(s1-s2+1))-1))

static char *byte_op_names12[] = {
  "movwf",
  "GPDASM ERROR",  /* this position isn't unique */ 
  "subwf", 
  "decf", 
  "iorwf", 
  "andwf", 
  "xorwf", 
  "addwf", 
  "movf", 
  "comf", 
  "incf", 
  "decfsz", 
  "rrf", 
  "rlf", 
  "swapf", 
  "incfsz" 
};

static char *bit_op_names12[] = {
  "bcf", 
  "bsf", 
  "btfsc", 
  "btfss"
};

static char *lit_op_names12[] = {
  "movlw", 
  "iorlw", 
  "andlw", 
  "xorlw"
};

static char *byte_op_names14[] = {
  "GPDASM ERROR",  /* this position isn't unique */ 
  "GPDASM ERROR",  /* this position isn't unique */ 
  "subwf", 
  "decf", 
  "iorwf", 
  "andwf", 
  "xorwf", 
  "addwf", 
  "movf", 
  "comf", 
  "incf", 
  "decfsz", 
  "rrf", 
  "rlf", 
  "swapf", 
  "incfsz" 
};

static char *bit_op_names14[] = {
  "bcf", 
  "bsf", 
  "btfsc", 
  "btfss"
};

static char *lit_op_names14[] = {
  "movlw", 
  "movlw", 
  "movlw", 
  "movlw", 
  "retlw",
  "retlw",
  "retlw",
  "retlw",
  "iorlw",
  "andlw",
  "xorlw",
  "GPDASM ERROR",  /* this position is unused, shouldn't ever get here */
  "sublw",
  "sublw",
  "addlw",
  "addlw"
};

void mem2asm12(int insn, char buffer[80])
{
  char *pointer = buffer;

  insn &= 0xfff;

  switch (slice(11,10))
  {
    case byte_op:
      if (slice(9,6) == 0) {
        if (slice(5,5)) {
          sprintf(pointer, "movwf\t0x%02x",
                  slice(4,0));
        } else {
          switch (slice(7,0)) 
          {
            case 0x00:
              sprintf(pointer, "nop");
              break;        
            case 0x02:
              sprintf(pointer, "option");
              break;
            case 0x03:
              sprintf(pointer, "sleep");
              break;        
            case 0x04:
              sprintf(pointer, "clrwdt");
              break;        
            case 0x05: 
            case 0x06: 
            case 0x07:
              sprintf(pointer, "tris\t%01x", insn & 7);
              break;
            default:
              sprintf(pointer, "dw\t0x%04x",
                      insn);
          }
        }
      } else if (slice(9,6) == 1) {
        if (slice(7,0) == 0x40) {
          sprintf(pointer, "clrw");
        } else if (slice(5,5)) {
          sprintf(pointer, "clrf\t0x%02x",
                  slice(4,0));
        } else {
          sprintf(pointer, "dw\t0x%03x",
                  insn);
        }
      } else {
        sprintf(pointer, "%s\t0x%02x,%s",
                byte_op_names12[slice(9,6)],
                slice(4,0),
                (insn & 0x20 ? "f" : "w"));
      }
      break;
    case bit_op:
      sprintf(pointer, "%s\t0x%02x,%d",
              bit_op_names12[slice(9,8)],
              slice(4,0),
              slice(7,5));
      break;
    case branch:
      if (insn & 0x200) {
        sprintf(pointer, "goto\t0x%03x",
                slice(8,0));
      } else if (slice(9,8) == 1) {
        sprintf(pointer, "call\t0x%02x",
                slice(7,0));
      } else {
        sprintf(pointer, "retlw\t0x%02x",
                slice(7,0));
      }
      break;
    case literals:
      sprintf(pointer, "%s\t0x%02x",
              lit_op_names12[slice(9,8)],
              slice(7,0));
      break;
    default:
      sprintf(pointer, "dw\t0x%04x",
              insn);
  }
  
}

void mem2asm14(int insn, char buffer[80])
{
  char *pointer = buffer;

  insn &= 0x3fff;

  switch (slice (13, 12))
  {
    case byte_op:
      if (slice(11,8) == 0) {
        if (slice(7,7)) {
          sprintf(pointer, "movwf\t0x%02x",
                  slice(6,0));
        } else {
          switch (slice(7,0)) 
          {
            case 0x08:
              sprintf(pointer, "return");
              break;        
            case 0x09:
              sprintf(pointer, "retfie");
              break;        
            case 0x62:
              sprintf(pointer, "option");
              break;
            case 0x63:
              sprintf(pointer, "sleep");
              break;        
            case 0x64:
              sprintf(pointer, "clrwdt");
              break;        
            case 0x65: 
            case 0x66: 
            case 0x67:
              sprintf(pointer, "tris\t%01x", insn & 7);
              break;
            case 0x00:
            case 0x20:
            case 0x40:
            case 0x60:
              sprintf(pointer, "nop");
              break;        
            default:
              sprintf(pointer, "dw\t0x%04x",
                      insn);
          }
        }
      } else if (slice(11,8) == 1) {
        if (slice(7,0) == 0x03) {
          sprintf(pointer, "clrw");
        } else if (slice(7,7)) {
          sprintf(pointer, "clrf\t0x%02x",
                  slice(6,0));
        } else {
          sprintf(pointer, "dw\t0x%04x",
                  insn);
        }
      } else {
        sprintf(pointer, "%s\t0x%02x,%s",
                byte_op_names14[slice(11,8)],
                slice(6,0),
                (insn & 0x80 ? "f" : "w"));
      }
      break;
    case bit_op:
      sprintf(pointer, "%s\t0x%02x,%d",
              bit_op_names14[slice(11,10)],
              slice(6,0),
              slice(9,7));
      break;
    case branch:
      if (insn & 0x800) {
        sprintf(pointer, "goto\t0x%04x",
                slice(10,0));
      } else {
        sprintf(pointer, "call\t0x%04x",
                slice(10,0));
      }
      break;
    case literals:
      if (slice(11,8) == 0x1011) {
        sprintf(pointer, "dw\t0x%04x",
                insn);
      } else {
        sprintf(pointer, "%s\t0x%02x",
                lit_op_names14[slice(11,8)],
                slice(7,0));
      }
      break;
    default:
      sprintf(pointer, "dw\t0x%04x",
              insn);
  }
}
