
### Version

m4_define([eyesight_version_major], [0])
m4_define([eyesight_version_minor], [0])
m4_define([eyesight_version_micro], [1])

m4_define([eyesight_version],
   [eyesight_version_major.eyesight_version_minor.eyesight_version_micro])

m4_define([lt_rev], m4_eval(eyesight_version_major + eyesight_version_minor))
m4_define([lt_cur], eyesight_version_micro)
m4_define([lt_age], eyesight_version_minor)

AC_INIT([eyesight], [eyesight_version], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_INIT([win32-dll])

version_info="lt_rev:lt_cur:lt_age"
AC_SUBST(version_info)

### Needed information

want_img="yes"
want_pdf="yes"
want_postscript="yes"
want_txt="yes"


### Additional options to configure

# CJK fonts for mupdf
AC_ARG_ENABLE([mupdf-cjk],
   [AC_HELP_STRING([--enable-mupdf-cjk], [enable mupdf CJK fonts @<:@default=disabled@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       want_mupdf_cjk="yes"
    else
       want_mupdf_cjk="no"
    fi
   ],
   [want_mupdfcjk="no"])

AC_MSG_CHECKING([whether mupdf CJK fonts are enabled])
AC_MSG_RESULT([${want_mupdf_cjk}])


### Checks for programs
AC_PROG_CC_C99

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   pkgconfig_requires_private="Requires.private"
else
   pkgconfig_requires_private="Requires"
fi
AC_SUBST([pkgconfig_requires_private])

EFL_ENABLE_BIN([eyesight])
EFL_WITH_BIN([edje], [edje-cc], [edje_cc])

# doxygen program for documentation building

EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])


### Checks for libraries

requirements_pc_eyesight="eina >= 1.2.0, evas >= 1.2.0 efreet-mime >= 1.2.0"

EYESIGHT_CHECK_MODULE([PDF], [${want_pdf}])
EYESIGHT_CHECK_MODULE([Postscript], [${want_postscript}])
EYESIGHT_CHECK_MODULE([Txt], [${want_txt}])
EYESIGHT_CHECK_MODULE([Img], [${want_img}])

PKG_CHECK_MODULES([EYESIGHT], [${requirements_pc_eyesight}])

if test "x${have_eyesight}" = "xyes" ; then
   PKG_CHECK_MODULES([EYESIGHT_BIN], [elementary >= 1.0.0 efreet >= 1.0.0 edbus >= 1.0.0 ecore-input >= 1.2.0],
      [build_eyesight_bin="yes"], [build_eyesight_bin="no"])
fi


### Checks for header files


### Checks for types


### Checks for structures


### Checks for compiler characteristics
AM_PROG_CC_C_O
EFL_ATTRIBUTE_UNUSED

EFL_EYESIGHT_BUILD=""
case "$host_os" in
   mingw*)
      EFL_EYESIGHT_BUILD="-DEFL_EYESIGHT_BUILD"
   ;;
esac
AC_SUBST([EFL_EYESIGHT_BUILD])

EFL_CHECK_COMPILER_FLAGS([eyesight], [-Wall -Wextra -Wshadow])


### Checks for linker characteristics

# use --enable-auto-import on Windows

lt_enable_auto_import=""
case "$host_os" in
   mingw*)
      lt_enable_auto_import="-Wl,--enable-auto-import"
      ;;
esac
AC_SUBST([lt_enable_auto_import])


### Checks for library functions


AC_SUBST([requirements_pc_eyesight])
AC_SUBST([requirements_libs_eyesight])

AC_CONFIG_FILES([
Makefile
eyesight.pc
src/Makefile
eyesight.spec
src/bin/Makefile
src/lib/Makefile
src/modules/Makefile
src/modules/img/Makefile
src/modules/pdf/Makefile
src/modules/pdf/mupdf-1.1/Makefile
src/modules/ps/Makefile
src/modules/txt/Makefile
data/Makefile
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
echo "  Modules..............:"
echo "    Image..............: ${enable_img}"
echo "    PDF................: ${enable_pdf}"
echo "    Postscript.........: ${enable_postscript}"
echo "    Txt................: ${enable_txt}"
echo
echo "  Build eyesight test..: ${build_eyesight_bin}"
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS"
echo "  LDFLAGS..............: $LDFLAGS"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo
