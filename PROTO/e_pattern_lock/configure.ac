AC_INIT([e_pattern_lock], [0.1.0], [rms@1407.org], [e_pattern_lock])
AC_CONFIG_SRCDIR(src/)
AM_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE

AC_PROG_CC
AC_GNU_SOURCE
AC_PROG_LIBTOOL

AM_GNU_GETTEXT(external)
AM_GNU_GETTEXT_VERSION([0.18.1])


PKG_CHECK_MODULES(ELEMENTARY, elementary, [],
		[AC_MSG_FAILURE([elementary not found])]
		)

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

AC_OUTPUT([
po/Makefile.in
Makefile
data/Makefile
data/theme/Makefile
data/theme/default/Makefile
e_pattern_lock.spec
src/Makefile
])
