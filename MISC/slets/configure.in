dnl Process this file with autoconf to produce a configure script.
AC_INIT(src/slets/main.h)
AM_INIT_AUTOMAKE(slets, 0.0.1)

dnl Checks for programs.
AC_PROG_CC
AM_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

dnl Checks for libraries.
dnl Replace `main' with a function in -lesd:
AC_CHECK_LIB(esd, esd_play_file)
dnl Replace `main' with a function in -levas:
AC_CHECK_LIB(evas, evas_new)

dnl Checks for header files.
AC_HEADER_STDC

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.
AC_CHECK_FUNCS(strstr)

if test "${prefix}" = "NONE"; then
  prefix="/usr/local"
fi
DATA_DIR=`eval eval eval eval eval echo "${datadir}/${PACKAGE}"`

AC_DEFINE_UNQUOTED(DATA_DIR, "${DATA_DIR}")

AM_CONFIG_HEADER(config.h)
AC_OUTPUT(Makefile src/Makefile src/slethe/Makefile src/slets/Makefile)
