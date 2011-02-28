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

AC_HEADER_TIME
AC_CHECK_HEADERS(sys/time.h)

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

# dlopen
dlopen_libs=""
case "$host_os" in
  mingw*)
# nothing on mingw platform
  ;;
  *)
    AC_CHECK_FUNCS(dlopen, res=yes, res=no)
    if test "x$res" = "xyes"; then
      AC_CHECK_FUNCS(dladdr, AC_DEFINE(HAVE_DLADDR))
    else
      AC_CHECK_LIB(dl, dlopen, res=yes, res=no)
      if test "x$res" = "xyes"; then
        AC_CHECK_LIB(dl, dladdr, AC_DEFINE(HAVE_DLADDR))
        dlopen_libs=-ldl
      else
        AC_MSG_ERROR(Cannot find dlopen)
      fi
    fi
esac
AC_SUBST(dlopen_libs)

PKG_CHECK_MODULES(LIBEMOTE,
  [eina]
  [ecore]
  [ecore-file]
)

PKG_CHECK_MODULES(IRC,
  [ecore-con]
  [evas]
)

PKG_CHECK_MODULES(AIM,
  [ecore-con]
)

PKG_CHECK_MODULES(EMOTE,
  [elementary]
  [eet]
  [efreet]
  [eina]
  [evas]
  [ecore-file]
)

AC_OUTPUT([
Makefile
emote.pc
src/Makefile
src/bin/Makefile
src/lib/Makefile
src/lib/protocols/Makefile
src/lib/protocols/irc/Makefile
src/lib/protocols/aim/Makefile
data/Makefile
data/desktop/Makefile
po/Makefile.in
])
