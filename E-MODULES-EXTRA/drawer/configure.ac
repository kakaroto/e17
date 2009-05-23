dnl Process this file with autoconf to produce a configure script.

# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(drawer, 0.0.1, vkojouharov@gmail.com)
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

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION(0.14)

PKG_CHECK_MODULES(EPSILON, [epsilon])
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

datadir=$(pkg-config --variable=modules enlightenment)/${PACKAGE}

AC_ARG_ENABLE(homedir-install,
  AS_HELP_STRING([--enable-homedir-install], [Install module in homedir]),
  [ datadir="${HOME}/.e/e/modules/${PACKAGE}" ]
)

AC_ARG_ENABLE([xpixmap],
   [AC_HELP_STRING([--enable-xpixmap], [enable esmart_xpixmap support @<:@default=no@:>@])],
   [
    if test "x${enableval}" = "xyes" ; then
       _iv_enable_xpixmap="yes"
    else
       _iv_enable_xpixmap="no"
    fi
   ],
   [_iv_enable_xpixmap="no"]
)

AC_MSG_CHECKING([whether esmart_xpixmap code is built])
AC_MSG_RESULT([${_iv_enable_xpixmap}])

have_xpixmap="no"

if test "x${_iv_enable_xpixmap}" = "xyes" ; then
    have_xpixmap="yes"
    AC_DEFINE(HAVE_XPIXMAP, 1, [have esmart_xpixmap code support])
fi

AM_CONDITIONAL(BUILD_XPIXMAP, test $have_xpixmap = yes)

requirements_drawer="edje epsilon enlightenment"
AC_SUBST(requirements_drawer)

AC_OUTPUT([
Makefile
drawer.pc
data/Makefile
src/Makefile
src/plugins/Makefile
src/plugins/sources/Makefile
src/plugins/views/Makefile
po/Makefile.in
module.desktop
], [
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
echo "  Build with esmart_xpixmap support.......: $have_xpixmap"
echo
echo "------------------------------------------------------------------------"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo
