/* gpasm preprocessor defines and includes
   Copyright (C) 1012 Borut Razem

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

#ifndef __PREPROCESS_H__
#define __PREPROCESS_H__

#define PREPROC_MAX_DEPTH       16

/* from preprocess.c */
void preprocess_line(char *buf, int *n, int max_size);

/* from ppscan.c */
int ppparse_chunk(char *buf, int begin, int end);
int pplex(void);
extern int ppcol_begin;
extern int ppcol_end;

/* from ppparse.c */
int ppparse(void);
extern int ppresult;
#endif
