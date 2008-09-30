dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(configure.ac)

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(etk_server, 0.0.1)

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

#check for ETK
AC_PATH_EFL(
   etk,
   0.1.0.001,
   [
     AC_DEFINE(HAVE_ETK, 1, have ETK)
     etk="ETK "
     AM_CONDITIONAL(HAVE_ETK, true)
     have_etk="yes"
     etk_libs=`etk-config --libs`
     etk_cflags=`etk-config --cflags`
     etk_sources=`etk-config --build-dir`
     if test -d ${etk_sources}; then
       AC_DEFINE_UNQUOTED(ETK_SOURCE_DIR, "${etk_sources}", [Etk Source Code Directory])
     else
      AC_MSG_ERROR([Can't find Etk sources, can't build.]);
     fi
   ],
   [
     AC_MSG_ERROR([No Etk, can't build.])
   ]
)

#check for EVAS
AC_PATH_EFL(
   evas,
   0.9.9.025,
   [
     AC_DEFINE(HAVE_EVAS, 1, have EVAS)
     evas="EVAS "
     AM_CONDITIONAL(HAVE_EVAS, true)
     have_evas="yes"
     evas_libs=`evas-config --libs`
     evas_cflags=`evas-config --cflags`
   ],
   [
     AC_MSG_ERROR([No Evas, can't build.])
   ]
)

#check for ECORE
AC_PATH_EFL(
   ecore,
   0.9.9.026,
   [
     AC_DEFINE(HAVE_ECORE, 1, have ECORE)
     ecore="ECORE "
     AM_CONDITIONAL(HAVE_ECORE, true)
     have_ecore="yes"
     ecore_libs=`ecore-config --libs`
     ecore_cflags=`ecore-config --cflags`
   ],
   [
     AC_MSG_ERROR([No Ecore, can't build.])
   ]
)

dnl Set ETK_PACKAGE_DATA_DIR in config.h.  
etk_prefix="\"`etk-config --prefix`/share/etk\""
AC_DEFINE_UNQUOTED(ETK_PACKAGE_DATA_DIR, $etk_prefix, [Data dir for etk lib.])

# add required libs / include here
my_includes="$etk_cflags $evas_cflags $ecore_cflags"
my_libs="$etk_libs $evas_cflags $ecore_cflags"
AC_SUBST(my_includes)
AC_SUBST(my_libs)
AC_SUBST(etk_sources)

AC_OUTPUT([
Makefile
scripts/Makefile
scripts/etk_server_gen_signals.pl
src/Makefile
src/bin/Makefile
etk_server.spec
debian/changelog
],[
])
