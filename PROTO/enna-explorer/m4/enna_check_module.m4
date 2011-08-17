
dnl use: ENNA_CHECK_MODULE_DEP_VOLUME_MTAB(module[, ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])

AC_DEFUN([ENNA_CHECK_MODULE_DEP_VOLUME_MTAB],
[

requirement=""
have_dep="yes"
enna_module_[]$1[]_cflags=""
enna_module_[]$1[]_libs=""

AC_SUBST([enna_module_$1_cflags])
AC_SUBST([enna_module_$1_libs])

if test "x${have_dep}" = "xyes" ; then
  m4_default([$2], [:])
else
  m4_default([$3], [:])
fi

])
dnl use: ENNA_CHECK_MODULE_DEP_VOLUME_UDEV(module[, ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])

AC_DEFUN([ENNA_CHECK_MODULE_DEP_VOLUME_UDEV],
[

requirement=""
have_dep="no"
enna_module_[]$1[]_cflags=""
enna_module_[]$1[]_libs=""

PKG_CHECK_MODULES([UDEV],
   [libudev],
   [
    have_dep="yes"
    enna_module_[]$1[]_cflags="${UDEV_CFLAGS}"
    enna_module_[]$1[]_libs="${UDEV_LIBS}"
   ],
   [have_dep="no"])

AC_SUBST([enna_module_$1_cflags])
AC_SUBST([enna_module_$1_libs])

if test "x${have_dep}" = "xyes" ; then
  m4_default([$2], [:])
else
  m4_default([$3], [:])
fi

])

dnl use: ENNA_CHECK_MODULE(module, want_module, description)


AC_DEFUN([ENNA_CHECK_MODULE],
[

m4_pushdef([UP], m4_translit([$1], [-a-z], [_A-Z]))dnl
m4_pushdef([DOWN], m4_translit([$1], [-A-Z], [_a-z]))dnl

want_module="$2"
want_static_module="no"
have_module="no"
have_enna_module_[]DOWN="no"

AC_ARG_ENABLE([$1],
   [AC_HELP_STRING([--enable-$1], [enable $4 module])],
   [
    if test "x${enableval}" = "xyes" ; then
       want_module="yes"
    else
       want_module="no"
    fi
   ])

AC_MSG_CHECKING([whether to enable $4 module])
AC_MSG_RESULT([${want_module}])

if test "x${want_module}" = "xyes" || test "x${want_module}" = "xauto" ; then
   m4_default([ENNA_CHECK_MODULE_DEP_]m4_defn([UP]))(DOWN, [have_module="yes"], [have_module="no"])
fi

if test "x${have_module}" = "xno" && test "x${want_module}" = "xyes" ; then
   AC_MSG_ERROR([$4 dependencies not found (strict dependencies checking)])
fi

AC_MSG_CHECKING([whether $4 rendering backend will be built])
AC_MSG_RESULT([${have_module}])

have_enna_module_[]DOWN=${have_module}

if test "x${have_module}" = "xyes" ; then
   AC_DEFINE(BUILD_MODULE_[]UP, [1], [$4 rendering backend])
fi

AM_CONDITIONAL(BUILD_MODULE_[]UP, [test "x${have_module}" = "xyes"])

m4_popdef([UP])
m4_popdef([DOWN])

])
