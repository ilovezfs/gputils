/* standard defines and includes
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Craig Franklin
   
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


#ifndef __STDHDR_H__
#define __STDHDR_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "getopt.h"

#ifndef HAVE_STDLIB_H
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#endif

#ifndef HAVE_STRCASECMP
#ifdef HAVE_STRICMP
#define strcasecmp stricmp
#endif
#endif

#if defined(HAVE_STRING_H)
#include <string.h>
#elif defined(HAVE_STRINGS_H)
#include <strings.h>
#endif

#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#ifdef HAVE_DOS_BASED_FILE_SYSTEM
  #define UNIX_PATH_CHAR '/'
  #define PATH_CHAR '\\'
  #define COPY_CHAR "\\"
#else
  #define PATH_CHAR '/'
  #define COPY_CHAR "/"
#endif

#define GPUTILS_COPYRIGHT_STRING "Copyright (c) 1998-2011 gputils project"

#endif
