#! /bin/bash

dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)

# Initialization
AC_INIT([gputils],[0.11.8])
AC_CONFIG_SRCDIR(gpasm/gpasm.c)
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

# Determine the host and build type. The target is always a PIC.
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

# Defaults
GPUTILS_HEADER_PATH="\$(datadir)/gputils/header"
GPUTILS_LKR_PATH="\$(datadir)/gputils/lkr"
LIBGPUTILS=libgputils/libgputils.a
LIBIBERTY=libiberty/libiberty.a
MAKE_SUBDIRS="libgputils libiberty gpasm header lkr doc"

# Check for additional parameters
AC_ARG_ENABLE(debug, 
  [  --enable-debug          enable gputils debug features],
  [enable_gputils_debug=yes])    

AC_ARG_ENABLE(gpal,
  [  --disable-gpal          disable the compiler],
  [], [enable_gpal=yes])

AC_ARG_ENABLE(gplink,
  [  --disable-gplink        disable the linker],
  [], [enable_gplink=yes])

AC_ARG_ENABLE(gputils,
  [  --disable-gputils       disable the utilities],
  [], [enable_gputils=yes])

AC_ARG_ENABLE(path,
  [  --disable-path          disable adding default search paths],
  [], [enable_gputils_path=yes])

# Checks for programs.
AC_PROG_CC
AC_PROG_YACC
AM_PROG_LEX
AC_PROG_INSTALL

# append the host alias to the tools for cross compiling
AC_CHECK_TOOL(RANLIB, ranlib, :)
AC_CHECK_TOOL(AR, ar, :)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(malloc.h unistd.h string.h strings.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_STRUCT_TM
AC_TYPE_SIZE_T 

# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(memset strcasecmp strchr strdup strerror stricmp strrchr strtol strtoul)

# Check for functions to compile into libiberty (getopt is always compiled).
IBERTYOBJS=
AC_CHECK_FUNC(bzero, , [IBERTYOBJS="$IBERTYOBJS bzero.o"])

# Options for the system on which the package will run
case "${host}" in
  *cygwin* )
    if test "x$GCC" = "xyes"; then
      CFLAGS="$CFLAGS -Wall -pedantic"
      LDFLAGS="$LDFLAGS -Wl,-warn-common -Wl,-warn-once"
    fi
    ;;
  *linux* )
    if test "x$GCC" = "xyes"; then
      CFLAGS="$CFLAGS -Wall -pedantic"
      LDFLAGS="$LDFLAGS -Wl,-warn-common -Wl,-warn-once"
    fi
    ;;
  *sun* )
    # sunos cc needs the -xCC flag for // comments
    if test "x$GCC" != "xyes"; then
      CFLAGS="$CFLAGS -xCC" 
    fi
    ;;
  *mingw* )
    ;;
  *-pc-os2_emx | *-pc-os2-emx )
    EXEEXT=".exe"
    if test "x$GCC" = "xyes"; then
      CFLAGS="-Zcrtdll -s -O2"
    fi
    ;;
esac

# Host filesystem options
case "${host}" in
  *mingw* | *-pc-os2_emx | *-pc-os2-emx )
    AC_DEFINE(HAVE_DOS_BASED_FILE_SYSTEM, 1, 
      [Define if your host uses a DOS based file system. ])
    ;;
esac

# Add extra gcc flags when configuring 
CFLAGS="$CFLAGS $XTRA_CFLAGS"

# Process the options
if test x$enable_gpal = xyes; then
  MAKE_SUBDIRS="$MAKE_SUBDIRS gpal"
fi

if test x$enable_gplink = xyes; then
  MAKE_SUBDIRS="$MAKE_SUBDIRS gplink"
fi

if test x$enable_gputils = xyes; then
  MAKE_SUBDIRS="$MAKE_SUBDIRS gputils"
fi

if test x$enable_gputils_debug = xyes; then
  AC_DEFINE(__DEBUG__, 1, 
    [Define if you want to enable GPUTILS debug features. ])
fi

if test x$enable_gputils_path = xyes; then
  AC_DEFINE(USE_DEFAULT_PATHS, 1, 
    [Define if you want to add default search paths. ])
fi

# Substitute configuration variables
AC_SUBST(GPUTILS_HEADER_PATH)
AC_SUBST(GPUTILS_LKR_PATH)
AC_SUBST(IBERTYOBJS)
AC_SUBST(LIBGPUTILS)
AC_SUBST(LIBIBERTY)
AC_SUBST(MAKE_SUBDIRS)

# Generate Makefiles
AC_CONFIG_FILES([Makefile 
                 doc/Makefile 
                 gpal/Makefile 
                 gpasm/Makefile 
                 gplink/Makefile 
                 gputils/Makefile 
                 header/Makefile 
                 libgputils/Makefile 
                 libiberty/Makefile 
                 lkr/Makefile
                 gputils.spec])

AC_OUTPUT

AC_MSG_RESULT([

gputils-$PACKAGE_VERSION is now configured for $canonical_host_type

  Build:                $build
  Host:                 $host
  Source directory:     $srcdir
  Installation prefix:  $prefix
  C compiler:           $CC $CFLAGS

])
