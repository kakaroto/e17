dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(places, 0.1.0, dave@gurumeditation.it)
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_ISC_POSIX

AM_INIT_AUTOMAKE(1.8)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
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
PKG_CHECK_MODULES(EDJE, [edje >= 0.5.0])
AC_ARG_WITH(edje-cc,
  AC_HELP_STRING([--with-edje-cc=PATH], [specify a specific path to edje_cc]),
  [
    v=$withval;
    EDJE_CC=$v
  ],[
    EDJE_CC=$(pkg-config --variable=prefix edje)/bin/edje_cc
  ]
)
AC_SUBST(EDJE_CC)
AC_MSG_CHECKING([Which edje_cc to use])
AC_MSG_RESULT(${EDJE_CC})

dbus_mount="dbus-1, edbus >= 1.0.999"
hal_mount="ehal >= 1.0.999"
udisks_mount="eukit >= 1.0.999"
AC_ARG_ENABLE([mount-hal],
  AS_HELP_STRING([--enable-mount-hal],[enable hal mounting support @<:@default=enabled@:>@]),
  [e_cv_want_mount_hal=$enableval],
  AC_CACHE_VAL([e_cv_want_mount_hal], [e_cv_want_mount_hal=yes]))
if test "x$e_cv_want_mount_hal" != "xno";then
  PKG_CHECK_EXISTS([ehal >= 1.0.999],
    [],[
      hal_mount=
      AC_MSG_NOTICE([HAL mounting disabled])
    ])
else
  hal_mount=
  AC_MSG_NOTICE([HAL mounting disabled])
fi

AC_ARG_ENABLE([mount-udisks],
  AS_HELP_STRING([--enable-mount-udisks],[enable udisks mounting support @<:@default=enabled@:>@]),
  [e_cv_want_mount_udisks=$enableval],
  AC_CACHE_VAL([e_cv_want_mount_udisks], [e_cv_want_mount_udisks=yes]))
if test "x$e_cv_want_mount_udisks" != "xno";then
  PKG_CHECK_EXISTS([eukit >= 1.0.999],
    [],[
      udisks_mount=
      AC_MSG_NOTICE([udisks mounting disabled])
    ])
else
  udisks_mount=
  AC_MSG_NOTICE([udisks mounting disabled])
fi


if test "x${e_cv_want_mount_udisks}" = "xno" && test "x${e_cv_want_mount_hal}" = "xno" ; then
  DBUS_MOUNT_CONFIG=0
  dbus_mount=
  test "x${e_cv_want_mount_udisks}" = "xno" && udisks_mount=
  test "x${e_cv_want_mount_hal}" = "xno" && hal_mount=
fi

AM_CONDITIONAL([HAVE_HAL_MOUNT], [test -n "$hal_mount"])
AM_CONDITIONAL([HAVE_UDISKS_MOUNT], [test -n "$udisks_mount"])
test -n "$hal_mount" && AC_DEFINE_UNQUOTED([HAVE_HAL_MOUNT], [1], [enable HAL mounting])
test -n "$udisks_mount" && AC_DEFINE_UNQUOTED([HAVE_UDISKS_MOUNT], [1], [enable Udisks mounting])

PKG_CHECK_MODULES(E_DBUS, [$dbus_mount $hal_mount $udisks_mount])

datadir=$(pkg-config --variable=modules enlightenment)/${PACKAGE}
AC_ARG_ENABLE(homedir-install,
  AS_HELP_STRING([--enable-homedir-install], [Install module in homedir]),
  [ datadir="${HOME}/.e/e/modules/${PACKAGE}" ]
)

AC_OUTPUT([
Makefile
src/Makefile
module.desktop
e_modules-places.spec
$po_makefile_in
], [
])
