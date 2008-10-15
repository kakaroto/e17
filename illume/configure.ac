rm -f config.cache

AC_PREREQ(2.59)
AC_INIT([illume], [0.0.1], [raster@rasterman.com])
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.8 foreign])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AC_PROG_CC
AM_PROG_LIBTOOL
AC_HEADER_STDC

#ALL_LINGUAS="fr"
#AC_SUBST(ALL_LINGUAS)
#AM_GNU_GETTEXT([external])
#AM_GNU_GETTEXT_VERSION(0.14)

MODULE_ARCH="$host_os-$host_cpu"
AC_SUBST(MODULE_ARCH)
AC_DEFINE_UNQUOTED(MODULE_ARCH, "$MODULE_ARCH", "Module architecture")

edatadir="${datadir}/enlightenment/data"
AC_SUBST(edatadir)

datadir="${libdir}/enlightenment/modules/${PACKAGE}"
localedir=${datadir}/locale
AC_SUBST(LOCALEDIR, "${localedir}")
AC_DEFINE_UNQUOTED(LOCALEDIR, "${localedir}", "Module Locale Directory")

#
# Check EFL Libs
#
PKG_CHECK_MODULES(EDJE, [edje >= 0.5.0])
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
	
PKG_CHECK_MODULES(EET, [eet >= 1.0.1])
AC_ARG_WITH(eet-eet,
[  --with-eet-eet=PATH              specify a specific path to eet utility],
[
  v=$withval;
  eet_eet=$v
  echo "  Enlightenment eet explicitly set to "$eet_eet;
],[
  eet_eet=$(pkg-config --variable=prefix eet)/bin/eet
])
AC_SUBST(eet_eet)
	
#
# Check Enlightenment
#

PKG_CHECK_MODULES(E, [
  evas
  ecore
  ecore-evas
  ecore-file
  ecore-ipc
  ecore-con
  ecore-job
  ecore-imf
  ecore-imf-evas
  edje
  eet
  embryo
  efreet
  efreet-mime
])
PKG_CHECK_MODULES(e, [enlightenment >= 0.16.999.042])
e_cflags="$e_CFLAGS $E_CFLAGS"
e_modules="$(pkg-config --variable=modules enlightenment)"
e_libs="$E_LIBS"

PKG_CHECK_MODULES(ECORE, ecore)
PKG_CHECK_MODULES(E_DBUS, edbus, have_edbus=yes, have_edbus=no)
PKG_CHECK_MODULES(E_HAL, ehal, have_ehal=yes, have_ehal=no)
AM_CONDITIONAL(HAVE_EDBUS, test x$have_edbus = "xyes")
if test x$have_edbus = "xyes"; then
    AC_DEFINE(HAVE_EDBUS, 1, [E_Dbus support])
fi

AC_SUBST(e_cflags)
AC_SUBST(e_libs)
AC_SUBST(e_modules)

AC_OUTPUT([
Makefile
src/Makefile
config/Makefile
config/illume/Makefile
config/asu/Makefile
],[])

#po/Makefile
