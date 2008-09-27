dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(edje_player, 0.1)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC

AM_PROG_LIBTOOL

dnl Check for option to enable debug
AC_MSG_CHECKING(whether to enable debugging)
AC_ARG_ENABLE(debug,
        [  --enable-debug=[no/yes]        enables debug build (default=no)],,
                 enable_debug=no)

dnl Checks for libraries
if [ ! test "x$enable_debug" != "xyes"]; then
  AC_DEFINE(DEBUG, 1, [Define to enable debug build])
  CFLAGS="${CFLAGS} -ggdb -O0"
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

AC_CHECK_LIB(popt,
             poptGetContext,
             LIBS="${PACKAGE_LIBS} -lpopt",
             echo "*** You need to get libpopt ***";exit
)


PKG_CHECK_MODULES(EDJE_PLAYER, [edje ecore ecore-evas ])
AC_SUBST(EDJE_PLAYER_CFLAGS)
AC_SUBST(EDJE_PLAYER_LIBS)




AC_OUTPUT([
Makefile
src/Makefile
])
