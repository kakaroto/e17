AC_INIT([e_pattern_lock], [0.1.0], [rms@1407.org], [e_pattern_lock])
AC_CONFIG_SRCDIR(src/)
AM_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE

AC_PROG_CC
AC_GNU_SOURCE
AC_PROG_LIBTOOL

AM_GNU_GETTEXT(external)
AM_GNU_GETTEXT_VERSION([0.12.1])


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

AC_SUBST(VERSION)

dnl AC_ARG_WITH(dbusbindir,
dnl            AC_HELP_STRING([--with-dbusbindir=[=DIR]],
dnl                           [Use a different path to dbus tools]),,
dnl                           dbusdir=$withval;)

AC_ARG_WITH(dbusbindir,[  --with-dbusbindir=DIR  dbus tools in DIR], [DBUS_GLIB_BIN="$withval"])
echo "DBus tools location ${DBUS_GLIB_BIN}"
AC_SUBST(DBUS_GLIB_BIN)

AC_OUTPUT([
Makefile
data/Makefile
data/theme/Makefile
data/theme/default/Makefile
po/Makefile.in
src/Makefile
])
