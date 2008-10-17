dnl Process this file with autoconf to produce a configure script.

rm -f config.cache

AC_INIT(exchange, 0.0.0.001, mcalamelli@gmail.com)
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(config.h)

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST

AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

VMAJ=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $1);}'`
VMIN=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $2);}'`
VMIC=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $3);}'`
SNAP=`echo $PACKAGE_VERSION | awk -F. '{printf("%s", $4);}'`
version_info=`expr $VMAJ + $VMIN`":$VMIC:$VMIN"
AC_SUBST(version_info)

dnl Check enabled modules to build
AC_ARG_ENABLE(etk,
				  AC_HELP_STRING([--disable-etk],
									  [Disable ETK support]),
				  [enable_etk=$enableval],
				  [enable_etk="yes"])
AC_ARG_ENABLE(ewl,
				  AC_HELP_STRING([--disable-ewl], 
									  [Disable EWL support]),
				  [enable_ewl=$enableval],
				  [enable_ewl="yes"])

PKG_CHECK_MODULES(EXCHANGE, [
  eina-0
  ecore
  edje
  libxml-2.0
])

requirements="eina-0 ecore edje libxml-2.0"
AC_SUBST(requirements)

if test "x${enable_etk}" = "xyes"
then
	PKG_CHECK_MODULES(ETK, [etk], [], [enable_etk="no"])
   AC_DEFINE(HAVE_ETK, 1, [Set to 1 if ETK is installed])
fi

if test "x${enable_ewl}" = "xyes"
then
	PKG_CHECK_MODULES(EWL, [ewl], [], [enable_ewl="no"])
   AC_DEFINE(HAVE_EWL, 1, [Set to 1 if EWL is installed])
fi

AM_CONDITIONAL([BUILD_ETK], [test "x${enable_etk}" = "xyes"])
AM_CONDITIONAL([BUILD_EWL], [test "x${enable_ewl}" = "xyes"])

AC_OUTPUT([
exchange.pc
Makefile
src/Makefile
src/lib/Makefile
src/bin/Makefile
], []
)
