dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta - will *NOT* be overwritten

AC_INIT(EFLpp, 0.1, mickey@vanille-media.de)
AM_INIT_AUTOMAKE(eflpp, 0.1)
AM_CONFIG_HEADER(config.h)

dnl Disable libtool 1.5 support for languages we don't use
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_GCJ_CONFIG], [:])dnl

AM_MAINTAINER_MODE

AC_PROG_CC
AM_PROG_CC_STDC
AC_PROG_CPP
AC_PROG_CXX
AC_HEADER_STDC
AC_PROG_LIBTOOL

dnl Check for option to enable debug
AC_MSG_CHECKING(whether to enable debugging)
AC_ARG_ENABLE(debug,
        [  --enable-debug=[no/yes]        enables debug build (default=no)],,
                 enable_debug=no)

dnl Check for programs

dnl Checks for libraries
dnl Check for pkg-config stuff
pkg_modules="evas ecore ecore-config ecore-evas sigc++-2.0 esmart_container etk esmart_text_entry ewl"
PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

dnl Check for headers

dnl Check for functions

dnl Check for edce_cc
EDJE_DEF=""
AC_SUBST(EDJE_DEF)
AC_ARG_WITH(edje-cc,
        AC_HELP_STRING([--with-edje-cc=PATH],[Specify a specific path to edje_cc]),
[
  v=$withval;
  edje_cc=$v
  echo "  Enlightenment edje_cc explicitly set to "$edje_cc;
],[
  edje_cc=$(pkg-config --variable=prefix edje)/bin/edje_cc
])
AC_SUBST(edje_cc)

dnl Set PACKAGE SOURCE DIR
packagesrcdir=`cd $srcdir && pwd`

dnl Set PACKAGE PREFIX
if test "x${prefix}" = "xNONE"; then
  packageprefix=${ac_default_prefix}
else
  packageprefix=${prefix}
fi

dnl Set PACKAGE DIRS in config.h
packagedatadir=share/${PACKAGE}
packagedocdir=doc/${PACKAGE}
packagepixmapsdir=share/pixmaps/${PACKAGE}
systempixmapsdir=share/pixmaps

dnl Subst PACKAGE_DATA_DIR.
NO_PREFIX_PACKAGE_DATA_DIR="${packagedatadir}"
AC_SUBST(NO_PREFIX_PACKAGE_DATA_DIR)
PACKAGE_DATA_DIR="${packageprefix}/${packagedatadir}"
AC_SUBST(PACKAGE_DATA_DIR)

dnl Subst PACKAGE_DOC_DIR.
NO_PREFIX_PACKAGE_DOC_DIR="${packagedocdir}"
AC_SUBST(NO_PREFIX_PACKAGE_DOC_DIR)
PACKAGE_DOC_DIR="${packageprefix}/${packagedocdir}"
AC_SUBST(PACKAGE_DOC_DIR)

dnl Subst PACKAGE_PIXMAPS_DIR.
NO_PREFIX_PACKAGE_PIXMAPS_DIR="${packagepixmapsdir}"
AC_SUBST(NO_PREFIX_PACKAGE_PIXMAPS_DIR)
PACKAGE_PIXMAPS_DIR="${packageprefix}/${packagepixmapsdir}"
AC_SUBST(PACKAGE_PIXMAPS_DIR)

dnl Subst SYSTEM_PIXMAPS_DIR.
NO_PREFIX_SYSTEM_PIXMAPS_DIR="${packagepixmapsdir}"
AC_SUBST(NO_PREFIX_SYSTEM_PIXMAPS_DIR)
SYSTEM_PIXMAPS_DIR="${packageprefix}/${systempixmapsdir}"
AC_SUBST(SYSTEM_PIXMAPS_DIR)

dnl Subst PACKAGE_SOURCE_DIR
PACKAGE_SOURCE_DIR="${packagesrcdir}"
AC_SUBST(PACKAGE_SOURCE_DIR)

AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${packageprefix}/${packagedatadir}","")
AC_DEFINE_UNQUOTED(PACKAGE_DOC_DIR, "${packageprefix}/${packagedocdir}","")
AC_DEFINE_UNQUOTED(PACKAGE_PIXMAPS_DIR, "${packageprefix}/${packagepixmapsdir}","")
AC_DEFINE_UNQUOTED(SYSTEM_PIXMAPS_DIR, "${packageprefix}/${systempixmapsdir}","")
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}","")

AC_OUTPUT([
eflpp.pc
eflpp-uninstalled.pc
Makefile
src/Makefile
src/common/Makefile
src/ecore/Makefile
src/edje/Makefile
src/esmart/Makefile
src/evas/Makefile
src/edb/Makefile
src/eet/Makefile
src/emotion/Makefile
src/etk/Makefile
src/ewl/Makefile
src/misc/Makefile
examples/Makefile
examples/edje/Makefile
examples/edje/simple/Makefile
examples/esmart/Makefile
examples/esmart/container/Makefile
examples/esmart/textentry/Makefile
examples/etk/Makefile
examples/etk/embed/Makefile
examples/etk/simple/Makefile
examples/ewl/Makefile
examples/ewl/embed/Makefile
examples/ewl/simple/Makefile
examples/signals/Makefile
examples/signals/simple/Makefile
examples/emotion/Makefile
examples/emotion/simple/Makefile
examples/debug/Makefile
examples/debug/simple/Makefile
examples/ecore/Makefile
examples/ecore/3dstars/Makefile
examples/ecore/calibrate/Makefile
examples/ecore/callbacks/Makefile
examples/ecore/config/Makefile
examples/ecore/paint/Makefile
examples/ecore/simple/Makefile
examples/ecore/stars/Makefile
examples/ecore/timers/Makefile
examples/angstrom/Makefile
examples/angstrom/bootmanager/Makefile
data/Makefile
data/edjes/Makefile
data/fonts/Makefile
data/images/Makefile
])
