dnl use: EXPEDITE_CHECK_ENGINE(Engine, Description, want[, ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
AC_DEFUN([EXPEDITE_CHECK_ENGINE],
[
pushdef([UP], translit([$1], [-a-z], [_A-Z]))dnl
pushdef([DOWN], translit([$1], [-A-Z], [_a-z]))dnl

want_[]DOWN=$3
have_[]DOWN="no"

AC_ARG_ENABLE($1,
  AC_HELP_STRING([--enable-$1], [enable $2 engine]),
  [ want_[]DOWN=$enableval ]
)

AC_MSG_CHECKING([whether $2 engine is to be built])
AC_MSG_RESULT($want_[]DOWN)

if test "x$want_[]DOWN" = "xyes"; then
  PKG_CHECK_EXISTS([evas-$1],
    [
      AC_DEFINE(HAVE_EVAS_[]UP, 1, [Evas $2 Engine Support])
      have_[]DOWN="yes"
    ],
    [ have_[]DOWN="no" ]
  )
fi

AC_MSG_CHECKING([whether $2 engine is available])
AC_MSG_RESULT($have_[]DOWN)

AM_CONDITIONAL(BUILD_[]UP, [test "x$have_[]DOWN" = "xyes"])

if test "x$have_[]DOWN" = "xyes" ; then
  ifelse([$4], , :, [$4])
else
  ifelse([$5], , :, [$5])
fi

popdef([UP])
popdef([DOWN])
])
