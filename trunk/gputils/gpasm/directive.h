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

#define W                   0
#define F                   1
#define ACCESS              0
#define BANKED              1

extern gpasmVal do_insn(const char *name, struct pnode *parms);
extern void opcode_init(int stage);
extern void begin_cblock(const struct pnode *c);
extern void continue_cblock(void);
extern void cblock_expr(const struct pnode *s);
extern void cblock_expr_incr(const struct pnode *s, const struct pnode *incr);
extern gp_boolean asm_enabled(void);

#endif
