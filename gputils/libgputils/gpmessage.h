/* messaging functions
   Copyright (C) 2003, 2004, 2005
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

#ifndef __GPMESSAGE_H__
#define __GPMESSAGE_H__

extern gp_boolean gp_quiet;
extern gp_boolean gp_message_disable;
extern gp_boolean gp_debug_disable;

extern int gp_num_errors;
extern int gp_num_warnings;
extern int gp_num_messages;

void gp_error(const char *format, ...);
void gp_warning(const char *format, ...);
void gp_message(const char *format, ...);
void gp_debug(const char *format, ...);

#endif
