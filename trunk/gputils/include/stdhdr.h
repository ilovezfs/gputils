/* standard defines and includes
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
   James Bowman, Craig Franklin
   
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


#ifndef __STDHDR_H__
#define __STDHDR_H__

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#if defined(HAVE_STRING_H)
  #define _GNU_SOURCE
  #include <string.h>
#elif defined(HAVE_STRINGS_H)
  #include <strings.h>
#endif

#ifdef HAVE_INTTYPES_H
  #include <inttypes.h>
#else

  #ifdef TYPE_INT8
    typedef signed   TYPE_INT8 int8_t;
    typedef unsigned TYPE_INT8 uint8_t;
  #endif
  #ifdef TYPE_INT16
    typedef signed   TYPE_INT16 int16_t;
    typedef unsigned TYPE_INT16 uint16_t;
  #endif
  #ifdef TYPE_INT32
    typedef signed   TYPE_INT32 int32_t;
    typedef unsigned TYPE_INT32 uint32_t;
  #endif
  #ifdef TYPE_INT64
    typedef signed   TYPE_INT64 int64_t;
    typedef unsigned TYPE_INT64 uint64_t;
  #endif

  #if (NATIVE_WORD_SIZE == 64)
    #define __INT64_C(Cnt)	Cnt ## L
    #define __UINT64_C(Cnt)	Cnt ## UL
  #else
    #define __INT64_C(Cnt)	Cnt ## LL
    #define __UINT64_C(Cnt)	Cnt ## ULL
  #endif

  #define INT8_MIN		(-128)
  #define INT16_MIN		(-32767-1)
  #define INT32_MIN		(-2147483647-1)
  #define INT64_MIN		(-__INT64_C(9223372036854775807)-1)

  #define INT8_MAX		(127)
  #define INT16_MAX		(32767)
  #define INT32_MAX		(2147483647)
  #define INT64_MAX		(__INT64_C(9223372036854775807))

  #define UINT8_MAX		(255)
  #define UINT16_MAX		(65535)
  #define UINT32_MAX		(4294967295U)
  #define UINT64_MAX		(__UINT64_C(18446744073709551615))

#endif

#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>

#ifdef HAVE_STDLIB_H
  #include <stdlib.h>
#endif

#ifndef EXIT_SUCCESS
  #define EXIT_SUCCESS          0
  #define EXIT_FAILURE          1
#endif

#ifdef HAVE_LOCALE_H
  #include <locale.h>

  #ifdef HAVE_LANGINFO_H
    #include <langinfo.h>
  #endif
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
    #define strcasecmp          stricmp
  #endif
#endif

#ifndef HAVE_STRNCASECMP
  #ifdef HAVE_STRNICMP
    #define strncasecmp strnicmp
  #endif
#endif

#ifndef HAVE_STRNDUP
/* No system strndup: Use the one implemented in libiberty. */
char *strndup(const char *s, size_t n);
#endif

#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#ifdef HAVE_DOS_BASED_FILE_SYSTEM
  #define UNIX_PATH_CHAR        '/'
  #define PATH_SEPARATOR_CHAR   '\\'
  #define PATH_SEPARATOR_STR    "\\"
#else
  #define PATH_SEPARATOR_CHAR   '/'
  #define PATH_SEPARATOR_STR    "/"
#endif

#define GPUTILS_COPYRIGHT_STRING "Copyright (c) 1998-2016 gputils project"

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
