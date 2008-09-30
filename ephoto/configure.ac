dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(ephoto, 3.15.0)
AM_CONFIG_HEADER(src/bin/config.h)

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AM_ENABLE_SHARED
AM_PROG_LIBTOOL

AM_WITH_DMALLOC

dnl Add the languages which your application supports here.
ALL_LINGUAS="de eo fr hu it sv ru"
AC_SUBST(ALL_LINGUAS)

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.12.1])

dnl Set PACKAGE_LOCALE_DIR in config.h.
if test "x$LIBINTL" = "x"; then
  LIBINTL="$INTLLIBS"
fi
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

PKG_CHECK_MODULES([EXIF], libexif >= 0.6.13,
   [
      have_exif="Yes"
      AC_DEFINE(BUILD_EXIF_SUPPORT, 1, [Enable Exif Support])
   ],
   [
      have_exif="No"
   ]
)

PKG_CHECK_MODULES([EVAS], evas,
   [
      have_evas="Yes"
   ],
   [
      have_evas="No"
      echo "Evas was not found by pkg-config!";
      AC_MSG_ERROR([Ephoto needs evas to compile.])
   ]
)

PKG_CHECK_MODULES([ECORE_FILE], ecore-file,
   [
      have_ecoref="Yes"
   ],
   [
      have_ecoref="No"
      echo "Ecore_File was not found by pkg-config!";
      AC_MSG_ERROR([Ephoto needs ecore_file to compile.])
   ]
)

PKG_CHECK_MODULES([ECORE_EVAS], ecore-evas,
   [
      have_ecoree="Yes"
   ],
   [
      have_ecoree="No"
      echo "Ecore_Evas was not found by pkg-config!";
      AC_MSG_ERROR([Ephoto needs ecore_evas to compile.])
   ]
)

PKG_CHECK_MODULES([EDJE], edje,
   [
      have_edje="Yes"
   ],
   [
      have_edje="No"
      echo "Edje was not found by pkg-config!";
      AC_MSG_ERROR([Ephoto needs edje to compile.])
   ]
)

PKG_CHECK_MODULES([EWL], ewl,
   [
      have_ewl="Yes"
   ],
   [
      have_ewl="No"
      echo "Ewl was not found by pkg-config!";
      AC_MSG_ERROR([Ephoto needs ewl to compile.])
   ]
)

PKG_CHECK_MODULES([EFREET_MIME], efreet-mime,
   [
      have_efreet_mime="Yes"
   ],
   [
      have_efreet_mime="No"
      echo "Efreet_Mime was not found by pkg-config!";
      AC_MSG_ERROR([Ephoto needs efreet_mime to compile.])
   ]
)

PKG_CHECK_MODULES([EPSILON], epsilon,
   [
      have_epsilon="Yes"
   ],
   [
      have_epsilon="No"
      echo "Epsilon was not found by pkg-config!";
      AC_MSG_ERROR([Ephoto needs epsilon to compile.])
   ]
)

AC_OUTPUT([
Makefile 
ephoto.spec
data/Makefile
data/images/Makefile
po/Makefile
src/Makefile
src/bin/Makefile
debian/changelog
])

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo "    ________   ________  ___  ___  ________  ________  ________"
echo "   /  _____/  /  __   / /  / /  / /  __   / /__  ___/ /  __   /"
echo "  /  /____   /  /_/  / /  /_/  / /  / /  /    / /    /  / /  / "
echo " /  _____/  /  _____/ /  __   / /  / /  /    / /    /  / /  /  "
echo "/  /_____  /  /      /  / /  / /  /_/  /    / /    /  /_/  /   "
echo "________/ /__/      /__/ /__/ /_______/    /_/    /_______/    "
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "  Evas ............: $have_evas"
echo "  Ecore_File.......: $have_ecoref"
echo "  Ecore_Evas.......: $have_ecoree"
echo "  Edje ............: $have_edje"
echo "  Efreet_Mime .....: $have_efreet_mime"
echo "  Epsilon .........: $have_epsilon"
echo "  Ewl .............: $have_ewl"
echo "  Libexif .........: $have_exif"
echo
echo "Installation Path.........: $prefix"
echo
echo "Compilation...............: make"
echo "Installation..............: make install"
echo
