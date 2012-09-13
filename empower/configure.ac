##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_maj], [2])
m4_define([v_min], [0])
m4_define([v_mic], [999])
m4_define([v_rev], m4_esyscmd([(svnversion "${SVN_REPO_PATH:-.}" | grep -v '\(export\|Unversioned directory\)' || echo 0) | awk -F : '{printf("%s\n", $1);}' | tr -d ' :MSP\n']))
m4_if(v_rev, [0], [m4_define([v_rev], m4_esyscmd([git log 2> /dev/null | (grep -m1 git-svn-id || echo 0) | sed -e 's/.*@\([0-9]*\).*/\1/' | tr -d '\n']))])
##--   When released, remove the dnl on the below line
dnl m4_undefine([v_rev])
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_ifdef([v_rev], [m4_define([v_ver], [v_maj.v_min.v_mic.v_rev])], [m4_define([v_ver], [v_maj.v_min.v_mic])])
m4_define([lt_rev], m4_eval(v_maj + v_min))
m4_define([lt_cur], v_mic)
m4_define([lt_age], v_min)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT([empower], [v_ver], [mekius@mekius.net])
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

# Check for systemd-login library
have_libsystemd_login=no
PKG_CHECK_MODULES(LIBSYSTEMD_LOGIN,
                  [libsystemd-login],
                  have_libsystemd_login=yes,
                  have_libsystemd_login=no)
if test "$have_libsystemd_login" = "yes"; then
  AC_DEFINE([HAVE_LIBSYSTEMD_LOGIN], 1, [Define to 1 if libsystemd-login is available])
fi
AM_CONDITIONAL(HAVE_LIBSYSTEMD_LOGIN, [test "$have_libsystemd_login" = "yes"], [Using libsystemd-login])

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
