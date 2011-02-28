dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT([empower], [2.0.0], [mekius@mekius.net])
AC_PREREQ([2.52])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AM_PROG_CC_STDC
AM_PROG_CC_C_O
AC_HEADER_STDC
AC_C_CONST
AM_ENABLE_SHARED
AM_PROG_LIBTOOL

AM_WITH_DMALLOC
AC_PROG_LN_S
define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])
define([AC_LIBTOOL_LANG_GCJ_CONFIG], [:])
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])
AC_PROG_LIBTOOL

dnl Set PACKAGE_LOCALE_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${ac_default_prefix}/share/locale", [Locale-specific data directory])
else
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${prefix}/share/locale", [Locale-specific data directory])
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

dnl Set PACKAGE_SOURCE_DIR in config.h.
packagesrcdir=`cd $srcdir && pwd`
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}", [Source code directory])

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.12.1])

PKG_CHECK_MODULES([EVAS], evas,
   [],
   [
      echo "Could not find Evas.  If it is installed, \nplease make sure its path is in PKG_CONFIG_PATH";
      AC_MSG_ERROR([Empower needs Evas to compile.])
   ]
)

PKG_CHECK_MODULES([ECORE], [ ecore ecore-x ],
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
        [enable/disable ewl version (default=no)]),
        [enable_ewl=$enableval ], [ enable_ewl=no])

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

if test "x$enable_ewl" = "xno"; then
AC_ARG_ENABLE(elementary,
        AC_HELP_STRING([--enable-elementary/--disable-elementary],
        [enable/disable elementary version (default=autodetect)]),
        [enable_elementary=$enableval ], [ enable_elementary=default])

if test "x$enable_elementary" = "xdefault" || test "x$enable_elementary" = "xyes"; then
   PKG_CHECK_MODULES([ELEMENTARY], elementary >= 0.3.0.0,
      [  enable_elementary="yes"  ],
      [
         enable_elementary="no"
         echo "Could not find Elementary.  If it is installed, \nplease makesure its path is in PKG_CONFIG_PATH";
         AC_MSG_ERROR([Empower needs Elementary to compile the Elementary version.])
      ]
)
fi
else
enable_elementary="no"
fi

AM_CONDITIONAL(ENABLE_ELEMENTARY, test x$enable_elementary = xyes)

AC_OUTPUT([
Makefile 
empower.spec
src/Makefile
src/bin/Makefile
src/bin/ewl/Makefile
src/bin/elm/Makefile
data/Makefile
po/Makefile.in
])

echo
echo "Configuration:"
echo
echo "  Install path:              ${prefix}"
echo
echo "  Toolkit Support"
echo "  ----------------"
echo "  EWL:                       $enable_ewl"
echo "  Elementary:                $enable_elementary"
echo 
