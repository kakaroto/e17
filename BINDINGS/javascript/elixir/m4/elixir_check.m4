dnl use: ELIXIR_CHECK_PKG_MODULE(FOO-BAR, foo-bar, description[, version])
AC_DEFUN([ELIXIR_CHECK_PKG_MODULE],
[
pushdef([SAFE], translit([$2], [-], [_]))dnl

have_[]SAFE="no"

AC_ARG_ENABLE([$2],
	[AS_HELP_STRING([--disable-$2], [$3])],
	[have_[]SAFE=$enableval],
	[have_[]SAFE=yes])
AC_MSG_CHECKING(whether to build $3)
AC_MSG_RESULT(${have_[]SAFE})

if test "x${have_[]SAFE}" = "xyes"; then
  PKG_CHECK_MODULES([$1], [$2 $4],
    [
      AC_DEFINE(BUILD_MODULE_$1, 1, [$3])
      have_[]SAFE="yes"
    ], [
      have_[]SAFE="no"
    ]
  )
  AC_MSG_CHECKING(whether $2 module is available)
  AC_MSG_RESULT([${have_[]SAFE}])
fi

AM_CONDITIONAL(BUILD_MODULE_$1, test x"${have_[]SAFE}" = x"yes")
if test x"${have_[]SAFE}" = x"yes"; then
   AC_DEFINE(BUILD_MODULE_$1, 1, [Set to 1 if $2 is build])
fi

popdef([SAFE])
])

dnl use: ELIXIR_CHECK_MODULE(FOO-BAR, foo-bar, description)
AC_DEFUN([ELIXIR_CHECK_MODULE],
[
AC_ARG_ENABLE([$2],
        [AS_HELP_STRING([--disable-$2], [$3])],
        [have_$2=$enableval],
        [have_$2=yes])
AC_MSG_CHECKING(whether to build $3)
if test "x${have_$2}" = "xyes"; then
   AC_DEFINE(MODULE_$1, 1, [$3])
fi
AC_MSG_RESULT(${have_$2})

AM_CONDITIONAL(BUILD_MODULE_$1, test x"${have_$2}" = x"yes")
if test x"${have_[]SAFE}" = x"yes"; then
   AC_DEFINE(BUILD_MODULE_$1, 1, [Set to 1 if $2 is build])
fi
])
