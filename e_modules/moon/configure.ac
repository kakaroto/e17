#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

rm -f config.cache

AC_PREREQ(2.59)
AC_INIT(moon, 0.2.2)
AC_CONFIG_SRCDIR([src/e_mod_main.c])
AC_CONFIG_HEADER([config.h])
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

AM_GNU_GETTEXT([external]) 
AM_GNU_GETTEXT_VERSION(0.14)

# Checks for libraries.
edje_cc="$(pkg-config --variable=prefix edje)/bin/edje_cc"
AC_ARG_WITH(edje-cc,
[  --with-edje-cc=PATH              specify a specific path to edje_cc],
[
  v=$withval;
  edje_cc=$v
  echo "  Enlightenment edje_cc explicitly set to "$edje_cc;
],[
  edje_cc="$(pkg-config --variable=prefix edje)/bin/edje_cc"
])
AC_SUBST(edje_cc)

AC_ARG_WITH(enlightenment-config,
[  --with-enlightenment-config=E_CONFIG    use enlightenment-config specified ],
[
  E_CONFIG=$withval;
  echo "using "$E_CONFIG" for enlightenment-config";
],[
  PROG="enlightenment-config";
  AC_PATH_PROG(E_CONFIG, $PROG, "", $PATH)
])
e_cflags=`$E_CONFIG --cflags`
e_libs=`$E_CONFIG --libs`
e_modules=`$E_CONFIG --module-dir`
AC_SUBST(e_cflags)
AC_SUBST(e_libs)
AC_SUBST(e_modules)

MODULE_ARCH="$host_os-$host_cpu"
AC_SUBST(MODULE_ARCH)

AC_PREFIX_DEFAULT(${HOME}/.e/e)
if test "x$prefix" = "xNONE"; then
  prefix=${ac_default_prefix}
  datarootdir=${prefix}
  moduledir=${prefix}/modules
else
  datarootdir=${prefix}/share
  moduledir=${libdir}/enlightenment/modules
fi
AC_SUBST(MODULEDIR, "${moduledir}")

localedir=${datarootdir}/locale
AC_SUBST(LOCALEDIR, "${localedir}")
AC_DEFINE_UNQUOTED(LOCALEDIR, "${localedir}", [Module Locale Directory])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_STRUCT_TM

# Checks for library functions.
AC_CHECK_FUNCS([sqrt strdup])

AC_CONFIG_FILES([Makefile
                 e_modules-moon.spec
                 data/Makefile
                 data/themes/Makefile
                 data/themes/images/Makefile
                 po/Makefile.in
                 src/Makefile])
AC_OUTPUT
