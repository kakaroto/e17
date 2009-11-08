dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(eweather, 0.2.0, enlightenment-devel@lists.sourceforge.net)
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.6)
AM_CONFIG_HEADER(config.h)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AM_PROG_CC_C_O

define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

AC_SUBST(packagedatadir)

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES(E, [enlightenment])

pluginsdir="${libdir}/eweather/plugins"
AC_SUBST(pluginsdir)
AS_AC_EXPAND(PLUGINSDIR, $pluginsdir)
AC_DEFINE_UNQUOTED(PLUGINSDIR, ["$PLUGINSDIR"], [Where plugins are installed.])

AC_CHECK_LIB(dl, dlopen, dlopen_libs=-ldl)
AC_SUBST(dlopen_libs)

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


AC_OUTPUT([
Makefile
eweather.pc
data/Makefile
data/images/Makefile
src/Makefile
src/lib/Makefile
src/plugins/Makefile
src/plugins/test/Makefile
src/plugins/yahoo/Makefile
], [
])
