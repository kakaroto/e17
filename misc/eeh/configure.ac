AC_INIT(src/eeh.c)

AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE(eeh, 0.1)

AC_PROG_CC
AC_ISC_POSIX
AC_PROG_INSTALL
AC_HEADER_STDC
AC_HEADER_STDBOOL
AC_C_BIGENDIAN
AM_DISABLE_STATIC
AC_PROG_LIBTOOL

AH_VERBATIM([_BOOL], [
/* Define bool on systems that don't support C99 */
#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#else
# define bool int
# define true 1
# define false 0
#endif
])

AC_ARG_ENABLE(debug,
	[  --enable-debug          debug build (default: no)], [
		DEBUG_LVL=999
	], DEBUG_LVL=1)

AC_DEFINE_UNQUOTED([DEBUG_LEVEL], $DEBUG_LVL, [
	Define to 999 if you want much debugging output
])

AC_PATH_GENERIC(ecore, , ,
	AC_MSG_ERROR(Cannot find ecore: Is ecore-config in path?))

AC_PATH_GENERIC(evas, , ,
	AC_MSG_ERROR(Cannot find evas: Is evas-config in path?))


AC_OUTPUT([
Makefile
m4/Makefile
src/Makefile
])

echo
echo "$PACKAGE $VERSION"
echo
echo
