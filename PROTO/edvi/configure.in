AC_PREREQ(2.54)
AC_INIT(edvi, 0.1, enlightenment-devel@lists.sourceforge.net)
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE(1.6 dist-bzip2)

define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
AC_PROG_LIBTOOL

INTERFACE_CURRENT="0"
INTERFACE_REVISION="1"
INTERFACE_AGE="0"
version_info=${INTERFACE_CURRENT}:${INTERFACE_REVISION}:${INTERFACE_AGE}
AC_SUBST(version_info)

AC_PROG_CC
AM_PROG_CC_C_O

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

AC_PATH_GENERIC(DVIlib2, 2.9.0,
  [ AC_DEFINE(HAVE_DVILIB_2_9, 1, [have dvilib 2.9.0]) ],
  [ AC_PATH_GENERIC(DVIlib2, 2.8.0,
      [ ],
      [
       echo "ERROR:"
       echo "The DVIlib2-config development file was not found in your execute"
       echo "path. This may mean one of several things"
       echo "1. You may not have installed the DVIlib2-devel (or DVIlib2-dev)"
       echo "   packages."
       echo "2. You may have evas installed somewhere not covered by your path."
       echo ""
       echo "If this is the case make sure you have the packages installed, AND"
       echo "that the DVIlib2-config file is in your execute path (see your"
       echo "shell's manual page on setting the \$PATH environment variable), OR"
       echo "alternatively, specify the script to use with --with-dvilib2-config."
       AC_MSG_ERROR([Fatal Error: no DVIlib2-config library detected.])
      ]
    )
  ]
)

PKG_CHECK_MODULES(EDVI, evas >= ${evas_version} ecore >= ${ecore_version})
EDVI_CFLAGS="${EDVI_CFLAGS} ${DVILIB2_CFLAGS}"
EDVI_LIBS="${EDVI_LIBS} ${DVILIB2_LIBS}"

PKG_CHECK_MODULES(ECORE_EVAS,
   [ ecore-evas >= ${ecore_version} ]
)

requirements="evas ecore"

dnl Ewl
AC_ARG_ENABLE(ewl,
  AC_HELP_STRING(
    [--enable-ewl],
    [enable ewl widget. [[default=enabled]]]
  ), [
    have_ewl=$enableval
  ], [
    have_ewl="yes"
  ]
)
AC_MSG_CHECKING(whether using Ewl)
AC_MSG_RESULT($have_ewl)

if test "x$have_ewl" = "xyes"; then
   PKG_CHECK_MODULES(EWL,
      ewl >= 0.5,
     [
      have_ewl="yes"
      requirements="$requirements ewl"
      EWL_TEST_LIBS_DIR="`$PKG_CONFIG --variable=testsdir ewl`"
      EWL_TEST_SOURCE_DIR="`$PKG_CONFIG --variable=testssrcdir ewl`"
      AC_SUBST(EWL_TEST_LIBS_DIR)
      AC_SUBST(EWL_TEST_SOURCE_DIR)
     ],
     [
      have_ewl="no"
     ]
   )
else
   have_ewl="no"
fi
AM_CONDITIONAL(BUILD_EDVI_EWL, test "x${have_ewl}" = "xyes")

dnl Etk
AC_ARG_ENABLE(etk,
  AC_HELP_STRING(
    [--enable-etk],
    [enable etk widget. [[default=enabled]]]
  ), [
    have_etk=$enableval
  ], [
    have_etk="yes"
  ]
)
AC_MSG_CHECKING(whether using Etk)
AC_MSG_RESULT($have_etk)

if test "x$have_etk" = "xyes"; then
   PKG_CHECK_MODULES(ETK,
      etk >= 0.1,
     [
      have_etk="yes"
      requirements="$requirements etk"
     ],
     [
      have_etk="no"
     ]
   )
else
   have_etk="no"
fi
AM_CONDITIONAL(BUILD_EDVI_ETK, test "x${have_etk}" = "xyes")

PKG_CHECK_MODULES(EPSILON,
   epsilon >= 0.3,
  [
   have_epsilon="yes"
   requirements="$requirements epsilon"
   EPSILON_PLUGINS_DIR="`$PKG_CONFIG --variable=pluginsdir epsilon`"
   AC_SUBST(EPSILON_PLUGINS_DIR)
  ],
  [
   have_epsilon="no"
  ]
)
AM_CONDITIONAL(BUILD_EDVI_EPSILON, test "x${have_epsilon}" = "xyes")

AC_SUBST(requirements)

AC_OUTPUT([
Makefile
edvi.pc
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
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE_NAME $PACKAGE_VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "  Ewl widget...........: $have_ewl"
echo "  Etk widget...........: $have_etk"
echo "  Epsilon plugin.......: ${have_epsilon}"
echo
echo "  Compilation..........: make"
echo
echo "  Installation.........: make install"
echo
echo "    prefix.........: $prefix"
echo
