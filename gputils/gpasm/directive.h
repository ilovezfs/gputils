/* Implements directives, pseudo-ops and processor opcodes
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

#ifndef __OPCODE_H__
#define __OPCODE_H__

#define HEAD(L) (L)->value.list.head
#define TAIL(L) (L)->value.list.tail

#define CORE_8BIT_MASK  0xff
#define CORE_12BIT_MASK 0xfff
#define CORE_14BIT_MASK 0x3fff
#define CORE_16BIT_MASK 0xffff

gpasmVal do_insn(const char *name, struct pnode *parms);
void opcode_init(int stage);
void begin_cblock(const struct pnode *c);
void continue_cblock(void);
void cblock_expr(const struct pnode *s);
void cblock_expr_incr(const struct pnode *s, const struct pnode *incr);
int asm_enabled(void);
int check_page(int address);

#endif
