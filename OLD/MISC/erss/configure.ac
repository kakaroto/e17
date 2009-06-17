dnl Process this file with autoconf to produce a configure script.

AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(erss, 0.0.2)
AM_CONFIG_HEADER(src/config.h)

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC

AC_EXPAND_DIR(PACKAGE_DATA_DIR, "$datadir/${PACKAGE}")
AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "$PACKAGE_DATA_DIR")

if test "x${prefix}" = "xNONE"; then
	AC_DEFINE_UNQUOTED(PREFIX, "/usr/local/")
else
	AC_DEFINE_UNQUOTED(PREFIX, "${prefix}")
fi

AC_EXPAND_DIR(PACKAGE_CFG_DIR, "$sysconfdir")
AC_DEFINE_UNQUOTED(PACKAGE_CFG_DIR, "$PACKAGE_CFG_DIR")

dnl Set PACKAGE_SOURCE_DIR in config.h.
packagesrcdir=`cd $srcdir && pwd`
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}")

dnl Use -Wall if we have gcc.
changequote(,)dnl
if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;;
  esac
fi
changequote([,])dnl

AC_PATH_GENERIC(ecore, 0.9.9, [
  AC_SUBST(ecore_libs)
	AC_SUBST(ecore_cflags) ],
	AC_MSG_ERROR(Cannot find ecore: Is ecore-config in path?))
ecore_libs=`ecore-config --libs`
ecore_cflags=`ecore-config --cflags`
AC_SUBST(ecore_libs)
AC_SUBST(ecore_cflags)

AC_PATH_GENERIC(edje, 0.5.0, [
  AC_SUBST(edje_libs)
	AC_SUBST(edje_cflags) ],
	AC_MSG_ERROR(Cannot find edje: Is edje-config in path?))
edje_libs=`edje-config --libs`
edje_cflags=`edje-config --cflags`
AC_SUBST(edje_libs)
AC_SUBST(edje_cflags)

AC_PATH_GENERIC(esmart, 0.9.0, [
  AC_SUBST(esmart_libs)
	AC_SUBST(esmart_cflags) ],
	AC_MSG_ERROR(Cannot find esmart: Is esmart-config in path?))
esmart_libs=`esmart-config --libs`
esmart_cflags=`esmart-config --cflags`
AC_SUBST(edje_libs)
AC_SUBST(edje_cflags)

AC_PATH_GENERIC(etox, 0.0.2, [
	AC_SUBST(etox_libs)
	AC_SUBST(etox_cflags) ],
	AC_MSG_ERROR(Cannot find etox: Is etox-config in path?))
etox_libs=`etox-config --libs`
etox_cflags=`etox-config --cflags`
AC_SUBST(etox_libs)
AC_SUBST(etox_cflags)



AC_PATH_GENERIC(xml2, 2.6.0, [
	AC_SUBST(xml2_libs)
	AC_SUBST(xml2_cflags) ],
	AC_MSG_ERROR(Cannot find libxml2: Is xml2-config in path?))
xml2_libs=`xml2-config --libs`
xml2_cflags=`xml2-config --cflags`
AC_SUBST(xml2_libs)
AC_SUBST(xml2_cflags)


AC_OUTPUT([
Makefile
src/Makefile
data/Makefile
data/fonts/Makefile
data/config/Makefile
])
