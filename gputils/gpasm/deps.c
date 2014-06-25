/* dependency file generation
   Copyright (C) 2005
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
#include "gpasm.h"

void
deps_init(void)
{
  char output_file[BUFSIZ];

  if (state.depfile != OUT_NAMED) {
    snprintf(state.depfilename, sizeof(state.depfilename), "%s.d", state.basefilename);
  }

  if (state.depfile == OUT_SUPPRESS) {
    state.dep.enabled = false;
  } else {
    state.dep.f = fopen(state.depfilename, "w");
    if (state.dep.f == NULL) {
      perror(state.depfilename);
      exit(1);
    }
    state.dep.enabled = true;

    /* output file names may not be setup, so make one */
    if (state.mode == MODE_RELOCATABLE) {
      snprintf(output_file, sizeof(output_file), "%s.o", state.basefilename);
    } else {
      snprintf(output_file, sizeof(output_file), "%s.hex", state.basefilename);
    }

    fprintf(state.dep.f, "%s : ", output_file);
  }
}

void
deps_add(const char *file_name)
{
  if (state.dep.enabled) {
    fprintf(state.dep.f, " \\\n  %s", file_name);
  }
}

void
deps_close(void)
{
  if (state.dep.enabled) {
    fprintf(state.dep.f, "\n");
    fclose(state.dep.f);
  }
}
