AC_INIT(clouseau, 0.1.0, tom@stosb.com)
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(config.h)

AC_PROG_LIBTOOL

dnl Checking for __attribute__ support
AC_MSG_CHECKING([for __attribute__])
AC_CACHE_VAL(_cv_have___attribute__,
  [
    AC_TRY_COMPILE([#include <stdlib.h>],
      [int func(int x); int foo(int x __attribute__ ((unused))) { exit(1); }],
      [_cv_have___attribute__="yes"],
      [_cv_have___attribute__="no"])
  ]
)

if test "x${_cv_have___attribute__}" = "xyes" ; then
   AC_DEFINE(HAVE___ATTRIBUTE__, 1, [Define to 1 if your compiler has __attribute__])
fi
AC_MSG_RESULT(${_cv_have___attribute__})

PKG_CHECK_MODULES(EFL,
  [
   elementary >= 0.7.0
   evas >= 1.0.0
  ]
)

AC_CHECK_LIB(dl,dlopen)
AC_CHECK_FUNCS(dlopen dlerror)

AC_OUTPUT([
           Makefile	
           src/Makefile
           src/lib/Makefile
           data/Makefile
           data/images/Makefile
           ])
