/* messaging functions
   Copyright (C) 2003, 2004
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

#ifdef STDC_HEADERS
#include <stdarg.h>
#endif

int gp_message_disable = 0;
int gp_quiet = 0;     
int gp_num_errors = 0;
int gp_num_warnings = 0;
int gp_num_messages = 0;
int gp_debug_disable = 1;

void 
gp_error(const char *format, ...)
{
  va_list args;
  char buffer[BUFSIZ]; 

  if (gp_message_disable != 0)
    return;

  gp_num_errors++;

  if (gp_quiet != 0)
    return;

  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);

  printf("%s %s\n", "error:", buffer);

  return;
}

void 
gp_warning(const char *format, ...)
{
  va_list args;
  char buffer[BUFSIZ]; 

  if (gp_message_disable != 0)
    return;

  gp_num_warnings++;

  if (gp_quiet != 0)
    return;

  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);

  printf("%s %s\n", "warning:", buffer);

  return;
}

void 
gp_message(const char *format, ...)
{
  va_list args;
  char buffer[BUFSIZ]; 

  if (gp_message_disable != 0)
    return;

  gp_num_messages++;

  if (gp_quiet != 0)
    return;

  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);

  printf("%s %s\n", "message:", buffer);

  return;
}

void 
gp_debug(const char *format, ...)
{
  va_list args;
  char buffer[BUFSIZ]; 

  if (gp_debug_disable != 0)
    return;

  if (gp_quiet != 0)
    return;

  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);

  printf("%s %s\n", "debug:", buffer);

  return;
}
