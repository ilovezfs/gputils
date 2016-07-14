/* Double linked list support

   Copyright (C) 2016 Molnar Karoly

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

#ifndef __GPLIST_H__
#define __GPLIST_H__

typedef void (*gp_node_del_t)(void *);

#define GPNodeHeader(Type) \
  Type         *prev;      \
  Type         *next;      \
  unsigned int  list_id

#define GPListHeader(Type)  \
  Type          *first;     \
  Type          *last;      \
  size_t         num_nodes; \
  gp_node_del_t  node_del;  \
  unsigned int   serial_id

extern void *gp_node_new(size_t Item_size);
extern void gp_node_free(void *Node);
extern void *gp_node_append(void *List, void *Node);
extern void *gp_node_insert_after(void *List, void *Node, void *Node_new);
extern void *gp_node_insert_before(void *List, void *Node, void *Node_new);
extern void *gp_node_remove(void *List, void *Node);
extern void *gp_node_move(void *Dst, void *Src, void *Node);

extern void gp_list_set_delete_node_func(void *List, gp_node_del_t Function);
extern void **gp_list_make_block(void *List, size_t Num_nodes, size_t Item_size);
extern void gp_list_move(void *Dst, void *Src);
extern void gp_list_clear(void *List);
extern void gp_list_delete(void *List);

#endif /* __GPLIST_H__ */
