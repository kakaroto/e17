dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(eflxx_examples, 0.1)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CXX
AM_PROG_CC_STDC
AC_HEADER_STDC






AM_PROG_LIBTOOL




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


dnl Set PACKAGE SOURCE DIR in config.h.
packagesrcdir=`cd $srcdir && pwd`
PACKAGE_SOURCE_DIR="${packagesrcdir}"
AC_SUBST(PACKAGE_SOURCE_DIR)

dnl Set PACKAGE PREFIX
if test "x${prefix}" = "xNONE"; then
  packageprefix=${ac_default_prefix}
else
  packageprefix=${prefix}
fi

dnl Set PACKAGE DATA & DOC DIR
packagedatadir=share/${PACKAGE}
packagedocdir=doc/${PACKAGE}

dnl Set PACKAGE DIRS in config.h.
packagepixmapsdir=${packagedatadir}/pixmaps
packagehelpdir=${packagedatadir}/help
packagemenudir=${packagedatadir}

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

dnl Subst PACKAGE_HELP_DIR.
NO_PREFIX_PACKAGE_HELP_DIR="${packagehelpdir}"
AC_SUBST(NO_PREFIX_PACKAGE_HELP_DIR)
PACKAGE_HELP_DIR="${packageprefix}/${packagehelpdir}"
AC_SUBST(PACKAGE_HELP_DIR)

dnl Subst PACKAGE_MENU_DIR.
NO_PREFIX_PACKAGE_MENU_DIR="${packagemenudir}"
AC_SUBST(NO_PREFIX_PACKAGE_MENU_DIR)
PACKAGE_MENU_DIR="${packageprefix}/${packagemenudir}"
AC_SUBST(PACKAGE_MENU_DIR)

AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${packageprefix}/${packagedatadir}","")
AC_DEFINE_UNQUOTED(SYSTEM_DATA_DIR, "${packageprefix}/${systemdatadir}","")
AC_DEFINE_UNQUOTED(PACKAGE_DOC_DIR, "${packageprefix}/${packagedocdir}","")
AC_DEFINE_UNQUOTED(PACKAGE_PIXMAPS_DIR, "${packageprefix}/${packagepixmapsdir}","")
AC_DEFINE_UNQUOTED(SYSTEM_PIXMAPS_DIR, "${packageprefix}/${systempixmapsdir}","")
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}","")


PKG_CHECK_MODULES(EDJEXX, edjexx)
PKG_CHECK_MODULES(EFL, eflxx)
PKG_CHECK_MODULES(ECOREXX, ecorexx)
PKG_CHECK_MODULES(EETXX, eetxx)
PKG_CHECK_MODULES(EMOTIONXX, emotionxx)
PKG_CHECK_MODULES(ELEMENTARYXX, elementaryxx)
PKG_CHECK_MODULES(EINAXX, einaxx)
AC_OUTPUT([
Makefile
data/Makefile
data/edjes/Makefile
data/fonts/Makefile
data/images/Makefile
src/Makefile
src/common/Makefile
src/ecorexx/Makefile
src/ecorexx/3dstars/Makefile
src/ecorexx/calibrate/Makefile
src/ecorexx/callbacks/Makefile
src/ecorexx/list/Makefile
src/ecorexx/paint/Makefile
src/ecorexx/simple/Makefile
src/ecorexx/stars/Makefile
src/ecorexx/timers/Makefile
src/ecorexx/x/Makefile
src/edjexx/Makefile
src/edjexx/simple/Makefile
src/eetxx/Makefile
src/eetxx/simple_write_read/Makefile
src/emotionxx/Makefile
src/emotionxx/simple/Makefile
src/emotionxx/audio/Makefile
src/elementaryxx/Makefile
src/elementaryxx/simple/Makefile
src/elementaryxx/full/Makefile
src/elementaryxx/external/Makefile
src/einaxx/Makefile
src/einaxx/list/Makefile
src/debug/Makefile
src/debug/simple/Makefile
src/signals/Makefile
src/signals/simple/Makefile
])
