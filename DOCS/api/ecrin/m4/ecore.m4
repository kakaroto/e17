dnl Configure script for Ecore
dnl Vincent Torri 2005-01-06
dnl
dnl AM_CHECK_ECORE([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl Test for the Ecore library, and define ECORE_LIBS and ECORE_CFLAGS.
dnl
AC_DEFUN([AM_CHECK_ECORE],
   [
    ECORE_CONFIG="ecore-config"
    dnl
    dnl Get the prefix where ecore-config is installed.
    dnl
    AC_ARG_WITH(
       [ecore-config],
       AC_HELP_STRING(
          [--with-ecore-config=FILE],
          [ecore-config script to use (eg /usr/bin/ecore-config)]),
       dnl
       dnl Check the given ecore-config script.
       dnl
       [ECORE_CONFIG=${withval}
        AC_CHECK_PROG(
           [ECORE_CONFIG_IN_PATH],
           [${ECORE_CONFIG}],
           [yes],
           [no])
        if test $ECORE_CONFIG_IN_PATH = no; then
           echo "ERROR:"
           echo "The ecore-config development script you specified:"
           echo "$ECORE_CONFIG"
           echo "was not found.  Please check the path and make sure "
           echo "the script exists and is executable."
           AC_MSG_ERROR(
              [Fatal Error: no ecore-config detected.])
        fi],
       [AC_CHECK_PROG(
           [ECORE_CONFIG_IN_PATH],
           [${ECORE_CONFIG}],
           [yes],
           [no])
        if test ${ECORE_CONFIG_IN_PATH} = no; then
           echo "ERROR:"
           echo "The ecore-config development script was not found in your execute"
           echo "path. This may mean one of several things"
           echo "1. You may not have installed the ecore-devel (or ecore-dev)"
           echo "   packages."
           echo "2. You may have ecore installed somewhere not covered by your path."
           echo ""
           echo "If this is the case make sure you have the packages installed, AND"
           echo "that the ecore-config script is in your execute path (see your"
           echo "shell's manual page on setting the \$PATH environment variable), OR"
           echo "alternatively, specify the script to use with --with-ecore-config."
           AC_MSG_ERROR(
              [Fatal Error: no ecore-config detected.])
        fi])
    ECORE_CFLAGS=`$ECORE_CONFIG --cflags`
    ECORE_LIBS=`$ECORE_CONFIG --libs`
    dnl
    dnl Substitution
    dnl
    AC_SUBST([ECORE_CFLAGS])
    AC_SUBST([ECORE_LIBS])
   ])
dnl End of ecore.m4
