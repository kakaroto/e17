m4_define([v_maj], [0])
m4_define([v_min], [1])
m4_define([v_mic], [0])
m4_define([v_ver], [v_maj.v_min.v_mic])

m4_define([lt_cur], m4_eval(v_maj + v_min))
m4_define([lt_rev], v_mic)
m4_define([lt_age], v_min)

AC_INIT([emap], [v_ver], [enlightenment-devel@lists.sourceforge.net])
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

AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AM_PROG_CC_C_O

AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

version_info="lt_cur:lt_rev:lt_age"
AC_SUBST([version_info])

EFL_ATTRIBUTE_UNUSED

### Needed information

AC_CANONICAL_BUILD
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

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   pkgconfig_requires_private="Requires.private"
else
   pkgconfig_requires_private="Requires"
fi
AC_SUBST(pkgconfig_requires_private)


### Checks for libraries

emap_requirements="eina >= 1.0.0 ecore >= 1.0.0 ecore-file >= 1.0.0"
AC_SUBST([emap_requirements])

PKG_CHECK_MODULES([EMAP], [${emap_requirements}])

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
emap.pc
src/Makefile
src/lib/Makefile
])

echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE_NAME $PACKAGE_VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS"
echo "  LDFLAGS..............: $LDFLAGS"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo
