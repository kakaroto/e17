dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(empower, 1.5.2)
AM_CONFIG_HEADER(src/bin/config.h)

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AM_ENABLE_SHARED
AM_PROG_LIBTOOL

AM_WITH_DMALLOC
AC_PROG_LN_S

AM_PROG_CC_C_O

dnl Add the languages which your application supports here.
ALL_LINGUAS=""

dnl Set PACKAGE_LOCALE_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${ac_default_prefix}/${DATADIRNAME}/locale", [Locale-specific data directory])
else
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${prefix}/${DATADIRNAME}/locale", [Locale-specific data directory])
fi

dnl Set PACKAGE_DATA_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}", [Shared Data Directory])
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}", [Shared Data Directory])
fi

dnl Set PACKAGE_BIN_DIR in config.h.
if test "x${datadir}" = 'x${prefix}/bin'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${ac_default_prefix}/bin", [Installation directory for user executables])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${prefix}/bin", [Installation directory for user executables])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${bindir}", [Installation directory for user executables])
fi

dnl Set PACKAGE_LIB_DIR in config.h.
if test "x${datadir}" = 'x${prefix}/lib'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${ac_default_prefix}/lib", [Installation directory for libraries])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${prefix}/lib", [Installation directory for libraries])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${libdir}", [Installation directory for libraries])
fi

dnl Set PACKAGE_SOURCE_DIR in config.h.
packagesrcdir=`cd $srcdir && pwd`
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}", [Source code directory])

PKG_CHECK_MODULES([EVAS], evas,
   [],
   [
      echo "Could not find Evas.  If it is installed, \nplease make sure its path is in PKG_CONFIG_PATH";
      AC_MSG_ERROR([Empower needs Evas to compile.])
   ]
)

PKG_CHECK_MODULES([ECORE], ecore,
   [],
   [
      echo "Could not find Ecore.  If it is installed, \nplease make sure its path is in PKG_CONFIG_PATH";
      AC_MSG_ERROR([Empower needs Ecore to compile.])
   ]
)

PKG_CHECK_MODULES([EDJE], edje,
   [],
   [
      echo "Could not find Edje.  If it is installed, \nplease make sure its path is in PKG_CONFIG_PATH";
      AC_MSG_ERROR([Empower needs Edje to compile.])
   ]
)

AC_ARG_ENABLE(ewl,
        AC_HELP_STRING([--enable-ewl/--disable-ewl],
        [enable/disable ewl version (default=autodetect)]),
        [enable_ewl=$enableval ], [ enable_ewl=default])

if test "x$enable_ewl" = "xdefault" || test "x$enable_ewl" = "xyes"; then
   PKG_CHECK_MODULES([EWL], ewl >= 0.4.0,
      [  enable_ewl="yes"  ],
      [
         enable_ewl="no"
         echo "Could not find Ewl.  If it is installed, \nplease make sure its path is in PKG_CONFIG_PATH";
         AC_MSG_ERROR([Empower needs Ewl to compile the Ewl version.])
      ]
)
fi

AM_CONDITIONAL(ENABLE_EWL, test x$enable_ewl = xyes)

if test "x$enable_etk" = "xyes" && test "x$enable_ewl" = "xyes"; then
  build_separate=yes
else
  build_separate=no
fi

AM_CONDITIONAL(BUILD_SEPARATE, test x$build_separate = xyes)

AC_OUTPUT([
Makefile 
empower.spec
src/Makefile
src/bin/Makefile
src/bin/ewl/Makefile
])

echo
echo "Configuration:"
echo
echo "  Install path:                           ${prefix}"
echo
echo "  Toolkit Support"
echo "  ----------------"
echo "  Build ETK:                              $enable_etk"
echo "  Build EWL:                              $enable_ewl"
echo
echo "  Building Seperate Binaries              $build_separate"
echo 
