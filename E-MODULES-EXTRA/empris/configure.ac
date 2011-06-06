dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(empris, 0.0.1, hannes.janetzek@gmail.com)
AC_PREREQ(2.59)
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE(1.8)

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AC_PROG_CC
AC_HEADER_STDC
AC_HEADER_TIME

define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

ALL_LINGUAS=""
AC_SUBST(ALL_LINGUAS)

PKG_CHECK_MODULES(E, [enlightenment])
release=$(pkg-config --variable=release enlightenment)
MODULE_ARCH="$host_os-$host_cpu-$release"
AC_SUBST(MODULE_ARCH)
AC_DEFINE_UNQUOTED(MODULE_ARCH, "$MODULE_ARCH", "Module architecture")

uname=`uname`

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

datadir=$(pkg-config --variable=modules enlightenment)/${PACKAGE}


AC_ARG_ENABLE(homedir-install,
  AS_HELP_STRING([--enable-homedir-install], [Install module in homedir]),
  [ datadir="${HOME}/.e/e/modules/${PACKAGE}" ]
)

AC_OUTPUT([
Makefile
src/Makefile
e_modules-empris.spec
module.desktop
],[
])
