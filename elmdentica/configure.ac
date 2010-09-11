AC_PREREQ(2.53)
AC_INIT(elmdentica, 0.9.10, http://code.google.com/p/elmdentica/)
AM_INIT_AUTOMAKE(elmdentica, 0.9.10)
AC_CONFIG_SRCDIR(src/)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_GNU_SOURCE
AM_GNU_GETTEXT([external])
AC_ISC_POSIX
AC_PROG_CC
AC_STDC_HEADERS
AC_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR([m4])

PKG_CHECK_MODULES(GLIB, glib-2.0, [],
		[AC_MSG_FAILURE([glib not found])]
		)

PKG_CHECK_MODULES(SQLITE, sqlite3, [],
		[AC_MSG_FAILURE([sqlite3 not found])]
		)

PKG_CHECK_MODULES(CURL, libcurl, [],
		[AC_MSG_FAILURE([libcurl not found])]
		)

PKG_CHECK_MODULES(ELEMENTARY, elementary, [],
		[AC_MSG_FAILURE([elementary not found])]
		)

dnl AC_ARG_WITH([frameworkd], AS_HELP_STRING([--without-frameworkd], [Build without frameworkd-glib0 library (default: test)]))

dnl if test "x$with_frameworkd" != "xno"; then
dnl 	PKG_CHECK_MODULES(FRAMEWORKD_GLIB, libframeworkd-glib, [],
dnl 		[AC_MSG_FAILURE([libframeworkd-glib not found])]
dnl 		)
dnl 
dnl 	AC_DEFINE([HAVE_LIBFRAMEWORKD_GLIB], [1], [Whether to use libframeworkd_glib])
dnl fi

PTHREAD_CFLAGS="-pthread"
PTHREAD_LIBS="-lpthread"
AC_SUBST(PTHREAD_CFLAGS)
AC_SUBST(PTHREAD_LIBS)

BINDIR=`eval echo $bindir`
AC_SUBST([BINDIR])
DATADIR=`eval echo $datadir`
AC_SUBST([DATADIR])

CONFIGDIR="${sysconfdir}/default"
AC_SUBST([CONFIGDIR])

APMDIR="${sysconfdir}/apm"
AC_SUBST([APMDIR])

AC_SUBST([VERSION])

dnl ==============================================
dnl DBUS
dnl ==============================================

DBUS_GLIB_BIN="`$PKG_CONFIG --variable=exec_prefix dbus-glib-1`/bin"

dnl AC_ARG_WITH(dbusbindir,
dnl            AC_HELP_STRING([--with-dbusbindir=[=DIR]],
dnl                           [Use a different path to dbus tools]),,
dnl                           dbusdir=$withval;)

AC_ARG_WITH(dbusbindir,[  --with-dbusbindir=DIR  dbus tools in DIR], [DBUS_GLIB_BIN="$withval"])
echo "DBus tools location ${DBUS_GLIB_BIN}"
AC_SUBST(DBUS_GLIB_BIN)

AC_OUTPUT([
po/Makefile.in
Makefile
elmdentica.spec
data/Makefile
data/elmdentica.desktop
data/elmdentica.conf
src/Makefile
])
