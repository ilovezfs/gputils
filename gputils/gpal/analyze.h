/* analyze semantics
   Copyright (C) 2003, 2004, 2005
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

#ifndef __ANALYZE_H__
#define __ANALYZE_H__

void analyze(tree *module);
void analyze_select_processor(tree *node, char *name);
void analyze_error(tree *node, const char *format, ...);
void analyze_warning(tree *node, const char *format, ...);

gp_boolean can_evaluate(tree *p, gp_boolean gen_errors);
int evaluate(tree *p);
int maybe_evaluate(tree *p);

int analyze_check_array(tree *symbol, struct variable *var);
void analyze_call(tree *call, gp_boolean in_expr, enum size_tag codegen_size);
void analyze_expr(tree *expr);

#endif
