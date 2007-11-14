dnl use: EWL_CHECK_ENGINE(engine, description, version, dep[, ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])


AC_DEFUN([EWL_CHECK_ENGINE],
[
pushdef([COND], translit([$1], [a-z], [A-Z]))dnl
pushdef([ARG], translit([$1], [_], [-]))dnl

have_$1="no"

AC_ARG_ENABLE([]ARG,
  AC_HELP_STRING([--enable-[]ARG], [enable $2 support]),
  [enable_$1=$enableval],
  [enable_$1="auto"])

if test "x$4" = "xyes" -a "x$enable_$1" != "xno" ; then
  PKG_CHECK_EXISTS([evas-[]ARG >= $3],
    [have_$1="yes"],
    [have_$1="no"])
  if test "x$have_$1" = "xyes" ; then
    AC_DEFINE(ENABLE_EWL_[]COND, 1, [Enable $2])
  elif test "x$enable_$1" = "xyes" ; then
    AC_MSG_ERROR($2 requested but no evas/ecore support found)
  fi
fi

AM_CONDITIONAL(EWL_ENABLE_EVAS_[]COND, test "x$have_$1" = "xyes")

if test "x$have_$1" = "xyes" ; then
  ifelse([$5], , :, [$5])
else
  ifelse([$6], , :, [$6])
fi

popdef([ARG])
popdef([COND])
])