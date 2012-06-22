dnl FILTER_VAR_REGEXP(VARIABLE, REGEXP-TO-DELETE [, ACTION-IF-REMOVED])
dnl Applies "sed" regular expression to remove REGEXP-TO-DELETE from $VARIABLE
AC_DEFUN([FILTER_VAR_REGEXP],
[
_tmp_[]$1=`echo "$[]$1" | sed -e 's/$2//g'`
if test "x$_tmp_[]$1" != "x$[]$1"; then
   ifelse([$3], [], [AC_MSG_WARN([Removed $2 from \$[]$1])], [$3])
   $1="$_tmp_[]$1"
fi
])
