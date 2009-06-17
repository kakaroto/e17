dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(configure.ac)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE(engage, 0.0.9)
AM_CONFIG_HEADER(config.h)

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AM_ENABLE_SHARED
AM_PROG_LIBTOOL

MODULE_ARCH="$host_os-$host_cpu"
AC_SUBST(MODULE_ARCH)
AC_DEFINE_UNQUOTED(MODULE_ARCH, "$MODULE_ARCH", "Module architecture")

AC_CHECK_FUNCS(setenv unsetenv)

if test "x${bindir}" = 'x${exec_prefix}/bin'; then
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
fi

if test "x${libdir}" = 'x${exec_prefix}/lib'; then
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
fi

dnl Set PACKAGE_DATA_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}", [Shared Data Directory])
else
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}", [Shared Data Directory])
fi

dnl Set PACKAGE_BIN_DIR in config.h.
if test "x${bindir}" = "xNONE"; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${ac_default_prefix}/bin", [PACKAGE_BIN_DIR])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${prefix}/bin", [PACKAGE_BIN_DIR])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${bindir}", [PACKAGE_BIN_DIR])
fi

dnl Set PACKAGE_LIB_DIR in config.h.
if test "x${libdir}" = "xNONE"; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${ac_default_prefix}/lib", [PACKAGE_LIB_DIR])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${prefix}/lib", [PACKAGE_LIB_DIR])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${libdir}", [PACKAGE_LIB_DIR])
fi

PKG_CHECK_MODULES(ENGAGE, [
  imlib2
  edje
  ecore
  ecore-config
  ecore-evas
  ecore-desktop
  evas
  etk
  embryo
  esmart_trans_x11
])

AC_ARG_ENABLE(xinerama,
  [  --enable-xinerama  Use Xinerama],
  [case "${enableval}" in
    yes) xinerama=true ;;
    no)  xinerama=false ;;
    *)   AC_MSG_ERROR(bad value ${enableval} for --enable-xinerama) ;;
  esac],[xinerama=false])
  AM_CONDITIONAL(XINERAMA, test x$xinerama = xtrue)

CFLAGS="-Wall $CFLAGS"

AC_OUTPUT([
Makefile
src/Makefile
data/Makefile
data/icons/Makefile
data/themes/Makefile
],[
])
