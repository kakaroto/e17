dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(exalt, [0.0.1], [exalt@xxx.com])
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.8)
AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST
AC_C___ATTRIBUTE__

define([AC_LIBTOOL_LANG_CXX_CONFIG], [:])dnl
define([AC_LIBTOOL_LANG_F77_CONFIG], [:])dnl
AC_PROG_LIBTOOL

m4_ifdef([AM_GNU_GETTEXT_VERSION], [
AM_GNU_GETTEXT_VERSION([0.14])
])

m4_ifdef([AM_GNU_GETTEXT], [
AM_GNU_GETTEXT([external])
po_makefile_in=po/Makefile.in
AM_CONDITIONAL([HAVE_PO], [true])
],[
AM_CONDITIONAL([HAVE_PO], [false])
])
AC_SUBST(LTLIBINTL)

PKG_CHECK_MODULES(E, [enlightenment])
release=$(pkg-config --variable=release enlightenment)
MODULE_ARCH="$host_os-$host_cpu-$release"
AC_SUBST(MODULE_ARCH)
AC_DEFINE_UNQUOTED(MODULE_ARCH, "$MODULE_ARCH", "Module architecture")

# Find edje_cc
PKG_CHECK_MODULES(EDJE, [edje >= 1.0.0])
AC_ARG_WITH(edje-cc,
  AC_HELP_STRING([--with-edje-cc=PATH], [specify a specific path to edje_cc]),
  [
    v=$withval;
    EDJE_CC=$v
  ],[
    EDJE_CC=$(pkg-config --variable=prefix edje)/bin/edje_cc
  ]
)
PKG_CHECK_MODULES(EXALT, [exalt >= 0.0.1])
PKG_CHECK_MODULES(EXALT_DBUS, [exalt_dbus >= 0.0.1])
PKG_CHECK_MODULES(E_NOTIFY, [enotify >= 1.0.0])
AC_SUBST(EDJE_CC)
AC_MSG_CHECKING([Which edje_cc to use])
AC_MSG_RESULT(${EDJE_CC})

datadir=$(pkg-config --variable=modules enlightenment)/${PACKAGE}
AC_ARG_ENABLE(homedir-install,
  AS_HELP_STRING([--enable-homedir-install], [Install module in homedir]),
  [ datadir="${HOME}/.e/e/modules/${PACKAGE}" ]
)

AC_ARG_ENABLE([elive],
              [  --enable-elive           add some extra buttons to launch some GTK apps.],
              [ case "${enableval}" in
               yes)    elive=yes ;;
               no) elive=no ;;
               esac],
               [elive=no])

if test "$elive" = "yes"; then
    AC_DEFINE_UNQUOTED(ELIVE, "${elive}", [Use the special Elive extension])
fi

AC_ARG_ENABLE([shave],
              [  --disable-shave                  Use shave output cleaner when building.],
              [ case "${enableval}" in
               yes)    enable_shave=yes;;
               no)    enable_shave=no;;
               *)    AC_MSG_ERROR([bad value ${enableval} for --disable-shave]);;
               esac],
               [enable_shave=yes])

if test x$enable_shave = xyes; then
    SHAVE_INIT([./])
fi

AC_CONFIG_FILES([
    shave
    shave-libtool
])

AC_OUTPUT([
Makefile
src/Makefile
module.desktop
e_modules-exalt-client.spec
$po_makefile_in
], [
])
