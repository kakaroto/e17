dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(configure.ac)

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(estickies, 0.1)

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

PKG_CHECK_MODULES(ESTICKIES, [
  elementary >= 0.3.0.0
  eet >= 1.0
])

AC_ARG_WITH(edje-cc,
[  --with-edje-cc=PATH              specify a specific path to edje_cc],
[
  v=$withval;
  edje_cc=$v
  echo "  Enlightenment edje_cc explicitly set to "$edje_cc;
],[
  edje_cc=$(pkg-config --variable=prefix edje)/bin/edje_cc
])
AC_SUBST(edje_cc)

AC_ARG_ENABLE([experimental],
   [AC_HELP_STRING([--enable-experimental], [enable experimental code @<:@default=no@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       _estickies_enable_experimental="yes"
    else
       _estickies_enable_experimental="no"
    fi
   ],
   [_estickies_enable_experimental="no"]
)

AC_MSG_CHECKING([whether experimental code is built])
AC_MSG_RESULT([${_estickies_enable_experimental}])

have_experimental="no"

if test "x${_estickies_enable_experimental}" = "xyes" ; then
    have_experimental="yes"
    AC_DEFINE(HAVE_EXPERIMENTAL, 1, [have experimental code support])
fi

AC_OUTPUT([
Makefile
src/Makefile
src/bin/Makefile
data/Makefile
data/other/Makefile
data/other/estickies.desktop
data/images/Makefile
data/themes/Makefile
data/themes/default/Makefile
data/themes/default/images/Makefile
])

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options Summary:"
echo
echo "Features:"
echo "  Build with experimental code support....: ${have_experimental}"
echo
echo "edje_cc.........: ${edje_cc}"
echo
echo "------------------------------------------------------------------------"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo
