/* dependency file generation
   Copyright (C) 2004, 2005
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

#include "libgputils.h"
#include "gpal.h"

void
deps_init(tree *module)
{
  char depfilename[BUFSIZ];

  if (state.make_deps) {
    strncpy(depfilename, FILE_NAME(module), sizeof(depfilename));
    strncat(depfilename, ".d", sizeof(depfilename));
    state.output.d = fopen(depfilename, "w");
    if (state.output.d == NULL) {
      perror(depfilename);
      exit(1);
    }
    
    fprintf(state.output.d, "%s.o : %s",
            FILE_NAME(module),
            get_file_name(module->file_id));

  }

}

void
deps_add(char *file_name)
{
  if (state.output.d) {
    fprintf(state.output.d, " \\\n  %s", file_name);
  }
}

void
deps_close(void)
{
  if (state.output.d) {
    fprintf(state.output.d, "\n");
    fclose(state.output.d);
  }
}
