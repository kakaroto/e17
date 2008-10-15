rm -f config.cache

AC_PREREQ(2.59)
AC_INIT([bling], [0.0.1], [ibukun@xcomputerman.com])
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.8 foreign])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AC_PROG_CC
AM_PROG_LIBTOOL
AC_HEADER_STDC

#AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION(0.14)

MODULE_ARCH="$host_os-$host_cpu"
AC_SUBST(MODULE_ARCH)
AC_DEFINE_UNQUOTED(MODULE_ARCH, "$MODULE_ARCH", "Module architecture")

datadir="${libdir}/enlightenment/modules/${PACKAGE}"
localedir=${datadir}/locale
AC_SUBST(LOCALEDIR, "${localedir}")
AC_DEFINE_UNQUOTED(LOCALEDIR, "${localedir}", "Module Locale Directory")

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

# Check enlightenment
PKG_CHECK_MODULES(E, [enlightenment])

datadir=$(pkg-config --variable=modules enlightenment)/${PACKAGE}

#######################################
## Simple X11 build/link
AC_ARG_ENABLE(simple-x11,
  AC_HELP_STRING([--enable-simple-x11], [enable simple x11 linking]),
  [ want_evas_simple_x11=$enableval ]
)

x_dir=""
x_cflags=""
x_libs=""
AC_PATH_XTRA
AC_CHECK_HEADER(X11/X.h,
  [
    if test "x$want_evas_simple_x11" = "xyes"; then
      x_libs="${x_libs} -lX11 -lXext -lXrender -lXcomposite -lXdamage -lXfixes"
    else
      x_dir=${x_dir:-/usr/X11R6}
      x_cflags=${x_cflags:--I${x_includes:-$x_dir/include}}
      x_libs="${x_libs:--L${x_libraries:-$x_dir/lib}} -lX11 -lXext -lXrender -lXcomposite -lXdamage -lXfixes"
    fi
  ],[
    AC_MSG_ERROR([Cannot find X headers and libraries.])
  ]
)
AC_SUBST(x_cflags)
AC_SUBST(x_libs)

x_cflags="-I/usr/X11R6/include"
x_libs="-L/usr/X11R6/lib -lX11 -lXext -lXrender -lXcomposite"
AC_SUBST(x_cflags)
AC_SUBST(x_libs)

AC_OUTPUT([
Makefile
e_modules-bling.spec
], [
])

