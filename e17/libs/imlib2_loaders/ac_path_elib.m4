# This file defines standard E Library checks which are commonly used.
#
# With thanks to a long list of autoconf macros that were lying around
# on my linux box. ;-)
#

dnl
dnl AM_PATH_ELIB(NAME, MINIMUM-VERSION, [Config Script Name, [TRUE-ACTION [, FALSE-ACTION]]])
dnl Note: Name should be <NAME>-config
dnl 

AC_DEFUN([AC_PATH_ELIB], [
dnl
dnl Writing 9 or 10 different macros that did the same thing in this
dnl file seemed almost as bad as writing it in each of the configure.in
dnl files.  These defines allow me to futher genericize it into one single
dnl macro for all of the enlightenment libraries.
dnl 
pushdef([UCASE], translit([$1], [a-z], [A-Z]))
pushdef([LCASE], translit([$1], [A-Z], [a-z]))
pushdef([ELIB], ac_path_elib_[]LCASE)
ELIB[]_cscript=ifelse([$3], , [$1-config], [$3])
no_[]LCASE=no
dnl
dnl Config Path Options
dnl

dnl
dnl This is probably an ugly hack, but it allows more flexibility 
dnl by allowing both x-config args (if you rename it for some reason,
dnl or write your own or whatever) and /path/to/config-script args.
dnl
PATH=$PATH:/
AC_ARG_WITH(LCASE-config,
[  --with-]LCASE[-config=CFG  path to ]LCASE[-config script (optional)],
[
   ELIB[]_cscript=$withval
   AC_CHECK_PROG(UCASE[]_CONFIG_IN_PATH, $ELIB[]_cscript, yes, no)
   if test $UCASE[]_CONFIG_IN_PATH = no; then
      echo "ERROR:"
      echo "The LCASE[]-config development script you specified:"
      echo "$ELIB[]_cscript"
      echo "was not found.  Please check the path and make sure "
      echo "the script exists and is executable."
      AC_MSG_WARN([Fatal Error: no $ELIB[]_cscript detected.])
      no_[]LCASE=yes
   fi
],
[
  AC_CHECK_PROG(UCASE[]_CONFIG_IN_PATH, $ELIB[]_cscript, yes, no)
  if test $UCASE[]_CONFIG_IN_PATH = no; then
    echo "ERROR:"
    echo "The LCASE[]-config development script was not found in your execute"
    echo "path. This may mean one of several things"
    echo "1. You may not have installed the LCASE[]-devel (or LCASE[]-dev)"
    echo "   packages."
    echo "2. You may have LCASE installed somewhere not covered by your path."
    echo ""
    echo "If this is the case make sure you have the packages installed, AND"
    echo "that the LCASE-config script is in your execute path (see your"
    echo "shell's manual page on setting the \$PATH environment variable), OR"
    echo "alternatively, specify the script to use with --with-[]LCASE[]-config."
    AC_MSG_WARN([Fatal Error: no LCASE[]-config detected.])
    no_[]LCASE=yes
  fi
])  

dnl
dnl Version Checking
dnl
ELIB[]_min_version=$2

if test $no_[]LCASE = no; then
   if test no$ELIB[]_min_version != no; then
      AC_MSG_CHECKING(for LCASE version >= $ELIB[]_min_version)
      LCASE[]_version=`$ELIB[]_cscript --version`

      LCASE[]_cflags=`$ELIB[]_cscript --cflags`
      LCASE[]_libs=`$ELIB[]_cscript --libs`

      LCASE[]_major=`echo $LCASE[]_version | \
                           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
      LCASE[]_minor=`echo $LCASE[]_version | \
                           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
      LCASE[]_micro=`echo $LCASE[]_version | \
                           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

      LCASE[]_major_min=`echo $ELIB[]_min_version | \
                           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
      LCASE[]_minor_min=`echo $ELIB[]_min_version | \
                           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
      LCASE[]_micro_min=`echo $ELIB[]_min_version | \
                           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

      ELIB[]_version_check=`expr \
                     $LCASE[]_major \> $LCASE[]_major_min \| \
                     $LCASE[]_major \= $LCASE[]_major_min \& \
                     $LCASE[]_minor \> $LCASE[]_minor_min \| \
                     $LCASE[]_minor \= $LCASE[]_minor_min \& \
                     $LCASE[]_micro \> $LCASE[]_micro_min \| \
                     $LCASE[]_micro \= $LCASE[]_micro_min`

      if test "$ELIB[]_version_check"="1"; then
         AC_MSG_RESULT([$LCASE[]_major.$LCASE[]_minor.$LCASE[]_micro])
      else
         AC_MSG_RESULT(no)
         echo ""
         echo "This package requires $LCASE version $ELIB[]_min_version or greater."
         echo "Please upgrade your software or use the --with-[]LCASE[]-config "
         echo "option to provide the path to the appropriate version."
         AC_MSG_WARN([Error: no compatible version of LCASE detected.]);
         no_[]LCASE=yes
      fi
   fi
fi

if test x$no_[]LCASE = xyes; then
   LCASE[]_cflags=""
   LCASE[]_libs=""
   ifelse([$5], , :, [$5])
else
   ifelse([$4], , :, [$4])
fi

AC_SUBST(LCASE[]_cflags)
AC_SUBST(LCASE[]_libs)

popdef([UCASE])
popdef([LCASE])
popdef([ELIB])
])

