dnl Process this file with autoconf to produce a configure script.

AC_INIT
AC_CONFIG_SRCDIR([configure.in])
AM_INIT_AUTOMAKE(etox, 0.0.1)
AM_CONFIG_HEADER(etox-config.h)

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AM_ENABLE_SHARED
AM_PROG_LIBTOOL

dnl Set PACKAGE_LOCALE_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${ac_default_prefix}/${DATADIRNAME}/locale")
else
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${prefix}/${DATADIRNAME}/locale")
fi

dnl Set PACKAGE_DATA_DIR in config.h.
if test "x${datadir}" = 'x${prefix}/share'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}")
  else
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}")
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${datadir}/${PACKAGE}")
fi

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

AC_SUBST(ebits_cflags)
AC_SUBST(ebits_libs)
ebits_cflags=`ebits-config --cflags`
ebits_libs=`ebits-config --libs`

AC_SUBST(edb_cflags)
AC_SUBST(edb_libs)
edb_cflags=`edb-config --cflags`
edb_libs=`edb-config --libs`

AC_SUBST(ecore_cflags)
AC_SUBST(ecore_libs)
ecore_cflags=`ecore-config --cflags`
ecore_libs=`ecore-config --libs`

AC_SUBST(evas_cflags)
AC_SUBST(evas_libs)
evas_cflags=`evas-config --cflags`
evas_libs=`evas-config --libs`

AC_SUBST(ewd_cflags)
AC_SUBST(ewd_libs)
ewd_cflags=`ewd-config --cflags`
ewd_libs=`ewd-config --libs`

AC_SUBST(estyle_cflags)
AC_SUBST(estyle_libs)
estyle_cflags=`estyle-config --cflags`
estyle_libs=`estyle-config --libs`

AC_SUBST(imlib2_cflags)
AC_SUBST(imlib2_libs)
imlib2_cflags=`imlib2-config --cflags`
imlib2_libs=`imlib2-config --libs`

dnl Look for jade for sgml translations.
AC_ARG_WITH(dbsheets,
       [  --with-dbsheets=DIR     use DIR to specify your DocBook stylesheets installation path.],
       DB_STYLESHEETS="$withval", DB_STYLESHEETS="/usr/lib/sgml/stylesheet/dsssl/docbook/nwalsh")
AC_SUBST(DB_STYLESHEETS)
AC_PATH_PROG(JADE, jade)
AM_CONDITIONAL(HAVE_JADE, test "x$JADE" != "x" && test -d "$DB_STYLESHEETS")

AC_CONFIG_FILES([
Makefile
etox-config
src/Makefile
doc/kernel-doc
doc/html-customizations.dsl
doc/Makefile
test/Makefile
test/img/Makefile
])
AC_CONFIG_COMMANDS([default],[[
chmod +x etox-config

]],[[]])
AC_OUTPUT
