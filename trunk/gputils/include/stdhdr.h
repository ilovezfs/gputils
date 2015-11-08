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

#ifndef HAVE_STRNCASECMP
#ifdef HAVE_STRNICMP
#define strncasecmp strnicmp
#endif
#endif

#ifndef HAVE_STRNDUP
/* No system strndup: Use the one implemented in libiberty. */
char *strndup (const char *s, size_t n);
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

#define GPUTILS_COPYRIGHT_STRING "Copyright (c) 1998-2015 gputils project"

#if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
  #define GPUTILS_GCC_STR(s)            #s
  #define GPUTILS_GCC_JOINSTR(x,y)      GPUTILS_GCC_STR(x ## y)
  #define GPUTILS_GCC_DO_PRAGMA(x)      _Pragma(#x)
  #define GPUTILS_GCC_PRAGMA(x)         GPUTILS_GCC_DO_PRAGMA(GCC diagnostic x)

  #if ((__GNUC__ * 100) + __GNUC_MINOR__) < 406
    /* needed for older GCCs that accept only top of the file diagnostic pragmas */
    #define GPUTILS_GCC_DIAG_TOP(x)     GPUTILS_GCC_PRAGMA(ignored GPUTILS_GCC_JOINSTR(-W,x))
    #define GPUTILS_GCC_DIAG_OFF(x)
    #define GPUTILS_GCC_DIAG_ON(x)
  #else
    #define GPUTILS_GCC_DIAG_TOP(x)

    #define GPUTILS_GCC_DIAG_OFF(x) \
                GPUTILS_GCC_PRAGMA(push) \
                GPUTILS_GCC_PRAGMA(ignored GPUTILS_GCC_JOINSTR(-W,x))

    #define GPUTILS_GCC_DIAG_ON(x)      GPUTILS_GCC_PRAGMA(pop)
  #endif
#else
  #define GPUTILS_GCC_DIAG_TOP(x)
  #define GPUTILS_GCC_DIAG_OFF(x)
  #define GPUTILS_GCC_DIAG_ON(x)
#endif

#endif /* __STDHDR_H__ */
