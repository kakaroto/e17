AC_INIT(etheme-winter.spec)

AM_INIT_AUTOMAKE(e16-theme-winter, 0.16.8)
AM_MAINTAINER_MODE

THEME=winter

dnl reasonable guesses for where stuff is installed
if test "x$prefix" = "xNONE"; then
  prefix="/usr/local"
fi
if test "x$exec_prefix" = "xNONE"; then
  exec_prefix=$prefix
fi

AC_PROG_MAKE_SET
AC_PROG_INSTALL

ENLIGHTENMENT_ROOT=`eval echo ${datadir}/e16`
AC_SUBST(ENLIGHTENMENT_ROOT)

AC_OUTPUT([
  Makefile
])
