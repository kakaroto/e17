
### Version

m4_define([v_maj], [0])
m4_define([v_min], [0])
m4_define([v_mic], [1])

m4_define([vigrid_version], [v_maj.v_min.v_mic])

m4_define([lt_cur], m4_eval(v_maj + v_min))
m4_define([lt_rev], v_mic)
m4_define([lt_age], v_min)

AC_INIT([vigrid], [vigrid_version], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.60])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.10 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_PREREQ([2.2])
LT_INIT([win32-dll])

VMAJ=v_maj
AC_SUBST([VMAJ])

version_info="lt_cur:lt_rev:lt_age"
AC_SUBST([version_info])


### Needed information

AC_CANONICAL_HOST

EFL_VIGRID_BUILD=""
case "$host_os" in
   mingw*)
      EFL_VIGRID_BUILD="-DEFL_VIGRID_BUILD"
   ;;
esac
AC_SUBST([EFL_VIGRID_BUILD])


### Checks for programs

AC_PROG_CC

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   pkgconfig_requires_private="Requires.private"
else
   pkgconfig_requires_private="Requires"
fi
AC_SUBST([pkgconfig_requires_private])

# doxygen program for documentation building

EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])


### Checks for libraries

requirements_pc_vigrid="eina"
AC_SUBST([requirements_pc_vigrid])

PKG_CHECK_MODULES([VIGRID], [${requirements_pc_vigrid}])


### Checks for header files


### Checks for types


### Checks for structures


### Checks for compiler characteristics

EFL_CHECK_COMPILER_FLAGS([VIGRID], [-Wall -Wextra -Wshadow])


### Checks for linker characteristics

lt_enable_auto_import=""
case "${host_os}" in
   mingw*)
      lt_enable_auto_import="-Wl,--enable-auto-import"
   ;;
esac
AC_SUBST([lt_enable_auto_import])


### Checks for library functions


AC_CONFIG_FILES([
Makefile
vigrid.pc
src/Makefile
src/bin/Makefile
src/include/Makefile
src/lib/Makefile
src/tests/Makefile
])

AC_OUTPUT


#####################################################################
## Info

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE_NAME $PACKAGE_VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "  OS...................: ${host_os}"
echo
echo "  Documentation........: ${build_doc}"
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS"
echo "  LDFLAGS..............: $LDFLAGS"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo