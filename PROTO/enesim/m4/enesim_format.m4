dnl use: ENESIM_SURFACE_FORMAT(format[, default-enabled])
AC_DEFUN([ENESIM_SURFACE_FORMAT],
[
pushdef([UP], translit([$1], [a-z], [A-Z]))dnl
pushdef([DOWN], translit([$1], [A-Z], [a-z]))dnl

format_[]DOWN="no"

ifelse("x$2", "xno",
[
  AC_ARG_ENABLE(format-[]DOWN,
    AC_HELP_STRING(
      [--enable-format-[]DOWN],
      [enable the $1 surface format]
    ),
    [ format_[]DOWN=$enableval ],
    [ format_[]DOWN=no ]
  )
], [
  AC_ARG_ENABLE(format-[]DOWN,
    AC_HELP_STRING(
      [--disable-format-[]DOWN],
      [disable the $1 surface format]
    ),
    [ format_[]DOWN=$enableval ],
    [ format_[]DOWN=yes ]
  )
])

AC_MSG_CHECKING(whether to build $1 surface format support)
AC_MSG_RESULT($format_[]DOWN)

if test "x$format_[]DOWN" = "xyes" ; then
  AC_DEFINE(BUILD_SURFACE_[]UP, 1, [$1 Surface Fromat Support])  
fi

AM_CONDITIONAL(BUILD_SURFACE_[]UP, test "x$format_[]DOWN" = "xyes")

popdef([UP])
popdef([DOWN])
])
