/* test for common problems in the header files
   Copyright (C) 2004
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

#include "stdhdr.h"

int
main(int argc, char *argv[])
{
  FILE *f;
  
  if (argc != 2) {
    printf("usage: header_check filename\n");
    return 1;
  }
  
  f = fopen(argv[1], "r");
  if (f == NULL) {
    perror(argv[1]);
    return 1;
  }

  fseek(f, -1, SEEK_END);
  if (fgetc(f) != '\n') {
    printf("Missing newline before EOF in file \"%s\"\n", argv[1]);
    return 1;
  }
  
  return 0;
}
