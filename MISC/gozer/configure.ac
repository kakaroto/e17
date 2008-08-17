dnl Process this file with autoconf to create configure.

AC_INIT
AC_CONFIG_SRCDIR([src/main.c])
AM_INIT_AUTOMAKE(gozer, 0.7)
AM_CONFIG_HEADER(src/config.h)

AC_PROG_CC
AM_PROG_CC_STDC
AC_C_CONST

AC_PROG_INSTALL
AC_PROG_MAKE_SET

AM_MAINTAINER_MODE

AC_ARG_WITH(imlib2,
        [  --with-imlib2=DIR       use imlib2 in <DIR>],
        [CFLAGS="$CFLAGS -I$withval/include"
        LIBS="-L$withval/lib $LIBS"])

AC_PATH_GENERIC(giblib, 1.0.0, [
  AC_SUBST(GIBLIB_LIBS)
  AC_SUBST(GIBLIB_CFLAGS) ],
  AC_MSG_ERROR(Cannot find giblib: Is giblib-config in the path?) )

LIBS="$LIBS -lm"
IMLIB_LIBS=`giblib-config --libs`
IMLIB_CFLAGS=`giblib-config --cflags`
AC_SUBST(GIBLIB_LIBS)
AC_SUBST(GIBLIB_CFLAGS)

AC_CHECK_FUNC(getopt_long,,[LIBOBJS="$LIBOBJS getopt.o getopt1.o"])
AC_SUBST(LIBOBJS)

AC_CONFIG_FILES([Makefile \
gozer.spec \
src/Makefile \
src/fonts/Makefile \
])
AC_OUTPUT
