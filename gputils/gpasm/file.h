/* Source file functions for gpasm

    Copyright (C) 2016 Molnar Karoly <molnarkaroly@users.sf.net>

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

#ifndef __FILE_H__
#define __FILE_H__

#include "stdhdr.h"

#include "libgputils.h"
#include "gpasm.h"

extern void file_delete_node(void *Node);
extern file_context_t *file_add(unsigned int Type, const char *Name);
extern void file_search_paths(source_context_t *Context, const char *Name);
extern void file_free(void);

#endif /* __FILE_H__ */
