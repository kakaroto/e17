dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(ephoto, 5.15.0)
AM_CONFIG_HEADER(src/bin/config.h)

AC_ISC_POSIX
AC_PROG_CC
AC_HEADER_STDC
AM_ENABLE_SHARED
AM_PROG_LIBTOOL

AM_WITH_DMALLOC

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

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

dnl Set PACKAGE_EXEC_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_EXEC_DIR, "${ac_default_prefix}/bin", [Installed directory for executables])
else
  AC_DEFINE_UNQUOTED(PACKAGE_EXEC_DIR, "${prefix}/bin", [Installed directory for executables])
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

PKG_CHECK_MODULES([EET], eet,
   [
      have_eet="Yes"
   ],
   [
      have_eet="No"
      echo "Eet was not found by pkg-config!";
      AC_MSG_ERROR([Ephoto needs eet to compile.])
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

PKG_CHECK_MODULES([EINA], eina,
   [
      have_eina="Yes"
   ],
   [
      have_eina="No"
      echo "Eina was not found by pkg-config!";
      AC_MSG_ERROR([Ephoto needs eina to compile.])
   ]
)

PKG_CHECK_MODULES([ETHUMB], ethumb_client,
   [
      have_ethumb="Yes"
   ],
   [
      have_ethumb="No"
      echo "Ethumb was not found by pkg-config!";
      AC_MSG_ERROR([Ephoto needs ethumb to compile.])
   ]
)

PKG_CHECK_MODULES([ELEMENTARY], elementary,
   [
      have_elementary="Yes"
   ],
   [
      have_elementary="No"
      echo "Elementary was not found by pkg-config!";
      AC_MSG_ERROR([Ephoto needs elementary to compile.])
   ]
)

PKG_CHECK_MODULES([EIO], eio,
   [
      have_eio="Yes"
   ],
   [
      have_eio="No"
      echo "Eio was not found by pkg-config!";
      AC_MSG_ERROR([Ephoto needs eio to compile.])
   ]
)

AC_ARG_ENABLE([libexif],
   [AC_HELP_STRING([--enable-libexif], [enable libexif support @<:@default=yes@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       _ephoto_enable_libexif="yes"
    else
       _ephoto_enable_libexif="no"
    fi
   ],
   [_ephoto_enable_libexif="yes"]
)

AC_MSG_CHECKING([whether ethumb is built])
AC_MSG_RESULT([${_ephoto_enable_libexif}])

have_libexif="No"

if test "x${_ephoto_enable_libexif}" = "xyes" ; then
    PKG_CHECK_MODULES(EXIF, libexif, [
        have_libexif="Yes"
        AC_DEFINE(HAVE_LIBEXIF, 1, [have libexif support])
    ],[
       have_libexif="No"
    ])
fi

AM_CONDITIONAL(BUILD_LIBEXIF, test $have_libexif = Yes)

AC_OUTPUT([
Makefile
ephoto.spec
data/Makefile
data/images/Makefile
data/themes/Makefile
data/themes/default/Makefile
data/themes/default/images/Makefile
src/Makefile
src/bin/Makefile
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
echo "  Eet .............: $have_eet"
echo "  Evas ............: $have_evas"
echo "  Ecore_File.......: $have_ecoref"
echo "  Ecore_Evas.......: $have_ecoree"
echo "  Edje ............: $have_edje"
echo "  Efreet_Mime .....: $have_efreet_mime"
echo "  Eina.............: $have_eina"
echo "  Ethumb...........: $have_ethumb"
echo "  Elementary.......: $have_elementary"
echo "  Eio..............: $have_eio"
echo "  Libexif .........: $have_libexif"
echo
echo "Installation Path.........: $prefix"
echo
echo "Compilation...............: make"
echo "Installation..............: make install"
echo
