/* GNU PIC general coff functions
   Copyright (C) 2001 Craig Franklin

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
#include "gpcoff.h"
#include "gpreadobj.h"

struct objectfile *
gp_coff_init(char * name)
{
  struct objectfile *object = NULL;

  /* allocate memory for the object file */
  object = (struct objectfile *)malloc(sizeof(*object));

  object->filename = strdup(name);

  /* initialize the file header */
  object->file_header.f_magic  = MICROCHIP_MAGIC;
  object->file_header.f_nscns  = 0;
  object->file_header.f_timdat = (long)time(NULL);;
  object->file_header.f_symptr = 0;
  object->file_header.f_nsyms  = 0;
  object->file_header.f_opthdr = OPT_HDR_SIZ;
  object->file_header.f_flags  = 0;  

  /* initialize the optional header */
  object->opt_header.opt_magic      = OPTMAGIC;
  object->opt_header.vstamp         = 1;
  object->opt_header.proc_type      = 0;
  object->opt_header.rom_width_bits = 0;
  object->opt_header.ram_width_bits = 0;

  return object;
}
