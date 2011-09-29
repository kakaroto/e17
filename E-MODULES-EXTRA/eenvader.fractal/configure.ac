dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(eenvaders, 0.1, billiob@efl.so)
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.6)
AM_CONFIG_HEADER(config.h)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_CC_C99
AC_HEADER_STDC
AC_C_CONST
AM_PROG_CC_C_O

define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

m4_ifdef([AM_GNU_GETTEXT_VERSION], [
AM_GNU_GETTEXT_VERSION([0.17])
])

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([EENVADER], [edje ecore-evas ecore evas eina])
datadir_edj=$($PKG_CONFIG --variable=prefix enlightenment)/share/enlightenment/data/backgrounds

EFL_WITH_BIN([edje], [edje-cc], [edje_cc])
vmaj=$($PKG_CONFIG --variable=vmaj edje)
MODULE_ARCH="$host_os-$host_cpu-$vmaj.0.0"
AC_SUBST(MODULE_ARCH)
AC_DEFINE_UNQUOTED(MODULE_ARCH, "$MODULE_ARCH", "Module architecture")

datadir=$(pkg-config --variable=modules edje)/${PACKAGE}
AC_ARG_ENABLE(homedir-install,
  AS_HELP_STRING([--enable-homedir-install], [Install module in homedir]),
  [
   datadir="${HOME}/.edje/modules/${PACKAGE}";
   datadir_edj="${HOME}/.e/e/backgrounds"
   ]
)
AC_SUBST(datadir_edj)

AC_OUTPUT([
Makefile
data/Makefile
src/Makefile
], [
])
