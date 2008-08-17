dnl Process this file with autoconf to produce a configure script.
AC_INIT
AC_CONFIG_SRCDIR([src/convert.c])
AM_CONFIG_HEADER(src/config.h)
AM_INIT_AUTOMAKE(imlib2_tools, 0.0.0alpha3)

dnl Checks for programs.
AC_PROG_CC

dnl Checks for libraries.
dnl Replace `main' with a function in -libs:
AC_CHECK_LIB(ibs, main)
dnl Replace `main' with a function in -lpopt:
AC_CHECK_LIB(popt, main)

dnl Checks for header files.
AC_PATH_X
AC_HEADER_STDC
AC_CHECK_HEADERS(limits.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

dnl Checks for library functions.
AC_CHECK_FUNCS(strdup strtod strtol strtoul)

dnl Provided --with-imlib2 argument to choose
AC_ARG_WITH(imlib2,
    [  --with-imlib2=DIR       use imlib2 rooted at <DIR>],
    [ imlib2_path="$withval/bin/"
    CPPFLAGS="$CPPFLAGS -I$withval/include"
    LIBS="$LIBS -L$withval/lib" ])


AC_CHECK_LIB(Imlib2, imlib_context_set_display, 
	[ imlib2_libs=`${imlib2_path}imlib2-config --libs`;
		CFLAGS="$CFLAGS `${imlib2_path}imlib2-config --cflags`";
		LIBS="$LIBS `${imlib2_path}imlib2-config --libs`"],
	AC_MSG_ERROR(
	""
	"**********************************************************************"
	"* imlib2_convert requres Imlib2. Available from"
	"* http://enlightenment.org/ and mirrors."
	"**********************************************************************"
	), `${imlib2_path}imlib2-config --libs`
)

AC_SUBST(imlib2_libs)


AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
