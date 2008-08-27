# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(eps, 0.1, enlightenment-devel@lists.sourceforge.net)
AC_PREREQ(2.52)
AC_CONFIG_HEADERS(config.h)
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE(1.6 dist-bzip2)

AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
AC_PROG_LIBTOOL

INTERFACE_CURRENT="0"
INTERFACE_REVISION="1"
INTERFACE_AGE="0"
version_info=${INTERFACE_CURRENT}:${INTERFACE_REVISION}:${INTERFACE_AGE}
AC_SUBST(version_info)

AC_PROG_CC
AC_PROG_CPP
AM_PROG_CC_C_O
AC_C_CONST

dnl Checking for __attribute__ support
AC_MSG_CHECKING([for __attribute__])
AC_CACHE_VAL(_cv_have___attribute__,
  [
    AC_TRY_COMPILE([#include <stdlib.h>],
      [int func(int x); int foo(int x __attribute__ ((unused))) { exit(1); }],
      [_cv_have___attribute__="yes"],
      [_cv_have___attribute__="no"])
  ]
)

if test "x${_cv_have___attribute__}" = "xyes" ; then
   AC_DEFINE(HAVE___ATTRIBUTE__, 1, [Define to 1 if your compiler has __attribute__])
fi
AC_MSG_RESULT(${_cv_have___attribute__})

evas_version=0.9.9
ecore_version=0.9.9

PKG_CHECK_MODULES(EPS,
   [ evas >= ${evas_version} libspectre >= 0.2.0 ]
)

PKG_CHECK_MODULES(EPS_BIN,
   [ ecore >= ${ecore_version}  ecore-evas >= ${ecore_version} ]
)

requirements="libspectre evas"

dnl Ewl
AC_ARG_ENABLE(ewl,
  AC_HELP_STRING(
    [--enable-ewl],
    [enable ewl widget. [[default=enabled]]]
  ),
  [ have_ewl=${enableval} ],
  [ have_ewl="yes" ]
)
AC_MSG_CHECKING(whether using Ewl)
AC_MSG_RESULT(${have_ewl})

if test "x${have_ewl}" = "xyes"; then
   PKG_CHECK_MODULES(EWL,
      ewl >= 0.5,
     [
      have_ewl="yes"
      requirements="${requirements} ewl"
      EWL_TEST_LIBS_DIR="`$PKG_CONFIG --variable=testsdir ewl`"
      EWL_TEST_SOURCE_DIR="`$PKG_CONFIG --variable=testssrcdir ewl`"
      AC_SUBST(EWL_TEST_LIBS_DIR)
      AC_SUBST(EWL_TEST_SOURCE_DIR)
     ],
     [ have_ewl="no" ]
   )
else
   have_ewl="no"
fi
AM_CONDITIONAL(BUILD_EPS_EWL, test "x${have_ewl}" = "xyes")

dnl Etk
AC_ARG_ENABLE(etk,
  AC_HELP_STRING(
    [--enable-etk],
    [enable etk widget. [[default=enabled]]]
  ),
  [ have_etk=${enableval} ],
  [ have_etk="yes" ]
)
AC_MSG_CHECKING(whether using Etk)
AC_MSG_RESULT(${have_etk})

if test "x${have_etk}" = "xyes"; then
   PKG_CHECK_MODULES(ETK,
      etk >= 0.1,
     [
      have_etk="yes"
      requirements="${requirements} etk"
     ],
     [ have_etk="no" ]
   )
else
   have_etk="no"
fi
AM_CONDITIONAL(BUILD_EPS_ETK, test "x${have_etk}" = "xyes")

dnl Epsilon
AC_ARG_ENABLE(epsilon,
  AC_HELP_STRING(
    [--enable-epsilon],
    [enable epsilon plugin. [[default=enabled]]]
  ),
  [ have_epsilon=${enableval} ],
  [ have_epsilon="yes" ]
)
AC_MSG_CHECKING(whether using Epsilon)
AC_MSG_RESULT(${have_epsilon})

if test "x${have_etk}" = "xyes"; then
   PKG_CHECK_MODULES(EPSILON,
      imlib2 >= 1.4 epsilon >= 0.3,
     [
      have_epsilon="yes"
      requirements="${requirements} epsilon imlib2"
      EPSILON_PLUGINS_DIR="`$PKG_CONFIG --variable=pluginsdir epsilon`"
      AC_SUBST(EPSILON_PLUGINS_DIR)
     ],
     [ have_epsilon="no" ]
   )
else
   have_epsilon="no"
fi
AM_CONDITIONAL(BUILD_EPS_EPSILON, test "x${have_epsilon}" = "xyes")

AC_SUBST(requirements)


AC_OUTPUT([
Makefile
eps.pc
src/Makefile
src/lib/Makefile
src/bin/Makefile
src/plugins/Makefile
src/plugins/epsilon/Makefile
src/plugins/ewl/Makefile
])

#####################################################################
## Info

echo
echo "------------------------------------------------------------------------"
echo "  ${PACKAGE_NAME} ${PACKAGE_VERSION}"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "  Ewl widget...........: ${have_ewl}"
echo "  Etk widget...........: ${have_etk}"
echo "  Epsilon plugin.......: ${have_epsilon}"
echo
echo "  Compilation..........: make"
echo
echo "  Installation.........: make install"
echo "    prefix.............: ${prefix}"
echo
