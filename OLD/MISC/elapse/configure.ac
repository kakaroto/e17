dnl Process this file with autoconf to produce a configure script.

AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(elapse, 0.0.1)
AM_CONFIG_HEADER(src/config.h)

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_SUBST(DEBIANSTAMP)
if test "$DEBINFO"; then
  DEBIANSTAMP="$DEBINFO  `date "+%a, %d %b %G %T %z"`"
else
  DEBIANSTAMP="`grep "Maintainer:" debian/control | cut -d" " -f2-`  `date "+%a, %d %b %G %T %z"`"
fi

AC_EXPAND_DIR(PACKAGE_DATA_DIR, "$datadir/${PACKAGE}")
AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "$PACKAGE_DATA_DIR", [Path to data files])

if test "x${prefix}" = "xNONE"; then
	AC_DEFINE_UNQUOTED(PREFIX, "/usr/local/", [Base install path])
else
	AC_DEFINE_UNQUOTED(PREFIX, "${prefix}", [Base install path])
fi

AC_EXPAND_DIR(PACKAGE_CFG_DIR, "$sysconfdir")
AC_DEFINE_UNQUOTED(PACKAGE_CFG_DIR, "$PACKAGE_CFG_DIR", [Config file path])

dnl Set PACKAGE_SOURCE_DIR in config.h.
packagesrcdir=`cd $srcdir && pwd`
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}", [Path to source files])

dnl Use -Wall if we have gcc.
changequote(,)dnl
if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;;
  esac
fi
changequote([,])dnl

PKG_CHECK_MODULES(ELAPSE,
		  edje
		  esmart_trans_x11)

AC_OUTPUT([
Makefile
src/Makefile
data/Makefile
debian/changelog
])
