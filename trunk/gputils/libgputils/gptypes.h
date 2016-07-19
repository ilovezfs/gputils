/* libgputils typedefs
   Copyright (C) 2001, 2002, 2003, 2004, 2005
   Craig Franklin

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

#ifndef __GPTYPES_H__
#define __GPTYPES_H__

#include "stdhdr.h"

typedef enum {
  false = (0 == 1),
  true  = (0 == 0)
} gp_boolean;

typedef long    gp_symvalue_t;

typedef struct __attribute__ ((packed))
{
  union __attribute__ ((packed))
  {
    uint32_t u32[2];
    uint64_t u64;
  } low;

  union __attribute__ ((packed))
  {
    uint32_t u32[2];
    uint64_t u64;
  } high;
} hash128_t;

#define BYTE_SIZE               (sizeof(uint8_t))
#define WORD_SIZE               (sizeof(uint16_t))
#define DWORD_SIZE              (sizeof(uint32_t))
#define QWORD_SIZE              (sizeof(uint64_t))

/*------------------------------------------------------------------------------------------------*/

/* Parse node: created by the parser, interpreted by the 'backend' */

typedef struct pnode {
  enum pnode_tag {
    PTAG_CONSTANT,
    PTAG_SYMBOL,
    PTAG_STRING,
    PTAG_OFFSET,
    PTAG_LIST,
    PTAG_BINOP,
    PTAG_UNOP
  } tag;

  union {
    int             constant;
    const char     *symbol;
    char           *string;

    struct pnode   *offset;

    struct {
      struct pnode *head;
      struct pnode *tail;
    } list;

    struct {
      int           op;
      struct pnode *p0;
      struct pnode *p1;
    } binop;

    struct {
      int           op;
      struct pnode *p0;
    } unop;
  } value;
} pnode_t;

#define PnIsConstant(Node)              ((Node)->tag == PTAG_CONSTANT)
#define PnIsSymbol(Node)                ((Node)->tag == PTAG_SYMBOL)
#define PnIsString(Node)                ((Node)->tag == PTAG_STRING)
#define PnIsOffset(Node)                ((Node)->tag == PTAG_OFFSET)
#define PnIsList(Node)                  ((Node)->tag == PTAG_LIST)
#define PnIsBinOp(Node)                 ((Node)->tag == PTAG_BINOP)
#define PnIsUnOp(Node)                  ((Node)->tag == PTAG_UNOP)

#define PnConstant(Node)                ((Node)->value.constant)
#define PnSymbol(Node)                  ((Node)->value.symbol)
#define PnString(Node)                  ((Node)->value.string)
#define PnOffset(Node)                  ((Node)->value.offset)
#define PnListHead(Node)                ((Node)->value.list.head)
#define PnListTail(Node)                ((Node)->value.list.tail)
#define PnBinOpOp(Node)                 ((Node)->value.binop.op)
#define PnBinOpP0(Node)                 ((Node)->value.binop.p0)
#define PnBinOpP1(Node)                 ((Node)->value.binop.p1)
#define PnUnOpOp(Node)                  ((Node)->value.unop.op)
#define PnUnOpP0(Node)                  ((Node)->value.unop.p0)

#endif
