#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([gputils],[1.4.2], [<URL:http://gputils.sourceforge.net/>])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

# Determine the host and build type. The target is always a PIC.
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

# Defaults
GPUTILS_HEADER_PATH="\$(datadir)/gputils/header"
GPUTILS_LKR_PATH="\$(datadir)/gputils/lkr"
GPUTILS_LIB_PATH="\$(datadir)/gputils/lib"
GPUTILS_HTMLDOC_PATH=${GPUTILS_HTMLDOC_PATH:-"\$(datadir)/doc/gputils-\$(PACKAGE_VERSION)/html"}
LIBGPUTILS=libgputils/libgputils.a
LIBIBERTY=libiberty/libiberty.a
MAKE_SUBDIRS="libgputils libiberty gpasm gplink gputils header lkr doc man"

# Check for additional parameters
AC_ARG_ENABLE(debug,
  [  --enable-debug          enable gputils debug features],
  [enable_gputils_debug=yes])

AC_ARG_ENABLE(path,
  [  --disable-path          disable adding default search paths],
  [], [enable_gputils_path=yes])

AC_ARG_ENABLE(html-doc,
  [  --disable-html-doc      disable creating the html device doc],
  [ENABLE_HTML_DOC="$enableval"],
  [ENABLE_HTML_DOC=yes])

AC_ARG_WITH(mplabx-path,
  AC_HELP_STRING([--with-mplabx-path=DIR],
                 [location of MPLAB-X (default=/opt/microchip/mplabx/v3.15)]),
  [DEFAULT_MPLABX_PATH="$withval"],
  [DEFAULT_MPLABX_PATH="/opt/microchip/mplabx/v3.15"])

# Checks for programs.
AC_PROG_CC
AC_PROG_YACC
AM_PROG_LEX
set `$LEX -V`
if test "${2%.*}" != "2.5" && test "${2##*.}" -ge 4; then
  AC_MSG_WARN(gputils supports flex version 2.5.4)
fi
AC_PROG_INSTALL

# append the host alias to the tools for cross compiling
AC_CHECK_TOOL(RANLIB, ranlib, :)
AC_CHECK_TOOL(AR, ar, :)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([libintl.h malloc.h stdlib.h string.h strings.h sys/ioctl.h termios.h unistd.h windows.h])
AC_CHECK_DECLS([asprintf, basename, getopt, vasprintf])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T 
AC_STRUCT_TM

# Checks for library functions.
AC_CHECK_FUNCS([strcasecmp strncasecmp stricmp strnicmp strndup realpath])

# Check for functions to compile into libiberty (getopt is always compiled).
IBERTYOBJS=
AC_CHECK_FUNC(vsnprintf, , [IBERTYOBJS="$IBERTYOBJS vasprintf.o vsnprintf.o"])
AC_CHECK_FUNC(snprintf, , [IBERTYOBJS="$IBERTYOBJS snprintf.o"])
AC_CHECK_FUNC(strndup, , [IBERTYOBJS="$IBERTYOBJS strndup.o"])

AM_CFLAGS=
AM_LDFLAGS=

# Options for the system on which the package will run
case "${host}" in
  *cygwin* )
    if test "x$GCC" = "xyes"; then
      AM_CFLAGS="-Wall -pedantic"
      AM_LDFLAGS="-Wl,-warn-common -Wl,-warn-once"
    fi
    ;;
  *linux* )
    if test "x$GCC" = "xyes"; then
      AM_CFLAGS="-Wall -pedantic"
      AM_LDFLAGS="-Wl,-warn-common -Wl,-warn-once"
    fi
    ;;
  *sun* )
    # sunos cc needs the -xCC flag for // comments
    if test "x$GCC" != "xyes"; then
      AM_CFLAGS="-xCC" 
    fi
    ;;
  *mingw* )
    ;;
  *-pc-os2_emx | *-pc-os2-emx )
    EXEEXT=".exe"
    if test "x$GCC" = "xyes"; then
      AM_CFLAGS="-Zcrtdll"
    fi
    ;;
esac

# Host filesystem options
case "${host}" in
  *mingw* | *-pc-os2_emx | *-pc-os2-emx | *djgpp* )
    AC_DEFINE(HAVE_DOS_BASED_FILE_SYSTEM, 1, 
      [Define if your host uses a DOS based file system. ])
    ;;
esac

# Process the options
if test x$enable_gputils_debug = xyes; then
  AC_DEFINE(GPUTILS_DEBUG, 1,
    [Define if you want to enable GPUTILS debug features. ])
fi

if test x$enable_gputils_path = xyes; then
  AC_DEFINE(USE_DEFAULT_PATHS, 1,
    [Define if you want to add default search paths. ])
fi

# Substitute configuration variables
AC_SUBST(ENABLE_HTML_DOC)
AC_SUBST(DEFAULT_MPLABX_PATH)
AC_SUBST(AM_CFLAGS)
AC_SUBST(AM_LDFLAGS)
AC_SUBST(GPUTILS_HEADER_PATH)
AC_SUBST(GPUTILS_LKR_PATH)
AC_SUBST(GPUTILS_LIB_PATH)
AC_SUBST(GPUTILS_HTMLDOC_PATH)
AC_SUBST(IBERTYOBJS)
AC_SUBST(LIBGPUTILS)
AC_SUBST(LIBIBERTY)
AC_SUBST(MAKE_SUBDIRS)

# Generate Makefiles
AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 doc/html-help/Makefile
                 gpasm/Makefile
                 gplink/Makefile
                 gputils/Makefile
                 header/Makefile
                 libgputils/Makefile
                 libiberty/Makefile
                 lkr/Makefile
                 man/Makefile
                 man/gpasm.1
                 man/gpdasm.1
                 man/gplib.1
                 man/gplink.1
                 man/gpstrip.1
                 man/gputils.1
                 man/gpvc.1
                 man/gpvo.1
                 man/fr/Makefile
                 man/fr/gpasm.1
                 man/fr/gpdasm.1
                 man/fr/gplib.1
                 man/fr/gplink.1
                 man/fr/gpstrip.1
                 man/fr/gputils.1
                 man/fr/gpvc.1
                 man/fr/gpvo.1
                 gputils.spec])

AC_OUTPUT

AC_MSG_RESULT([

gputils-$PACKAGE_VERSION is now configured for $canonical_host_type

  Build:                $build
  Host:                 $host
  Source directory:     $srcdir
  Installation prefix:  $prefix
  C compiler:           $CC $CFLAGS
  Location of MPLAB-X:  $DEFAULT_MPLABX_PATH

])
