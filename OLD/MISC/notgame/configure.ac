dnl# $Id$

AC_INIT(src/notgame.c)
AM_INIT_AUTOMAKE(notgame, 0.9)

dnl# Supply default CFLAGS, if not specified by `CFLAGS=flags ./configure'
if test ! -z "${CFLAGS}" -o ! -z "${CCOPTS}"; then
  CFLAGS_GIVEN=1
else
  CFLAGS_GIVEN=0
fi

if test -z "${CFLAGS}"; then
    if test -z "${CCOPTS}"; then
	CCOPTS="-O"
    fi
    CFLAGS="$CCOPTS"
fi

AC_PROG_CC
AC_PROG_CPP
AC_DEFINE_UNQUOTED(PKGDATADIR, "${prefix}/share/$PACKAGE")

dnl# These must be run after AC_PROG_CC but before any other macros that use
dnl# the C compiler
AC_AIX
AC_ISC_POSIX
AC_MINIX

dnl# At least make the attempt to support CygWin32
AC_CYGWIN
AC_ARG_PROGRAM

AC_GCC_TRADITIONAL

dnl # If using gcc, use -O2.  If -g works with it, use that too
if test "${CFLAGS_GIVEN}" -ne 1; then
    if test "$GCC" = "yes"; then
	if test "$ac_cv_prog_cc_g" = "yes"; then
	    CCOPTS='-g -O2'
	else
	    CCOPTS='-O2'
	fi
	CFLAGS="$CCOPTS"
    fi
fi

AC_PROG_INSTALL

dnl# Check for host system type
AC_CANONICAL_HOST

dnl# Check the sanity of what we've done so far
AM_SANITY_CHECK

dnl# Most people don't want the developer-only clutter
AM_MAINTAINER_MODE

dnl# If it's there, what the hell?
AM_WITH_DMALLOC

dnl# Look for needed programs
AC_CHECK_PROG(SED, sed, sed, false)
AC_CHECK_PROG(RM, rm, rm, true)
AC_CHECK_PROG(CP, cp, cp, false)
AC_CHECK_PROG(CHMOD, chmod, chmod, true)
AC_CHECK_PROG(TAR, tar, tar, tar)
AC_CHECK_PROG(MKDIR, mkdir, mkdir, false)
AC_CHECK_PROG(CTAGS, ctags, ctags, true)
AC_CHECK_PROG(AR, ar, ar, false)
AC_CHECK_PROG(MV, mv, mv, true)
AC_LN_S

AC_C_CONST
AC_C_INLINE
AC_PATH_XTRA

dnl# Checks for header files.
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(fcntl.h strings.h sys/ioctl.h sys/time.h unistd.h)
AC_HEADER_TIME

dnl# Checks for library functions.
AC_CHECK_FUNCS(strerror)

CFLAGS="`imlib-config --cflags-gdk` `gtk-config --cflags` $CFLAGS"
LDFLAGS="`imlib-config --libs-gdk` `gtk-config --libs`"

dnl# Did they want debugging?
AC_MSG_CHECKING(for debugging level)
AC_ARG_WITH(debugging, [  --with-debugging[=num]  enable debugging output, num is an optional level],
              if test "$withval" = "yes"; then
                  withval=1
              else
                  :
              fi
              if test "$withval" != "no"; then 
                  echo "$withval"
                  AC_DEFINE_UNQUOTED(DEBUG, $withval)
              else
                  echo "no debugging"
                  AC_DEFINE_UNQUOTED(DEBUG, 0)
              fi, echo "no debugging"
                  AC_DEFINE_UNQUOTED(DEBUG, 0)
)

CPPFLAGS="$CPPFLAGS"
AC_SUBST(CC)
AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(LIBS)

AM_CONFIG_HEADER(config.h)

AC_OUTPUT(Makefile src/Makefile)

echo "
$PACKAGE $VERSION
Configuration:
--------------

  Source code location:       ${srcdir}
  Host System Type:           ${host}
  Compiler:	              ${CC} ${CFLAGS}
  Link flags:                 ${LDFLAGS}
  Install path:		      ${prefix}

  Now type 'make' to build $PACKAGE $VERSION.
"
