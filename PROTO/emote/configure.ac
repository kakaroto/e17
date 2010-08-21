rm -f config.cache

AC_INIT([emote], [0.0.1], [devilhorns@comcast.net])
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
AM_CONFIG_HEADER([config.h])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_GNU_SOURCE
AC_C_BIGENDIAN
AC_PROG_CC_C99
AM_PROG_CC_C_O
AC_C_CONST
AC_FUNC_ALLOCA
AC_C___ATTRIBUTE__

define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])
define([AC_LIBTOOL_LANG_GCJ_CONFIG], [:])
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

EFL_COMPILER_FLAG([-g])
EFL_COMPILER_FLAG([-Wall])
EFL_COMPILER_FLAG([-Wextra])

PKG_PROG_PKG_CONFIG

EFL_CHECK_PATH_MAX

if test "x${prefix}" = "xNONE"; then
   PACKAGE_PREFIX="${ac_default_prefix}"
else
   PACKAGE_PREFIX="${prefix}"
fi
AC_DEFINE_UNQUOTED(PACKAGE_PREFIX, "$PACKAGE_PREFIX", Installation Prefix)

if test "x${prefix}" = "xNONE"; then
  PACKAGE_DATA_DIR="${ac_default_prefix}/share/${PACKAGE}"
else
  PACKAGE_DATA_DIR="${prefix}/share/${PACKAGE}"
fi
AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "$PACKAGE_DATA_DIR", Data Installation Prefix)

if test "x${prefix}" = "xNONE"; then
  PACKAGE_LOCALE_DIR="${ac_default_prefix}/share/locale"
else
  PACKAGE_LOCALE_DIR="${prefix}/share/locale"
fi
AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "$PACKAGE_LOCALE_DIR", Locale Installation Prefix)

AC_DEFINE_UNQUOTED(PACKAGE_BUGREPORT, "<devilhorns@comcast.net>", Send bug reports here)
AC_DEFINE_UNQUOTED(PACKAGE_TARNAME, "$PACKAGE")
AC_DEFINE_UNQUOTED(PACKAGE_VERSION, "$VERSION")

m4_ifdef([AM_GNU_GETTEXT_VERSION], [
AM_GNU_GETTEXT_VERSION([0.17])
])

m4_ifdef([AM_GNU_GETTEXT], [
AM_GNU_GETTEXT([external])
AM_CONDITIONAL([HAVE_PO], [true])
],[
AM_CONDITIONAL([HAVE_PO], [false])
])
AC_SUBST(LTLIBINTL)
if test "x$LIBINTL" = "x"; then
  LIBINTL="$INTLLIBS"
fi

PKG_CHECK_MODULES(EMOTE, [
  elementary
])

emote_libs=$EMOTE_LIBS" "$LIBINTL" "
emote_cflags="-DUSE_EMOTE_CONFIG_H "$EMOTE_CFLAGS" "
AC_SUBST(emote_libs)
AC_SUBST(emote_cflags)

AC_OUTPUT([
Makefile
src/Makefile
src/bin/Makefile
data/Makefile
data/desktop/Makefile
po/Makefile.in
])
