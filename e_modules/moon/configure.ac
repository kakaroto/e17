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
if test "$GCC" = "yes"; then
  CFLAGS="$CFLAGS -Wall"
fi
AC_PROG_LIBTOOL

ALL_LINGUAS="eo ja it sv"
AC_SUBST(ALL_LINGUAS)

AM_GNU_GETTEXT([external]) 
AM_GNU_GETTEXT_VERSION(0.14)

# Checks for libraries.
AC_PATH_GENERIC(eet, 0.9.10, [
	AC_SUBST(eet_libs)
	AC_SUBST(eet_cflags) ],
	AC_MSG_ERROR(Cannot find eet: Is eet-config in path?))
eet_libs=`eet-config --libs`
eet_cflags=`eet-config --cflags`
AC_SUBST(eet_flags)
AC_SUBST(eet_cflags)

AC_PATH_GENERIC(evas, 0.9.9, [
	AC_SUBST(evas_libs)
	AC_SUBST(evas_cflags) ],
	AC_MSG_ERROR(Cannot find evas: Is evas-config in path?))
evas_libs=`evas-config --libs`
evas_cflags=`evas-config --cflags`
AC_SUBST(evas_libs)
AC_SUBST(evas_cflags)

AC_PATH_GENERIC(ecore, 0.9.9, [
	AC_SUBST(ecore_libs)
	AC_SUBST(ecore_cflags) ],
	AC_MSG_ERROR(Cannot find ecore: Is ecore-config in path?))
ecore_libs=`ecore-config --libs`
ecore_cflags=`ecore-config --cflags`
AC_SUBST(ecore_libs)
AC_SUBST(ecore_cflags)

AC_PATH_GENERIC(embryo, 0.5.0, [
        AC_SUBST(embryo_libs)
	AC_SUBST(embryo_cflags) ],
	AC_MSG_ERROR(Cannot find embryo: Is embryo-config in path?))
embryo_libs=`embryo-config --libs`
embryo_cflags=`embryo-config --cflags`
AC_SUBST(embryo_libs)
AC_SUBST(embryo_cflags)

AC_PATH_GENERIC(edje, 0.5.0, [
	AC_SUBST(edje_libs)
	AC_SUBST(edje_cflags) ],
	AC_MSG_ERROR(Cannot find edje: Is edje-config in path?))
edje_libs=`edje-config --libs`
edje_cflags=`edje-config --cflags`
AC_SUBST(edje_libs)
AC_SUBST(edje_cflags)

edje_cc="`$EDJE_CONFIG --prefix`/bin/edje_cc"
AC_ARG_WITH(edje-cc,
[  --with-edje-cc=PATH              specify a specific path to edje_cc],
[
  v=$withval;
  edje_cc=$v
  echo "  Enlightenment edje_cc explicitly set to "$edje_cc;
],[
  edje_cc="`$EDJE_CONFIG --prefix`/bin/edje_cc"
])
AC_SUBST(edje_cc)

AC_PATH_GENERIC(enlightenment, 0.16.999, [
	AC_SUBST(e_libs)
	AC_SUBST(e_cflags) ],
	AC_MSG_ERROR(Cannot find enlightenment: Is enlightenment-config in path?))
e_libs=`enlightenment-config --libs`
e_cflags=`enlightenment-config --cflags`
AC_SUBST(e_libs)
AC_SUBST(e_cflags)

MODULE_ARCH="$host_os-$host_cpu"
AC_SUBST(MODULE_ARCH)

AC_PREFIX_DEFAULT(${HOME}/.e/e)
if test "x$prefix" = "xNONE"; then
  prefix=${ac_default_prefix}
  datarootdir=${prefix}
  moduledir=${prefix}/modules
else
  datarootdir=${prefix}/share
  moduledir=${libdir}/lib/enlightenment/modules
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
                 po/Makefile
                 src/Makefile])
AC_OUTPUT
