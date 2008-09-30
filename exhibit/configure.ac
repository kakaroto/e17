dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(configure.ac)

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(exhibit, 0.1.1)

AC_PREREQ(2.52)

AM_CONFIG_HEADER(config.h)

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AM_ENABLE_SHARED
AM_PROG_LIBTOOL
LIBS=""

if test "x${exec_prefix}" = "xNONE"; then
  if test "x${prefix}" = "xNONE"; then
    bindir="${ac_default_prefix}/bin";
  else
    bindir="${prefix}/bin";
  fi
else
  if test "x${prefix}" = "xNONE"; then
    bindir="${ac_default_prefix}/bin";
  else
    bindir="${prefix}/bin";
  fi
fi

if test "x${exec_prefix}" = "xNONE"; then
  if test "x${prefix}" = "xNONE"; then
    libdir="${ac_default_prefix}/lib";
  else
    libdir="${prefix}/lib";
  fi
else
  if test "x${prefix}" = "xNONE"; then
    libdir="${ac_default_prefix}/lib";
  else
    libdir="${prefix}/lib";
  fi
fi
      
dnl Set PACKAGE_DATA_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}", [Shared Data Directory])
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}", [Shared Data Directory])
fi

dnl Set PACKAGE_BIN_DIR in config.h.
if test "x${bindir}" = 'xNONE'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${ac_default_prefix}/bin", [Installation Directory for User Executables])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${prefix}/bin", [Installation Directory for User Executables])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${bindir}", [Installation Directory for User Executables])
fi

dnl Set PACKAGE_LIB_DIR in config.h.
if test "x${libdir}" = 'xNONE'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${ac_default_prefix}/lib", [Installation Directory for Libraries])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${prefix}/lib", [Installation Directory for Libraries])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${libdir}", [Installation Directory for Libraries])
fi

dnl Set PACKAGE_SOURCE_DIR in config.h.
packagesrcdir=`cd $srcdir && pwd`
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}", [Source Code Directory])

PKG_CHECK_MODULES(EXHIBIT, [
  evas
  ecore
  ecore-evas
  ecore-file
  edje
  eet
  etk
  efreet
  efreet-mime
  epsilon
])

#check for E
AC_PATH_EFL(
   enlightenment,
   0.16.999.022,
   [
     AC_DEFINE(HAVE_E, 1, have E)
     e="E "
     AM_CONDITIONAL(HAVE_E, true)
     have_e="yes"
   ],
   [
     AC_DEFINE(HAVE_E, 0, no E)
     AM_CONDITIONAL(HAVE_E, false)
     AC_MSG_WARN([No E, can't build E desktop support.])
     have_e="no"
   ]
)

#check for Engrave
PKG_CHECK_MODULES(ENGRAVE, [engrave >= 0.1.0],
   [
     AC_DEFINE(HAVE_ENGRAVE, 1, have Engrave)
     engrave="Engrave "
     AM_CONDITIONAL(HAVE_ENGRAVE, true)
     have_engrave="yes"
     engrave_libs=$ENGRAVE_LIBS
     engrave_cflags=$ENGRAVE_CFLAGS
   ],
   [
     AC_DEFINE(HAVE_ENGRAVE, 0, no Engrave)
     AM_CONDITIONAL(HAVE_ENGRAVE, false)
     AC_MSG_WARN([No Engrave, can't build E desktop wallpaper support (non edje).])
     have_engrave="no"
     engrave_libs=""
     engrave_cflags=""
   ]
)

dnl Set ETK_PACKAGE_DATA_DIR in config.h.  
etk_prefix="\"$(pkg-config --variable=prefix etk)/share/etk\""
AC_DEFINE_UNQUOTED(ETK_PACKAGE_DATA_DIR, $etk_prefix, [Data dir for etk lib.])

# required libs / cflags
exhibit_libs=$EXHIBIT_LIBS
exhibit_cflags=$EXHIBIT_CFLAGS
AC_SUBST(exhibit_libs)
AC_SUBST(exhibit_cflags)

#optional libs / cflags
optional_cflags="$engrave_cflags"
optional_libs="$engrave_libs"
AC_SUBST(optional_cflags)
AC_SUBST(optional_libs)

AC_OUTPUT([
Makefile
src/Makefile
src/bin/Makefile
data/Makefile
debian/changelog
],[
touch exhibit_docs.tar.gz
])
