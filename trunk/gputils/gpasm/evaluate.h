/* evaluates variables
   Copyright (C) 2002, 2003, 2004, 2005
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

#ifndef __EVALUATE_H__
#define __EVALUATE_H__

int enforce_arity(int arity, int must_be);
int enforce_simple(const struct pnode *p);
int list_length(const struct pnode *L);

int can_evaluate(const struct pnode *p);
int can_evaluate_value(const struct pnode *p);
gpasmVal evaluate(const struct pnode *p);
gpasmVal maybe_evaluate(const struct pnode *p);

int count_reloc(const struct pnode *p);
gpasmVal reloc_evaluate(const struct pnode *p, unsigned short type);
int eval_fill_number(const struct pnode *p);

#endif
