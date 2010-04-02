dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(evry-mpris, 0.1, hannes.janetzek@gmail.com)
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.8)
AM_CONFIG_HEADER(config.h)

define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION(0.14)


PKG_CHECK_MODULES(E, [enlightenment])
release=$(pkg-config --variable=release enlightenment)
MODULE_ARCH="$host_os-$host_cpu-$release"
AC_SUBST(MODULE_ARCH)
AC_DEFINE_UNQUOTED(MODULE_ARCH, "$MODULE_ARCH", "Module architecture")

PKG_CHECK_MODULES([EVRY], [everything])

AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AC_C___ATTRIBUTE__
datadir=$(pkg-config --variable=plugindir everything)/${PACKAGE}/${MODULE_ARCH}
AC_ARG_ENABLE(homedir-install, 
AS_HELP_STRING([--enable-homedir-install], [Install module in homedir]), 
[ datadir="${HOME}/.e/e/everything_plugins/plug_pidgin/${MODULE_ARCH}" ] 
) 
AC_CONFIG_FILES([
Makefile
src/Makefile
])

AC_OUTPUT