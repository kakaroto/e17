dnl Process this file with autoconf to produce a configure script.

AC_INIT
AC_CONFIG_SRCDIR([configure.in])
AM_INIT_AUTOMAKE(ewd, 0.0.1)
AM_CONFIG_HEADER(config.h)

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AM_PROG_LIBTOOL

AC_ARG_ENABLE(pthreads,
[--disable-pthreads            disable building with pthread support],
[
  if test x$enableval = xyes; then
    pthreads=yes
  else
    pthreads=no
  fi
]
)

dnl Set PACKAGE_LOCALE_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${ac_default_prefix}/${DATADIRNAME}/locale")
else
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${prefix}/${DATADIRNAME}/locale")
fi

dnl Set PACKAGE_DATA_DIR in config.h.
if test "x${datadir}" = 'x${prefix}/share'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}")
  else
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}")
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${datadir}/${PACKAGE}")
fi

dnl Set PACKAGE_SOURCE_DIR in config.h.
packagesrcdir=`cd $srcdir && pwd`
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}")

AC_SUBST(PTHREAD_LIBS)
AC_SUBST(HAVE_PTHREADS)

if test x$pthreads = xyes ; then
AC_CHECK_HEADERS(pthread.h,AC_DEFINE_UNQUOTED(HAVE_PTHREADS, "yes"),
  AC_MSG_ERROR([*** POSIX thread support not found ***]))
PTHREAD_LIBS=-lpthread
HAVE_PTHREADS=1
else
HAVE_PTHREADS=0
fi

dnl Use -Wall if we have gcc.
changequote(,)dnl
if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;;
  esac
fi
changequote([,])dnl

AC_CONFIG_FILES([
Makefile
ewd-config
src/Makefile
test/Makefile
src/ewd_config.h
])
AC_CONFIG_COMMANDS([default],[[
chmod +x ewd-config

]],[[]])
AC_OUTPUT
