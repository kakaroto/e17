=-------##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_define([v_maj], [0])
m4_define([v_min], [1])
m4_define([v_mic], [0])
m4_define([v_rev], m4_esyscmd([(svnversion "${SVN_REPO_PATH:-.}" | grep -v export || echo 0) | awk -F : '{printf("%s\n", $1);}' | tr -d ' :MSP\n']))
m4_if(v_rev, [0], [m4_define([v_rev], m4_esyscmd([git log 2> /dev/null | (grep -m1 git-svn-id || echo 0) | sed -e 's/.*@\([0-9]*\).*/\1/' | tr -d '\n']))])
##--   When released, remove the dnl on the below line
dnl m4_undefine([v_rev])
##--   When doing snapshots - change soname. remove dnl on below line
m4_define([relname], [ver-pre-svn-09])
m4_define([v_rel], [-release relname])
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_ifdef([v_rev], [m4_define([v_ver], [v_maj.v_min.v_mic.v_rev])],
[m4_define([v_ver], [v_maj.v_min.v_mic])])
m4_define([lt_rev], m4_eval(v_maj + v_min))
m4_define([lt_cur], v_mic)
m4_define([lt_age], v_min)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##

AC_INIT([elev8], [v_ver], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_GNU_SOURCE

AC_C_BIGENDIAN
AC_ISC_POSIX
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AC_C___ATTRIBUTE__
AC_FUNC_ALLOCA

EFL_COMPILER_FLAG([-Wall])

AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
m4_ifdef([v_rev], , [m4_define([v_rev], [0])])
m4_ifdef([v_rel], , [m4_define([v_rel], [])])
AC_DEFINE_UNQUOTED(VMAJ, [v_maj], [Major version])
AC_DEFINE_UNQUOTED(VMIN, [v_min], [Minor version])
AC_DEFINE_UNQUOTED(VMIC, [v_mic], [Micro version])
AC_DEFINE_UNQUOTED(VREV, [v_rev], [Revison])
version_info="lt_rev:lt_cur:lt_age"
release_info="v_rel"
AC_SUBST(version_info)
AC_SUBST(release_info)
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##--##
VMAJ=v_maj
VMIN=v_min
AC_SUBST(VMAJ)
AC_SUBST(VMIN)

case "$host_os" in
  mingw32ce* | cegcc*)
    MODULE_ARCH="$host_os-$host_cpu"
    ;;
  *)
    MODULE_ARCH="$host_os-$host_cpu-v_maj.v_min.v_mic"
    ;;
esac
AC_SUBST(MODULE_ARCH)
AC_DEFINE_UNQUOTED(MODULE_ARCH, "$MODULE_ARCH", "Module architecture")

PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES([ELEV8],
   [
    eina >= 1.0.999
    eet >= 1.4.0
    evas >= 1.0.999
    ecore >= 1.0.999
    ecore-evas >= 1.0.999
    ecore-file >= 1.0.999
    ecore-imf >= 1.0.999
    ecore-con >= 1.0.999
    edje >= 1.0.999
    edbus >= 1.0.999
    efreet >= 1.0.999
    efreet-mime >= 1.0.999
    efreet-trash >= 1.0.999
    elementary >= 0.7.0
   ]
)

dnl Check for libv8 and v8.h by linking a bit of code using v8
AC_LANG_PUSH(C++)
AC_SUBST(V8_LIBS)
AC_MSG_CHECKING([for libv8])
saved_LDFLAGS="$LDFLAGS"
LDFLAGS="$LDFLAGS -lv8"
AC_LINK_IFELSE(
[AC_LANG_PROGRAM(
[[#include <v8.h>]],
[[v8::Persistent<v8::Number> n;]],
[[n = v8::Persistent<v8::Number>::New(v8::Number::New(0));]],
[[return 0;]],
)],[V8_LIBS="-lv8"], AC_MSG_ERROR([elev8 requires libv8]))
LDFLAGS="$saved_LDFLAGS"
AC_MSG_RESULT([yes])
AC_LANG_POP

EFL_WITH_BIN([edje], [edje-cc], [edje_cc])

AC_OUTPUT([
Makefile
src/Makefile
src/bin/Makefile
data/Makefile
data/images/Makefile
data/desktop/Makefile
data/javascript/Makefile
])

#####################################################################
## Info

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "  eet......................: ${eet_eet}"
echo "  edje_cc..................: ${edje_cc}"
echo
echo "Compilation................: make (or gmake)"
echo "  CPPFLAGS.................: $CPPFLAGS"
echo "  CFLAGS...................: $CFLAGS"
echo "  LDFLAGS..................: $LDFLAGS"
echo
echo "Installation...............: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...................: $prefix"
echo
