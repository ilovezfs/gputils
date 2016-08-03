#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.60)
AC_INIT([gputils],[1.4.7], [<URL:http://gputils.sourceforge.net/>])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([dist-bzip2])
AM_MAINTAINER_MODE

# Determine the host and build type. The target is always a PIC.
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

# Defaults.
GPUTILS_HEADER_PATH="\$(datadir)/gputils/header"
GPUTILS_LKR_PATH="\$(datadir)/gputils/lkr"
GPUTILS_LIB_PATH="\$(datadir)/gputils/lib"
GPUTILS_HTMLDOC_PATH=${GPUTILS_HTMLDOC_PATH:-"\$(datadir)/doc/gputils-\$(PACKAGE_VERSION)/html"}
LIBGPUTILS=libgputils/libgputils.a
LIBIBERTY=libiberty/libiberty.a
MPLABX_PATH_DEFAULT="/opt/microchip/mplabx/v3.35"
MAKE_SUBDIRS="libgputils libiberty gpasm gplink gputils header lkr doc man"

# Check for additional parameters.
AC_ARG_ENABLE(debug,
  [  --enable-debug          enable gputils debug features],
  [enable_gputils_debug=yes])

AC_ARG_ENABLE(gdb-debuginfo,
  [  --enable-gdb-debuginfo  enable debugging information for gdb],
  [enable_gdb_debuginfo=yes])

AC_ARG_ENABLE(path,
  [  --disable-path          disable adding default search paths],
  [], [enable_gputils_path=yes])

AC_ARG_ENABLE(html-doc,
  [  --disable-html-doc      disable creating the html device doc],
  [ENABLE_HTML_DOC="$enableval"],
  [ENABLE_HTML_DOC=yes])

AC_ARG_WITH(mplabx-path,
  AC_HELP_STRING([--with-mplabx-path=DIR],
                 [location of MPLAB-X (default=${MPLABX_PATH_DEFAULT})]),
  [DEFAULT_MPLABX_PATH="$withval"],
  [DEFAULT_MPLABX_PATH="${MPLABX_PATH_DEFAULT}"])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_YACC
AM_PROG_LEX
set `$LEX -V`
if test "${2%.*}" != "2.5" && test "${2##*.}" -ge 4; then
  AC_MSG_WARN(gputils supports flex version 2.5.4)
fi
AC_PROG_INSTALL

# Append the host alias to the tools for cross compiling.
AC_CHECK_TOOL(RANLIB, ranlib, :)
AC_CHECK_TOOL(AR, ar, :)

AC_ARG_VAR([LIB_TYPE], [library type: LIB, AR or RANLIB (default)])
if test "x${LIB_TYPE}" = "x"; then
  LIB_TYPE="RANLIB"
fi

# Checks for header files.
AC_HEADER_STDC

AC_CHECK_HEADERS([libintl.h langinfo.h locale.h malloc.h stdlib.h string.h \
strings.h sys/ioctl.h termios.h unistd.h windows.h])

AC_CHECK_DECLS([asprintf, basename, getopt, vasprintf])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T 
AC_STRUCT_TM
AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)

type_name()
{
  if expr "$ac_cv_sizeof_char" '==' "$1" >/dev/null; then
    echo "char"
    exit
  fi
  if expr "$ac_cv_sizeof_short" '==' "$1" >/dev/null; then
    echo "short"
    exit
  fi
  if expr "$ac_cv_sizeof_int" '==' "$1" >/dev/null; then
    echo "int"
    exit
  fi
  if expr "$ac_cv_sizeof_long" '==' "$1" >/dev/null; then
    echo "long"
    exit
  fi
  if expr "$ac_cv_sizeof_long_long" '==' "$1" >/dev/null; then
    echo "long long"
    exit
  fi
  echo "long"
}

type_size()
{
  case "$1" in
    char)      __ac_tmp=$ac_cv_sizeof_char ;;
    short)     __ac_tmp=$ac_cv_sizeof_short ;;
    int)       __ac_tmp=$ac_cv_sizeof_int ;;
    long)      __ac_tmp=$ac_cv_sizeof_long ;;
    long_long) __ac_tmp=$ac_cv_sizeof_long_long ;;
    *)         __ac_tmp=$ac_cv_sizeof_int ;;
  esac
  expr $__ac_tmp '*' 8
}

AC_MSG_CHECKING(type name for int8_t)
TYPE_INT8=`type_name 1`
AC_MSG_RESULT($TYPE_INT8)

AC_MSG_CHECKING(type name for int16_t)
TYPE_INT16=`type_name 2`
AC_MSG_RESULT($TYPE_INT16)

AC_MSG_CHECKING(type name for int32_t)
TYPE_INT32=`type_name 4`
AC_MSG_RESULT($TYPE_INT32)

AC_MSG_CHECKING(type name for int64_t)
TYPE_INT64=`type_name 8`
AC_MSG_RESULT($TYPE_INT64)

AC_DEFINE_UNQUOTED(TYPE_INT8,  [$TYPE_INT8],  [int8_t])
AC_DEFINE_UNQUOTED(TYPE_INT16, [$TYPE_INT16], [int16_t])
AC_DEFINE_UNQUOTED(TYPE_INT32, [$TYPE_INT32], [int32_t])
AC_DEFINE_UNQUOTED(TYPE_INT64, [$TYPE_INT64], [int64_t])

NATIVE_WORD_SIZE=`type_size long`
AC_DEFINE_UNQUOTED(NATIVE_WORD_SIZE, [$NATIVE_WORD_SIZE], [Native word size.])

# Checks for library functions.
AC_CHECK_FUNCS([strcasecmp strncasecmp stricmp strnicmp strndup realpath])

# Check for functions to compile into libiberty (getopt is always compiled).
IBERTYOBJS=
AC_CHECK_FUNC(vsnprintf, , [IBERTYOBJS="$IBERTYOBJS vasprintf.o vsnprintf.o"])
AC_CHECK_FUNC(snprintf, , [IBERTYOBJS="$IBERTYOBJS snprintf.o"])
AC_CHECK_FUNC(strndup, , [IBERTYOBJS="$IBERTYOBJS strndup.o"])
AC_CHECK_FUNC(strverscmp, , [IBERTYOBJS="$IBERTYOBJS strverscmp.o"])

AM_CFLAGS=
AM_LDFLAGS=

# Options for the system on which the package will run.
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

VERSION_MAJOR=`echo $PACKAGE_VERSION | $AWK 'BEGIN {FS="."} {print $1}'`
VERSION_MINOR=`echo $PACKAGE_VERSION | $AWK 'BEGIN {FS="."} {print $2}'`
VERSION_MICRO=`echo $PACKAGE_VERSION | $AWK 'BEGIN {FS="."} {print $3}'`

AC_DEFINE_UNQUOTED(GPUTILS_VERSION_MAJOR, $VERSION_MAJOR, [gputils major version number])
AC_DEFINE_UNQUOTED(GPUTILS_VERSION_MINOR, $VERSION_MINOR, [gputils minor version number])
AC_DEFINE_UNQUOTED(GPUTILS_VERSION_MICRO, $VERSION_MICRO, [gputils micro version number])
AC_DEFINE_UNQUOTED(GPUTILS_VERSION_STR, "$PACKAGE_VERSION", [gputils version string])

AM_GPUTILS_SVN_VERSION="\$(shell \$(top_srcdir)/get_cl_revision.sh \$(top_srcdir)/ChangeLog)"
AM_PACKAGE_SVN_VERSION=`./get_cl_revision.sh ChangeLog`
CFLAGS="-DGPUTILS_SVN_VERSION=\$(AM_GPUTILS_SVN_VERSION)"

# Host filesystem options.
case "${host}" in
  *mingw* | *-pc-os2_emx | *-pc-os2-emx | *djgpp* )
    AC_DEFINE(HAVE_DOS_BASED_FILE_SYSTEM, 1, 
      [Define if your host uses a DOS based file system.])
    ;;
esac

# Process the options.
if test x$enable_gputils_debug = xyes; then
  AC_DEFINE(GPUTILS_DEBUG, 1,
    [Define if you want to enable GPUTILS debug features.])
fi

if test x$enable_gdb_debuginfo = xyes; then
  CFLAGS="$CFLAGS -ggdb"
fi

if test x$enable_gputils_path = xyes; then
  AC_DEFINE(USE_DEFAULT_PATHS, 1,
    [Define if you want to add default search paths.])
fi

LDFLAGS=

# Substitute configuration variables.
AC_SUBST(ENABLE_HTML_DOC)
AC_SUBST(DEFAULT_MPLABX_PATH)
AC_SUBST(AM_GPUTILS_SVN_VERSION)
AC_SUBST(AM_PACKAGE_SVN_VERSION)
AC_SUBST(AM_CFLAGS)
AC_SUBST(AM_LDFLAGS)
AC_SUBST(CFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(GPUTILS_HEADER_PATH)
AC_SUBST(GPUTILS_LKR_PATH)
AC_SUBST(GPUTILS_LIB_PATH)
AC_SUBST(GPUTILS_HTMLDOC_PATH)
AC_SUBST(IBERTYOBJS)
AC_SUBST(LIBGPUTILS)
AC_SUBST(LIBIBERTY)
AC_SUBST(MAKE_SUBDIRS)

# Generate Makefiles.
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

$PACKAGE_NAME-$PACKAGE_VERSION is now configured for $canonical_host_type

  Build:                $build
  Host:                 $host
  Source directory:     $srcdir
  Installation prefix:  $prefix
  C compiler:           $CC $CFLAGS
  Location of MPLAB-X:  $DEFAULT_MPLABX_PATH

])
