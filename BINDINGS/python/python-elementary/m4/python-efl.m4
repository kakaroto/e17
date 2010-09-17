dnl PY_EFL_PYTHONPATH(VARIABLE, MODULE, [ACTION-YES [, ACTION-NO]])
dnl checks if pkg-config MODULE specification provides $pythondir and
dnl $pyexecdir that are present in $PYTHONPATH.
dnl
dnl defines ${MODULE}_PYTHONPATH_CORRECT=yes/no
AC_DEFUN([PY_EFL_PYTHONPATH],
[

$1[]_PYTHONPATH_CORRECT=no

PKG_CHECK_EXISTS([$2],
   [$1[]_PYTHONPATH_CORRECT=yes], [$1[]_PYTHONPATH_CORRECT=no])

if test "x$[]$1[]_PYTHONPATH_CORRECT" != "xyes"; then
   ifelse([$4], [],
      [AC_MSG_WARN([$2 is not installed. This may result in runtime problems with python that uses this module.])],
          [$4])
else
   _pythondir=`pkg-config --variable=pythondir "$2"`
   _pyexecdir=`pkg-config --variable=pyexecdir "$2"`
   _missing_paths=""
   AM_CHECK_PYTHONPATH_CONTAINS([$_pythondir], [:],
      [$1[]_PYTHONPATH_CORRECT=no
       _missing_paths="$_missing_paths $_pythondir"
      ])

   if test "$_pythondir" != "$_pyexecdir"; then
      AM_CHECK_PYTHONPATH_CONTAINS([$_pyexecdir], [:],
         [$1[]_PYTHONPATH_CORRECT=no
          _missing_paths="$_missing_paths $_pythondir"
         ])
   fi

   if test "$[]$1[]_PYTHONPATH_CORRECT" = "yes"; then
       ifelse([$3], [], [:], [$3])
   else
       ifelse([$4], [],
          [AC_MSG_WARN([\$PYTHONPATH does not include${_missing_paths} where $2 is installed. This may result in runtime problems with python that uses this module.])],
          [$4])
   fi
fi

unset _missing_paths
unset _pythondir
unset _pyexecdir
])
