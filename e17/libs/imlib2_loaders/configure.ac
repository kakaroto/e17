dnl Process this file with autoconf to create configure.

AC_INIT
AC_CONFIG_SRCDIR([src/loader_xcf.c])
AM_INIT_AUTOMAKE(imlib2_loaders, 0.0.1)
AM_CONFIG_HEADER(config.h)

AC_C_BIGENDIAN	      

AC_PROG_CC
AM_PROG_CC_STDC
AC_C_CONST

AC_PROG_INSTALL
AC_PROG_MAKE_SET
AM_PROG_LIBTOOL

AC_PATH_X
AC_PATH_XTRA

dnl Check for Imlib2.h
AC_CHECK_HEADER(Imlib2.h, , AC_MSG_ERROR([Imlib2.h not found.]))

# Test for libedb
AC_CHECK_LIB(edb, e_db_open,
  edb_ok=yes,
  edb_ok=no
  AC_MSG_WARN(*** Native EDB support will not be built (EDB library not found) ***), $EXTRA_LIBS -lz)
if test "$edb_ok" = yes; then
  AC_MSG_CHECKING([for Edb.h])
  AC_TRY_CPP(
  [#include <stdio.h>
   #undef PACKAGE
   #undef VERSION
   #include <Edb.h>],
  edb_ok=yes,
  edb_ok=no)
  AC_MSG_RESULT($edb_ok)
  if test "$edb_ok" = yes; then
    EDBLIBS="-ledb"
  else
    AC_MSG_WARN(*** Native EDB support will not be built (EDB header file not found) ***)
  fi
fi

AM_CONDITIONAL(EDB_LOADER, test "$edb_ok" = yes)

AC_SUBST(EDBLIBS)

AC_CONFIG_FILES([
Makefile
src/Makefile
debian/Makefile
imlib2_loaders.spec
])
AC_OUTPUT
