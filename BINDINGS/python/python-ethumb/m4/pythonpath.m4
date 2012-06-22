dnl _CHECK_PYTHONPATH(PATH, ACTION-YES, ACTION-NO)
AC_DEFUN([_CHECK_PYTHONPATH],
[
m4_define([pyprog], [import sys; sys.exit(not ('$1' in sys.path))])

changequote(, )
_cachevar=`echo "$1" | sed -e "s/[^a-zA-Z0-9_]/_0123_/g" `
changequote([, ])
_cachevar="py_cv_path_${_cachevar}"

AC_CACHE_CHECK([for $1 in \$PYTHONPATH], [$_cachevar],
   [
   _res=no
   if AC_RUN_LOG([$PYTHON -c "pyprog"]); then
      _res=yes
   fi
   eval "${_cachevar}=$_res"
   unset _res
   ])
eval "_res=\${${_cachevar}}"
if test "$_res" = "yes"; then
   $2
else
   $3
fi
])

dnl a macro to check if install location is in $PYTHONPATH or builtin locations
dnl  AM_CHECK_PYTHONPATH([ACTION-YES [, ACTION-NO]])
dnl defines PYTHONPATH_CORRECT=yes/no
AC_DEFUN([AM_CHECK_PYTHONPATH],
[AC_REQUIRE([AM_PATH_PYTHON])

_orig_exec_prefix="$exec_prefix"
if test -z "$exec_prefix" -o "$exec_prefix" = "NONE"; then
   exec_prefix="$prefix"
fi
_pythondir=`eval "echo $pythondir"`
_pyexecdir=`eval "echo $pyexecdir"`
exec_prefix="${_orig_exec_prefix}"
unset _orig_exec_prefix

PYTHONPATH_CORRECT=yes

_CHECK_PYTHONPATH([$_pythondir],
   [ifelse([$1], [], [:], [$1])],
   [PYTHONPATH_CORRECT=no
    ifelse([$2], [],
    [AC_MSG_WARN([\$PYTHONPATH does not include $_pythondir, this may result in runtime problems with python that uses this module.])],
    [$2])])

if test "$_pythondir" != "$_pyexecdir"; then
_CHECK_PYTHONPATH([$_pyexecdir],
   [ifelse([$1], [], [:], [$1])],
   [PYTHONPATH_CORRECT=no
    ifelse([$2], [],
    [AC_MSG_WARN([\$PYTHONPATH does not include $_pyexecdir, this may result in runtime problems with python that uses this module.])],
    [$2])])
fi

unset _pythondir
unset _pyexecdir
])

dnl a macro to check if given directory is in $PYTHONPATH or builtin locations
dnl  AM_CHECK_PYTHONPATH_CONTAINS(DIRECTORY [, ACTION-YES [, ACTION-NO]])
dnl defines PYTHONPATH_CORRECT=yes/no
AC_DEFUN([AM_CHECK_PYTHONPATH_CONTAINS],
[AC_REQUIRE([AM_PATH_PYTHON])

_dir="$1"
_dir=`expr "X$_dir" : 'X\(.*[^/]\)' \| "X$_dir" : 'X\(.*\)'`

_CHECK_PYTHONPATH([$_dir],
   [ifelse([$2], [], [:], [$2])],
   [ifelse([$3], [],
    [AC_MSG_WARN([\$PYTHONPATH does not include $_dir, this may result in runtime problems with python that uses this module.])],
    [$3])])

unset _dir
])
