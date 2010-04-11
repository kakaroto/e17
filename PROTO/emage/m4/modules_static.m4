dnl use: EMAGE_MODULE(module, want_module, have_module)

AC_DEFUN([EMAGE_MODULE],
[

m4_pushdef([UP], m4_toupper([$1]))
m4_pushdef([DOWN], m4_tolower([$1]))

want_module="$2"
want_static_module="no"
have_module="$3"
have_module_[]DOWN="no"

AC_ARG_ENABLE([module-[]DOWN],
   [AC_HELP_STRING([--enable-module-[]DOWN], [enable $1 module])],
   [
    if test "x${enableval}" = "xyes" ; then
       want_module="yes"
    else
       if test "x${enableval}" = "xstatic" ; then
          want_module="static"
       else
          want_module="no"
       fi
    fi
   ]
)

AC_MSG_CHECKING([whether to enable $1 module])
AC_MSG_RESULT([${want_module}])

if test "x${have_module}" = "xno" -a "x${want_module}" = "xyes" -a "x${use_strict}" = "xyes" ; then
   AC_MSG_ERROR([$1 dependencies not found (strict dependencies checking)])
fi

AC_MSG_CHECKING([whether $1 module will be built])
AC_MSG_RESULT([${have_module}])

if test "x${have_module}" = "xyes" ; then
   if test "x${want_module}" = "xstatic" ; then
      have_module_[]DOWN="static"
      want_static_module="yes"
   else
      have_module_[]DOWN="yes"
   fi
fi

if test "x${have_module}" = "xyes" ; then
   AC_DEFINE(BUILD_MODULE_[]UP, [1], [UP Module Support])
fi

AM_CONDITIONAL(BUILD_MODULE_[]UP, [test "x${have_module}" = "xyes"])

if test "x${want_static_module}" = "xyes" ; then
   AC_DEFINE(BUILD_STATIC_MODULE_[]UP, [1], [Build $1 module inside the library])
   have_static_module="yes"
fi

AM_CONDITIONAL(BUILD_STATIC_MODULE_[]UP, [test "x${want_static_module}" = "xyes"])

m4_popdef([UP])
m4_popdef([DOWN])

])
