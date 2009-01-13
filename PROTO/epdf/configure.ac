
AC_INIT([epdf], [0.1], [enlightenment-devel@lists.sourceforge.net])
AC_PREREQ([2.54])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE(1.6 dist-bzip2)

AC_LIBTOOL_WIN32_DLL
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

INTERFACE_CURRENT="0"
INTERFACE_REVISION="0"
INTERFACE_AGE="0"
version_info=${INTERFACE_CURRENT}:${INTERFACE_REVISION}:${INTERFACE_AGE}
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

PKG_CHECK_MODULES(EPDF, [evas >= 0.9.9 ecore >= 0.9.9 poppler >= 0.6])

POPPLER_VERSION=`pkg-config --modversion poppler`
AC_DEFINE_UNQUOTED([POPPLER_VERSION], ["${POPPLER_VERSION}"], [Poppler version.])

PKG_CHECK_MODULES(ECORE_EVAS, [ecore-evas >= 0.9.9])

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

# Etk

have_etk="no"
AC_ARG_ENABLE([etk],
   [AC_HELP_STRING(
       [--enable-etk],
       [enable etk widget. [[default=enabled]]]
    )
   ],
   [have_etk=${enableval}]
   [have_etk="yes"]
)
AC_MSG_CHECKING(whether using Etk)
AC_MSG_RESULT(${have_etk})

if test "x${have_etk}" = "xyes" ; then
   PKG_CHECK_MODULES([ETK],
      [etk >= 0.1],
      [have_etk="yes"],
      [have_etk="no"]
   )
else
   have_etk="no"
fi
AM_CONDITIONAL([BUILD_EPDF_ETK], [test "x${have_etk}" = "xyes"])

# Epsilon

PKG_CHECK_MODULES([EPSILON],
   [epsilon >= 0.3],
   [
    have_epsilon="yes"
    EPSILON_PLUGINS_DIR="`$PKG_CONFIG --variable=pluginsdir epsilon`"
    AC_SUBST(EPSILON_PLUGINS_DIR)
   ],
   [have_epsilon="no"]
)
AM_CONDITIONAL([BUILD_EPDF_EPSILON], [test "x${have_epsilon}" = "xyes"])


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

AC_SUBST(requirements_epdf)

AC_OUTPUT([
Makefile
epdf.pc
epdf-ewl.pc
epdf-etk.pc
doc/Makefile
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
echo "  $PACKAGE_NAME $PACKAGE_VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "  Documentation........: ${enable_doc}"
echo
echo "  Ewl widget...........: ${have_ewl}"
echo "  Etk widget...........: ${have_etk}"
echo "  Epsilon plugin.......: ${have_epsilon}"
echo
echo "  Compilation..........: make"
echo
echo "  Installation.........: make install"
echo "    prefix.............: $prefix"
echo
