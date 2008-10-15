#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

rm -f config.cache

AC_INIT(moon, 0.2.2, enlightenment-devel@lists.sourceforge.net)
AC_PREREQ(2.59)
AC_CONFIG_SRCDIR(src/e_mod_main.c)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.6)
AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST

# Checks for header files.
AC_CHECK_HEADERS([limits.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_STRUCT_TM

# Checks for library functions.
AC_CHECK_FUNCS([sqrt strdup])

MODULE_ARCH="$host_os-$host_cpu"
AC_SUBST(MODULE_ARCH)

define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION(0.14)

# Find edje_cc
PKG_CHECK_MODULES(EDJE, [edje >= 0.5.0])
AC_ARG_WITH(edje-cc,
  AC_HELP_STRING([--with-edje-cc=PATH], [specify a specific path to edje_cc]),
  [
    v=$withval;
    EDJE_CC=$v
  ],[
    EDJE_CC=$(pkg-config --variable=prefix edje)/bin/edje_cc
  ]
)
AC_SUBST(EDJE_CC)
AC_MSG_CHECKING([Which edje_cc to use])
AC_MSG_RESULT(${EDJE_CC})

PKG_CHECK_MODULES(E, [enlightenment])

datadir=$(pkg-config --variable=modules enlightenment)/${PACKAGE}
AC_ARG_ENABLE(homedir-install,
  AS_HELP_STRING([--enable-homedir-install], [Install module in homedir]),
  [ datadir="${HOME}/.e/e/modules/${PACKAGE}" ]
)

AC_CONFIG_FILES([Makefile
                 e_modules-moon.spec
                 data/Makefile
                 data/themes/Makefile
                 data/themes/images/Makefile
                 po/Makefile.in
                 src/Makefile])
AC_OUTPUT
