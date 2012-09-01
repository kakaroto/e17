dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT([empower], [3.0.0], [mekius@mekius.net])
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AM_PROG_CC_STDC
AM_PROG_CC_C_O
AC_HEADER_STDC
AC_C_CONST
AM_ENABLE_SHARED
AM_PROG_LIBTOOL

AM_WITH_DMALLOC
AC_PROG_LN_S
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])
define([AC_LIBTOOL_LANG_GCJ_CONFIG], [:])
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])
AC_PROG_LIBTOOL

dnl Set PACKAGE_LOCALE_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${ac_default_prefix}/share/locale", [Locale-specific data directory])
else
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${prefix}/share/locale", [Locale-specific data directory])
fi

dnl Set PACKAGE_DATA_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}", [Shared Data Directory])
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}", [Shared Data Directory])
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

dnl Set PACKAGE_SOURCE_DIR in config.h.
packagesrcdir=`cd $srcdir && pwd`
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}", [Source code directory])

ALL_LINGUAS=""
AC_SUBST(ALL_LINGUAS)

m4_ifdef([AM_GNU_GETTEXT_VERSION], [
AM_GNU_GETTEXT_VERSION([0.17])
])

m4_ifdef([AM_GNU_GETTEXT], [
AM_GNU_GETTEXT([external], [need-ngettext])
po_makefile_in=po/Makefile.in
have_po="yes"
],[
have_po="no"
])
AC_SUBST(LTLIBINTL)
if test "x$LIBINTL" = "x"; then
  LIBINTL="$INTLLIBS"
fi

if test "x${POSUB}" = "x" ; then
   have_po="no"
fi

AM_CONDITIONAL([HAVE_PO], [test "x${have_po}" = "xyes"])


PKG_CHECK_MODULES([EVAS], evas,
   [],
   [
      echo "Could not find Evas.  If it is installed, \nplease make sure its path is in PKG_CONFIG_PATH";
      AC_MSG_ERROR([Empower needs Evas to compile.])
   ]
)

PKG_CHECK_MODULES([ECORE], [ ecore ecore-x ],
   [],
   [
      echo "Could not find Ecore.  If it is installed, \nplease make sure its path is in PKG_CONFIG_PATH";
      AC_MSG_ERROR([Empower needs Ecore to compile.])
   ]
)

PKG_CHECK_MODULES([EDJE], edje,
   [],
   [
      echo "Could not find Edje.  If it is installed, \nplease make sure its path is in PKG_CONFIG_PATH";
      AC_MSG_ERROR([Empower needs Edje to compile.])
   ]
)

PKG_CHECK_MODULES([ELEMENTARY], elementary,
   [],
   [
      echo "Could not find Elementary.  If it is installed, \nplease make sure its path is in PKG_CONFIG_PATH";
      AC_MSG_ERROR([Empower needs Elementary to compile.])
   ]
)

PKG_CHECK_MODULES([EDBUS], edbus,
   [],
   [
      echo "Could not find Edbus.  If it is installed, \nplease make sure its path is in PKG_CONFIG_PATH";
      AC_MSG_ERROR([Empower needs Edbus to compile.])
   ]
)

AC_OUTPUT([
Makefile
empower.spec
src/Makefile
src/bin/Makefile
data/Makefile
$po_makefile_in
])

echo
echo "Configuration:"
echo
echo "  Install path:              ${prefix}"
echo
