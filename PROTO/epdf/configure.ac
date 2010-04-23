
### Version

m4_define([epdf_version_major], [0])
m4_define([epdf_version_minor], [0])
m4_define([epdf_version_micro], [1])

m4_define([epdf_version],
   [epdf_version_major.epdf_version_minor.epdf_version_micro])

m4_define([lt_rev], m4_eval(epdf_version_major + epdf_version_minor))
m4_define([lt_cur], epdf_version_micro)
m4_define([lt_age], epdf_version_minor)

AC_INIT([epdf], [epdf_version], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.54])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

version_info="lt_rev:lt_cur:lt_age"
AC_SUBST(version_info)


### Default options with respect to host

requirements_epdf="poppler evas ecore"


### Checks for programs

AC_PROG_CC
AC_PROG_CPP
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_CXX_C_O
AC_LANG(C)

# doxygen program for documentation building

EFL_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])


### Checks for libraries

PKG_CHECK_MODULES(EPDF, [poppler >= 0.12 evas >= 0.9.9 eina-0])

POPPLER_VERSION=`pkg-config --modversion poppler`
AC_DEFINE_UNQUOTED([POPPLER_VERSION], ["${POPPLER_VERSION}"], [Poppler version.])

PKG_CHECK_MODULES(EPDFBIN, [ecore-evas >= 0.9.9 ecore >= 0.9.9])

# Ewl

have_ewl="no"
AC_ARG_ENABLE([ewl],
   [AC_HELP_STRING(
       [--enable-ewl],
       [enable ewl widget. [[default=enabled]]]
    )
   ],
   [have_ewl=${enableval}],
   [have_ewl="yes"]
)
AC_MSG_CHECKING(whether using Ewl)
AC_MSG_RESULT(${have_ewl})

if test "x${have_ewl}" = "xyes" ; then
   PKG_CHECK_MODULES([EWL],
      [ewl >= 0.5.3],
      [
       have_ewl="yes"
       EWL_TEST_LIBS_DIR="`$PKG_CONFIG --variable=testsdir ewl`"
       EWL_TEST_SOURCE_DIR="`$PKG_CONFIG --variable=testssrcdir ewl`"
       AC_SUBST(EWL_TEST_LIBS_DIR)
       AC_SUBST(EWL_TEST_SOURCE_DIR)
      ],
      [have_ewl="no"]
   )
else
   have_ewl="no"
fi
AM_CONDITIONAL([BUILD_EPDF_EWL], [test "x${have_ewl}" = "xyes"])


### Checks for header files

# Xpdf header files are needed

AC_LANG_PUSH(C++)
CPPFLAGS_save=${CPPFLAGS}
CPPFLAGS="${CPPFLAGS} ${EPDF_CFLAGS}"
AC_CHECK_HEADER([GlobalParams.h],
   [dummy="yes"],
   [AC_MSG_ERROR([Xpdf headers not found. Did you configure poppler with the option --enable-xpdf-headers ?])]
)
CPPFLAGS=${CPPFLAGS_save}
AC_LANG_POP(C++)


### Checks for types


### Checks for structures


### Checks for compiler characteristics
AC_C___ATTRIBUTE__


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
epdf-ewl.pc
doc/Makefile
src/Makefile
src/lib/Makefile
src/lib/poppler/Makefile
src/bin/Makefile
src/plugins/Makefile
src/plugins/ewl/Makefile
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
echo "  Documentation........: ${enable_doc}"
echo
echo "  Ewl widget...........: ${have_ewl}"
echo
echo "  Compilation..........: make"
echo
echo "  Installation.........: make install"
echo "    prefix.............: $prefix"
echo
