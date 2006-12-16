dnl Process this file with autoconf to produce a configure script.
AC_INIT(ipc.c)
AM_INIT_AUTOMAKE(e16keyedit, 0.5)

AM_CONFIG_HEADER(config.h)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl Checks for libraries.
AC_ARG_ENABLE(gtk2,
	[AC_HELP_STRING([--enable-gtk2],[enable gtk2 GUI (instead of GTK1) [default=yes]])],,
	[enable_gtk2=yes])

if test "x$enable_gtk2" = "xyes" ; then
	pkg_modules="gtk+-2.0 >= 2.0.0"
	PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
	CFLAGS="$CFLAGS $PACKAGE_CFLAGS"
	LIBS="$LIBS $PACKAGE_LIBS"
	AC_DEFINE(ENABLE_GTK2, , [Define to enable gtk2 support])
else
	if ! gtk-config --version &> /dev/null ; then
		AC_MSG_ERROR([Gtk+ is required.])
	else
		CFLAGS="$CFLAGS `gtk-config --cflags`"
		LIBS="$LIBS `gtk-config --libs`"
	fi
fi

AC_SUBST(ENABLE_GTK2)

dnl Checks for header files.
AC_PATH_X
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.

AC_OUTPUT(Makefile e16keyedit.spec)
