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

if test "x${POSUB}" = "x" ; then
   have_po="no"
fi

AM_CONDITIONAL([HAVE_PO], [test "x${have_po}" = "xyes"])


PKG_CHECK_MODULES([EMPOWER],
   [
     eina >= 1.7.0,
     evas >= 1.7.0,
     ecore >= 1.7.0,
     ecore-x >= 1.7.0,
     efreet >= 1.7.0,
     edbus >= 1.7.0,
     edje >= 1.7.0,
     elementary >= 1.7.0
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
