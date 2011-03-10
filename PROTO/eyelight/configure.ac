
m4_define([v_maj], [0])
m4_define([v_min], [5])
m4_define([v_mic], [0])
m4_define([v_ver], [v_maj.v_min.v_mic.v_rev])

m4_define([lt_cur], m4_eval(v_maj + v_min))
m4_define([lt_rev], v_mic)
m4_define([lt_age], v_min)

AC_INIT([eyelight], [v_ver], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

version_info="lt_cur:lt_rev:lt_age"
AC_SUBST(version_info)

### Needed information

AC_CANONICAL_HOST


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
AC_SUBST(pkgconfig_requires_private)

# doxygen program for documentation building

EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])


### Checks for libraries

requirements="emotion edje ecore-file ecore-evas ecore evas eina"

requirements_bin="edje ecore-file ecore-evas ecore-x ecore evas eina"

PKG_CHECK_MODULES([EYELIGHT], [${requirements}])

PKG_CHECK_MODULES([EYELIGHT_BIN], [${requirements_bin}])

EFL_WITH_BIN([edje], [edje-cc], [edje_cc])

AC_SUBST(requirements)


### Checks for compiler characteristics
AM_PROG_CC_C_O
AC_C_CONST
AC_PROG_CC_STDC
AC_C___ATTRIBUTE__

EFL_COMPILER_FLAG([-Wall])
EFL_COMPILER_FLAG([-W])


### Checks for linker characteristics

# use --enable-auto-import on Windows

lt_enable_auto_import=""
case "$host_os" in
   mingw*)
      lt_enable_auto_import="-Wl,--enable-auto-import"
      EFL_EYELIGHT_BUILD="-DEFL_EYELIGHT_BUILD"
      ;;
esac
AC_SUBST(lt_enable_auto_import)
AC_SUBST(EFL_EYELIGHT_BUILD)


### Checks for library functions
AC_FUNC_ALLOCA


AC_CONFIG_FILES([
Makefile
eyelight.pc
doc/Makefile
src/Makefile
src/lib/Makefile
src/bin/Makefile
data/Makefile
data/viewer/Makefile
data/themes/Makefile
data/themes/default/Makefile
data/themes/default/fonts/Makefile
data/themes/default/transition/Makefile
])

AC_OUTPUT

echo
echo
echo "--------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "--------------------------------------------------------"
echo
echo
echo "Configuration Options Summary:"
echo
echo "  edje_cc..................: ${edje_cc}"
echo "  Documentation............: ${build_doc}"
echo
echo "Installation...............: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...................: $prefix"
echo
