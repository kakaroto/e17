dnl Process this file with autoconf to produce a configure script.

AC_INIT([ecrire], [0.1.0], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
AM_CONFIG_HEADER([config.h])

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST

PKG_PROG_PKG_CONFIG

# GETTEXT stuff
m4_ifdef([AM_GNU_GETTEXT_VERSION], [
AM_GNU_GETTEXT_VERSION([0.18])
])

m4_ifdef([AM_GNU_GETTEXT], [
AM_GNU_GETTEXT([external])
po_makefile_in=po/Makefile.in
AM_CONDITIONAL([HAVE_PO], [true])
],[
AM_CONDITIONAL([HAVE_PO], [false])
])
AC_SUBST([LTLIBINTL])


dnl Checking for __attribute__ support
AC_MSG_CHECKING([for __attribute__])
AC_CACHE_VAL([_cv_have___attribute__],
  [
    AC_TRY_COMPILE([#include <stdlib.h>],
      [int func(int x); int foo(int x __attribute__ ((unused))) { exit(1); }],
      [_cv_have___attribute__="yes"],
      [_cv_have___attribute__="no"])
  ])

if test "x${_cv_have___attribute__}" = "xyes" ; then
  AC_DEFINE([HAVE___ATTRIBUTE__], [1], [Define to 1 if your compiler has __attribute__])
fi
AC_MSG_RESULT([${_cv_have___attribute__}])

PKG_CHECK_MODULES([EFL],
  [
   elementary >= 0.7.0
   edje >= 1.1.0
   ecore >= 1.0.0
  ])

PKG_CHECK_MODULES([ECORE_X], [ecore-x >= 1.0.0],
  [
    AC_DEFINE(HAVE_ECORE_X, 1, [X11 support])
  ],
    []
)
  

AC_OUTPUT([
Makefile
src/Makefile
src/bin/Makefile
$po_makefile_in
])
