
### Version

m4_define([v_maj], [0])
m4_define([v_min], [0])
m4_define([v_mic], [1])

m4_define([v_ver], [v_maj.v_min.v_mic])

m4_define([lt_rev], m4_eval(v_maj + v_min))
m4_define([lt_cur], v_mic)
m4_define([lt_age], v_min)

AC_INIT([epdf], [v_ver], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.54])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AC_GNU_SOURCE

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

version_info="lt_rev:lt_cur:lt_age"
AC_SUBST(version_info)


### Default options with respect to host

### Additional options to configure

# mupdf
AC_ARG_ENABLE([mupdf],
   [AC_HELP_STRING([--disable-mupdf], [disable mupdf backend @<:@default=enabled@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       want_mupdf="yes"
    else
       want_mupdf="no"
    fi
   ],
   [want_mupdf="yes"])

AC_MSG_CHECKING([whether mupdf is enabled])
AC_MSG_RESULT([${want_mupdf}])

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

# poppler
AC_ARG_ENABLE([poppler],
   [AC_HELP_STRING([--enable-poppler], [enable poppler backend @<:@default=disabled@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       want_poppler="yes"
    else
       want_poppler="no"
    fi
   ],
   [want_poppler="yes"])

AC_MSG_CHECKING([whether poppler is enabled])
AC_MSG_RESULT([${want_poppler}])

if test "x${want_mupdf}" = "xno" && test "x${want_poppler}" = "xno" ; then
   AC_MSG_ERROR([one of mupdf or poppler backends must be enabled])
fi


### Checks for programs

AC_PROG_CC
AC_PROG_CPP
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_CXX_C_O
AC_LANG(C)

# pkg-config

PKG_PROG_PKG_CONFIG

# doxygen program for documentation building

EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])


### Checks for libraries

requirements_epdf="ecore >= 1.0.0 evas >= 1.0.0"

case "$host_os" in
   mingw*)
      requirements_epdf="${requirements_epdf} evil"
      AC_DEFINE(HAVE_EVIL, 1, [Set to 1 if evil package is installed])
   ;;
esac

BUILD_MUPDF_CJK_FONTS=""
if test "x${want_mupdf}" = "xyes" ; then
   PKG_CHECK_MODULES(FREETYPE, [freetype2])
   PKG_CHECK_MODULES(EPDF, [${requirements_epdf}])
   backend="mupdf"
   if ! test "x${want_mupdf_cjk}" = "xyes" ; then
      BUILD_MUPDF_CJK_FONTS="-DNOCJK"
   fi
else
   requirements_epdf="poppler ${requirements_epdf}"
   PKG_CHECK_MODULES(EPDF, [${requirements_epdf}])
   PKG_CHECK_EXISTS([poppler >= 0.15.2], [have_poppler_0_15_2="yes"], [have_poppler_0_15_2="no"])
   POPPLER_VERSION=`${PKG_CONFIG} --modversion poppler`
   AC_DEFINE_UNQUOTED([POPPLER_VERSION], ["${POPPLER_VERSION}"], [Poppler version.])
   if test "x${have_poppler_0_15_2}" = "xyes" ; then
      AC_DEFINE([HAVE_POPPLER_0_15_2], [1], [Poppler >= 0.15.2.])
   fi
   backend="poppler"
fi

AC_SUBST(BUILD_MUPDF_CJK_FONTS)
AM_CONDITIONAL([BUILD_MUPDF_CJK_FONTS], [test "x${want_mupdf_cjk}" = "xyes"])
AM_CONDITIONAL([BUILD_EPDF_MUPDF], [test "x${want_mupdf}" = "xyes"])

PKG_CHECK_MODULES(EPDFBIN, [ecore-evas >= 1.0.0 ecore >= 1.0.0 evas >= 1.0.0 eina >= 1.0.0])


### Checks for header files

# Xpdf header files are needed

if ! test "x${want_mupdf}" = "xyes" ; then
   AC_LANG_PUSH(C++)
   CPPFLAGS_save=${CPPFLAGS}
   CPPFLAGS="${CPPFLAGS} ${EPDF_CFLAGS}"
   AC_CHECK_HEADER([GlobalParams.h],
      [dummy="yes"],
      [AC_MSG_ERROR([Xpdf headers not found. Did you configure poppler with the option --enable-xpdf-headers ?])])
   CPPFLAGS=${CPPFLAGS_save}
   AC_LANG_POP(C++)
fi


### Checks for types


### Checks for structures


### Checks for compiler characteristics
AC_C___ATTRIBUTE__

EFL_EPDF_BUILD=""
case "$host_os" in
   mingw*)
      EFL_EPDF_BUILD="-DEFL_EPDF_BUILD"
   ;;
esac
AC_SUBST(EFL_EPDF_BUILD)


### Checks for linker characteristics

# use --enable-auto-import on Windows

lt_enable_auto_import=""
case "$host_os" in
   mingw*)
      lt_enable_auto_import="-Wl,--enable-auto-import"
      ;;
esac
AC_SUBST(lt_enable_auto_import)


AC_SUBST(requirements_epdf)

AC_OUTPUT([
Makefile
epdf.pc
doc/Makefile
src/Makefile
src/lib/Makefile
src/lib/mupdf/Makefile
src/lib/poppler/Makefile
src/bin/Makefile
])

#####################################################################
## Info

echo
echo "------------------------------------------------------------------------"
echo "  $PACKAGE_NAME $PACKAGE_VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "  Backend..............: ${backend}"
echo "  Documentation........: ${enable_doc}"
echo
echo "  Compilation..........: make"
echo
echo "  Installation.........: make install"
echo "    prefix.............: $prefix"
echo
