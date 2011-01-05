# remove stupid cache mechanism
rm -f config.cache

AC_INIT([elm_fullscreen], [0.0.1], [devilhorns@comcast.net])
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(config.h)
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

if test "x${bindir}" = 'xNONE'; then
  if test "x${prefix}" = "xNONE"; then
    PACKAGE_BIN_DIR="${ac_default_prefix}/bin"
  else
    PACKAGE_BIN_DIR="${prefix}/bin"
  fi
else
  PACKAGE_BIN_DIR="${bindir}"
fi
AC_SUBST(PACKAGE_BIN_DIR)

if test "x${prefix}" = "xNONE"; then
  PACKAGE_DATA_DIR="${ac_default_prefix}/share/${PACKAGE}"
  LOCALE_DIR="${ac_default_prefix}/share/locale"
  AC_DEFINE_UNQUOTED([PACKAGE_DATA_DIR], "${ac_default_prefix}/share/${PACKAGE}", [Define the PACKAGE_DATA_DIR])
else
  PACKAGE_DATA_DIR="${prefix}/share/${PACKAGE}"
  LOCALE_DIR="${prefix}/share/locale"
  AC_DEFINE_UNQUOTED([PACKAGE_DATA_DIR], "${prefix}/share/${PACKAGE}", [Define the PACKAGE_DATA_DIR])
fi
AC_SUBST(PACKAGE_DATA_DIR)
AC_SUBST(LOCALE_DIR)

## Check for Debugging Output ##
AC_MSG_CHECKING([whether to enable debugging messages])
AC_ARG_ENABLE([debug], 
  AC_HELP_STRING([--enable-debug], [enable debugging messages @<:@default=yes@:>@]),
  [e_cv_debug=$enableval],
  AC_CACHE_VAL([e_cv_debug], [e_cv_debug=yes])
)
if test "[${e_cv_debug}]" = yes; then 
  AC_DEFINE_UNQUOTED([WANT_DEBUG], [1], [Enable debugging messages])
fi
AC_MSG_RESULT([$e_cv_debug])

# Find edje_cc
PKG_CHECK_MODULES(EDJE, [edje >= 0.5.0])
AC_ARG_WITH(edje-cc,
  AC_HELP_STRING([--with-edje-cc=PATH], [specify a specific path to edje_cc]),
  [
    v=$withval;
    EDJE_CC=$v
  ],[
    EDJE_CC=$(pkg-config --variable=prefix edje)/bin/edje_cc
  ]
)
AC_SUBST(EDJE_CC)
AC_MSG_CHECKING([Which edje_cc to use])
AC_MSG_RESULT(${EDJE_CC})

PKG_CHECK_MODULES(ELM_FULLSCREEN, [
  elementary
])

AC_OUTPUT([
Makefile
src/Makefile
src/bin/Makefile
data/Makefile
data/desktop/Makefile
])
