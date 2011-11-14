AC_INIT(ensure, 0.2.0, nash@nash.id.au)
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(config.h)
AH_TOP([
#ifndef EFL_CONFIG_H__
#define EFL_CONFIG_H__
])
AH_BOTTOM([
#endif /* EFL_CONFIG_H__ */
])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_GNU_SOURCE
AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AC_C___ATTRIBUTE__

AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

PKG_CHECK_MODULES([ELEMENTARY], elementary)
AC_LIBTOOL_DLOPEN
AC_CHECK_LIB([dl], [dlopen], [],AC_ERROR([Usable libdl not found! Apollo port not yet complete.]))

AC_OUTPUT(Makefile	
		src/Makefile
		src/assurances/Makefile)
