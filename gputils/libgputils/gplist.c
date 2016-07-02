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

#include "stdhdr.h"
#include "libgputils.h"

#define LIST_INVALID_ID                 0

typedef struct gp_node {
  GPNodeHeader(struct gp_node);
} gp_node_t;

typedef struct gp_list {
  GPListHeader(struct gp_node);
} gp_list_t;

static unsigned int list_serial_id = 1;

/*------------------------------------------------------------------------------------------------*/

void *
gp_node_new(size_t Item_size)
{
  if (Item_size == 0) {
    fprintf(stderr, "%s() -- Item_size is 0.\n", __func__);
    exit(1);
  }

  return GP_Calloc(1, Item_size);
}

/*------------------------------------------------------------------------------------------------*/

void
gp_node_free(void *Node)
{
  if (Node != NULL) {
    free(Node);
  }
}

/*------------------------------------------------------------------------------------------------*/

void *
gp_node_append(void *List, void *Node)
{
  gp_list_t *l;
  gp_node_t *n;

  if ((List == NULL) || (Node == NULL)) {
    return NULL;
  }

  l = (gp_list_t *)List;

  if (l->serial_id == LIST_INVALID_ID) {
    l->serial_id = list_serial_id++;
  }

  n = (gp_node_t *)Node;
  n->prev = l->last;
  n->next = NULL;

  if (l->first == NULL) {
    /* The list is empty. */
    l->first = n;
  }
  else {
    /* Append the new node to the end of the list. */
    l->last->next = n;
  }

  l->last    = n;
  n->list_id = l->serial_id;
  (l->num_nodes)++;

  return Node;
}

/*------------------------------------------------------------------------------------------------*/

void *
gp_node_insert_after(void *List, void *Node, void *Node_new)
{
  gp_list_t *l;
  gp_node_t *n;
  gp_node_t *new;
  gp_node_t *next;

  if ((List == NULL) || (Node == NULL) || (Node_new == NULL)) {
    return NULL;
  }

  l   = (gp_list_t *)List;
  n   = (gp_node_t *)Node;
  new = (gp_node_t *)Node_new;

  next      = n->next;
  new->prev = n;
  new->next = next;

  if (l->last == n) {
    /* This is last node of the list. */
    l->last = new;
  }

  if (next != NULL) {
    /* The "next" node exists. */
    next->prev = new;
  }

  n->next      = new;
  new->list_id = l->serial_id;
  (l->num_nodes)++;

  return Node_new;
}

/*------------------------------------------------------------------------------------------------*/

void *
gp_node_insert_before(void *List, void *Node, void *Node_new)
{
  gp_list_t *l;
  gp_node_t *n;
  gp_node_t *new;
  gp_node_t *prev;

  if ((List == NULL) || (Node == NULL) || (Node_new == NULL)) {
    return NULL;
  }

  l   = (gp_list_t *)List;
  n   = (gp_node_t *)Node;
  new = (gp_node_t *)Node_new;

  prev      = n->prev;
  new->prev = prev;
  new->next = n;

  if (l->first == n) {
    /* This is first node of the list. */
    l->first = new;
  }

  if (prev != NULL) {
    /* The "prev" node exists. */
    prev->next = new;
  }

  n->prev      = new;
  new->list_id = l->serial_id;
  (l->num_nodes)++;

  return Node_new;
}

/*------------------------------------------------------------------------------------------------*/

void *
gp_node_remove(void *List, void *Node)
{
  gp_list_t *l;
  gp_node_t *n;

  if ((List == NULL) || (Node == NULL)) {
    return NULL;
  }

  l = (gp_list_t *)List;
  n = (gp_node_t *)Node;

  if (n->list_id != l->serial_id) {
    gp_error("The node{%u} does not belong to this list{%u}.", n->list_id, l->serial_id);
    assert(0);
  }

  if (l->first == n) {
    /* This is first node of the list, the next will be the first. */
    l->first = n->next;
  }
  else {
    /* The previous node connects to next. */
    n->prev->next = n->next;
  }

  if (l->last == n) {
    /* This is last node of the list, the previous will be the last. */
    l->last = n->prev;
  }
  else {
    /* The next node connects to previous. */
    n->next->prev = n->prev;
  }

  n->prev    = NULL;
  n->next    = NULL;
  n->list_id = 0;
  (l->num_nodes)--;

  return Node;
}

/*------------------------------------------------------------------------------------------------*/

void *
gp_node_move(void *Dst, void *Src, void *Node)
{
  return gp_node_append(Dst, gp_node_remove(Src, Node));
}

/*------------------------------------------------------------------------------------------------*/

void **
gp_list_make_block(void *List, size_t Num_nodes, size_t Item_size)
{
  gp_list_t     *l;
  gp_node_t    **ptr_array;
  unsigned int   id;
  size_t         i;

  if ((List == NULL) || (Num_nodes == 0) || (Item_size == 0)) {
    return NULL;
  }

  l = (gp_list_t *)List;

  if (l->first != NULL) {
    gp_error("%s() -- The block list already exists, can not be created again.", __func__);
    assert(0);
  }

  id = list_serial_id++;
  ptr_array = (gp_node_t **)GP_Malloc(Num_nodes * sizeof(gp_node_t *));
  l->num_nodes = Num_nodes;
  l->serial_id = id;

  for (i = 0; i < Num_nodes; i++) {
    ptr_array[i] = (gp_node_t *)gp_node_new(Item_size);
    ptr_array[i]->list_id = id;
  }

  /* Do not process the last item. */
  Num_nodes--;

  /* Initialize the pointers to create the linked list. */
  for (i = 0; i < Num_nodes; i++) {
    ptr_array[i + 1]->prev = ptr_array[i];
    ptr_array[i]->next     = ptr_array[i + 1];
  }

  /* The first->prev and last->next values is already NULL. (calloc) */

  l->first = ptr_array[0];
  l->last  = ptr_array[Num_nodes];

  return (void **)ptr_array;
}

/*------------------------------------------------------------------------------------------------*/

static void
_fresh_list_ids(gp_list_t *List, gp_node_t *Start_node)
{
  gp_node_t    *node;
  unsigned int  id;

  id   = List->serial_id;
  node = (Start_node != NULL) ? Start_node : List->first;
  while (node != NULL) {
    node->list_id = id;
    node = node->next;
  }
}

/*------------------------------------------------------------------------------------------------*/

void
gp_list_move(void *Dst, void *Src)
{
  gp_list_t *d;
  gp_list_t *s;

  if ((Dst == NULL) || (Src == NULL)) {
    return;
  }

  d = (gp_list_t *)Dst;
  s = (gp_list_t *)Src;

  if (s->first != NULL) {
    /* Append the nodes from the "Src" list to the "Dst". */
    if (s->num_nodes == 0) {
      fprintf(stderr, "%s() -- Src->num_nodes: %zu", __func__, s->num_nodes);
      assert(0);
    }

    if (d->first == NULL) {
      if (d->num_nodes > 0) {
        fprintf(stderr, "%s() -- Dst->num_nodes: %zu", __func__, d->num_nodes);
        assert(0);
      }

      d->serial_id = list_serial_id++;
      d->first     = s->first;
    }
    else {
      if (d->num_nodes == 0) {
        fprintf(stderr, "%s() -- Dst->num_nodes: %zu", __func__, d->num_nodes);
        assert(0);
      }

      d->last->next  = s->first;
      s->first->prev = d->last;
    }

    d->last       = s->last;
    d->num_nodes += s->num_nodes;
    _fresh_list_ids(d, s->first);

    /* In the "Src" list will not stay reference onto any node. */
    gp_list_clear(Src);
  }
}

/*------------------------------------------------------------------------------------------------*/

void
gp_list_clear(void *List)
{
  if (List == NULL) {
    return;
  }

  memset(List, 0, sizeof(gp_list_t));
}

/*------------------------------------------------------------------------------------------------*/

void
gp_list_delete(void *List)
{
  gp_node_t *node;
  gp_node_t *next;

  if (List == NULL) {
    return;
  }

  node = ((gp_list_t *)List)->first;
  while (node != NULL) {
    next = node->next;
    gp_node_free(gp_node_remove(List, node));
    node = next;
  }

  gp_list_clear(List);
}
