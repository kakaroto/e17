dnl Configure script for Ewl
dnl Vincent Torri 2005-01-06
dnl
dnl AM_CHECK_EWL([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl Test for the Ewl library, and define EWL_LIBS and EWL_CFLAGS.
dnl
AC_DEFUN([AM_CHECK_EWL],
   [
    EWL_CONFIG="ewl-config"
    dnl
    dnl Get the prefix where ewl-config is installed.
    dnl
    AC_ARG_WITH(
       [ewl-config],
       AC_HELP_STRING(
          [--with-ewl-config=FILE],
          [ewl-config script to use (eg /usr/bin/ewl-config)]),
       dnl
       dnl Check the given ewl-config script.
       dnl
       [EWL_CONFIG=${withval}
        AC_CHECK_PROG(
           [EWL_CONFIG_IN_PATH],
           [${EWL_CONFIG}],
           [yes],
           [no])
        if test $EWL_CONFIG_IN_PATH = no; then
           echo "ERROR:"
           echo "The ewl-config development script you specified:"
           echo "$EWL_CONFIG"
           echo "was not found.  Please check the path and make sure "
           echo "the script exists and is executable."
           AC_MSG_ERROR(
              [Fatal Error: no ewl-config detected.])
        fi],
       [AC_CHECK_PROG(
           [EWL_CONFIG_IN_PATH],
           [${EWL_CONFIG}],
           [yes],
           [no])
        if test ${EWL_CONFIG_IN_PATH} = no; then
           echo "ERROR:"
           echo "The ewl-config development script was not found in your execute"
           echo "path. This may mean one of several things"
           echo "1. You may not have installed the ewl-devel (or ewl-dev)"
           echo "   packages."
           echo "2. You may have ewl installed somewhere not covered by your path."
           echo ""
           echo "If this is the case make sure you have the packages installed, AND"
           echo "that the ewl-config script is in your execute path (see your"
           echo "shell's manual page on setting the \$PATH environment variable), OR"
           echo "alternatively, specify the script to use with --with-ewl-config."
           AC_MSG_ERROR(
              [Fatal Error: no ewl-config detected.])
        fi])
    EWL_CFLAGS=`$EWL_CONFIG --cflags`
    EWL_LIBS=`$EWL_CONFIG --libs`
    dnl
    dnl Substitution
    dnl
    AC_SUBST([EWL_CFLAGS])
    AC_SUBST([EWL_LIBS])
   ])
dnl End of ewl.m4
