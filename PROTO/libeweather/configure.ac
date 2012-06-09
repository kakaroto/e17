
m4_define([v_maj], [0])
m4_define([v_min], [2])
m4_define([v_mic], [0])
m4_define([v_ver], [v_maj.v_min.v_mic])

m4_define([lt_cur], m4_eval(v_maj + v_min))
m4_define([lt_rev], v_mic)
m4_define([lt_age], v_min)

AC_INIT([eweather], [v_ver], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_HEADERS([config.h])
AH_TOP([
#ifndef EFL_CONFIG_H__
#define EFL_CONFIG_H__
])
AH_BOTTOM([
#endif /* EFL_CONFIG_H__ */
])

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

version_info="lt_cur:lt_rev:lt_age"
AC_SUBST([version_info])


### Needed information

AC_CANONICAL_HOST

case "$host_os" in
   mingw32ce*)
      MODULE_ARCH="$host_os-$host_cpu"
      ;;
   *)
      MODULE_ARCH="$host_os-$host_cpu-v_maj.v_min.v_mic"
      ;;
esac
AC_SUBST([MODULE_ARCH])
AC_DEFINE_UNQUOTED([MODULE_ARCH], ["${MODULE_ARCH}"], ["Module architecture"])

EFL_EWEATHER_BUILD=""
case "$host_os" in
   mingw*)
      EFL_EWEATHER_BUILD="-DEFL_EWEATHER_BUILD"
   ;;
esac
AC_SUBST([EFL_EWEATHER_BUILD])


### Default options with respect to host


### Additional options to configure


### Checks for programs

AM_PROG_CC_STDC

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   pkgconfig_requires_private="Requires.private"
else
   pkgconfig_requires_private="Requires"
fi
AC_SUBST(pkgconfig_requires_private)

# edje_cc
EFL_WITH_BIN([edje], [edje-cc], [edje_cc])


### Checks for libraries

eweather_requirements="eina >= 1.0.0 ecore >= 1.0.0 ecore-file >= 1.0.0 ecore-evas >= 1.0.0 evas >= 1.0.0 ecore-con >= 1.0.0 edje >= 1.0.0"
AC_SUBST([eweather_requirements])

PKG_CHECK_MODULES([EWEATHER], [${eweather_requirements}])

EFL_CHECK_DLOPEN


### Checks for header files


### Checks for types


### Checks for structures


### Checks for compiler characteristics

AM_PROG_CC_C_O


### Checks for linker characteristics

# use --enable-auto-import on Windows

lt_enable_auto_import=""
case "$host_os" in
   mingw*)
      lt_enable_auto_import="-Wl,--enable-auto-import"
      ;;
esac
AC_SUBST([lt_enable_auto_import])


AC_OUTPUT([
Makefile
eweather.pc
data/Makefile
data/default/Makefile
data/default/images/Makefile
data/simple/Makefile
data/simple/images/Makefile
src/Makefile
src/lib/Makefile
src/bin/Makefile
src/plugins/Makefile
src/plugins/test/Makefile
src/plugins/yahoo/Makefile
src/plugins/google/Makefile
])

echo
echo "$PACKAGE $VERSION"
echo
