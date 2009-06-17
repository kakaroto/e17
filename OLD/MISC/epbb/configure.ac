dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(epbb, 0.0.5)
AM_CONFIG_HEADER(src/config.h)

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AM_ENABLE_SHARED
AM_PROG_LIBTOOL

AM_WITH_DMALLOC

dnl Add the languages which your application supports here.
ALL_LINGUAS=""

dnl Set PACKAGE_LOCALE_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${ac_default_prefix}/${DATADIRNAME}/locale", [Locale-specific data directory])
else
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${prefix}/${DATADIRNAME}/locale", [Locale-specific data directory])
fi

dnl Set PACKAGE_DATA_DIR in config.h.
if test "x${datadir}" = 'x${prefix}/share'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}", [Shared data directory])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}", [Shared data directory])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${datadir}/${PACKAGE}", [Shared data directory])
fi

dnl Set PACKAGE_BIN_DIR in config.h.
if test "x${datadir}" = 'x${prefix}/bin'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${ac_default_prefix}/bin", [Installation directory for user executables])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${prefix}/bin", [Installation directory for user executables])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${bindir}", [Installation directory for user executables])
fi

dnl Set PACKAGE_LIB_DIR in config.h.
if test "x${datadir}" = 'x${prefix}/lib'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${ac_default_prefix}/lib", [Installation directory for libraries])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${prefix}/lib", [Installation directory for libraries])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${libdir}", [Installation directory for libraries])
fi

dnl Set PACKAGE_SOURCE_DIR in config.h.
packagesrcdir=`cd $srcdir && pwd`
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}", [Source code directory])

PKG_CHECK_MODULES(EVAS, [evas])
PKG_CHECK_MODULES(ECORE, [ecore])
PKG_CHECK_MODULES(EDJE, [edje])

pbb_cflags=
pbb_libs=
if test -d "../pbbuttonsd"; then
  curpath=`pwd`
  PBB_CFLAGS="-I${curpath}/../pbbuttonsd/libpbbipc"
  PBB_LDFLAGS="-L${curpath}/../pbbuttonsd/libpbbipc"
  PBB_LDADD="-lpbb"
  echo "Use develpment version of libpbb."
else
  AC_CHECK_LIB([pbb], [init_libpbb], [pbb_libs="-lpbb"], [AC_MSG_ERROR([Cannot find libpbb, please compile and install it first.])])
  AC_CHECK_LIB([pbb], [taglist_count], [], [AC_DEFINE_UNQUOTED([HAVE_OLDLIBPBB], [1], [latest libpbb version available])])
fi
AC_SUBST(pbb_cflags)
AC_SUBST(pbb_libs)

AC_OUTPUT([
Makefile 
src/Makefile 
src/bin/Makefile
data/Makefile
data/fonts/Makefile
data/themes/Makefile
data/themes/default/Makefile
])

