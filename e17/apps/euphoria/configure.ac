dnl $Id$

AC_INIT(src/euphoria.c)

AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE(euphoria, 0.7.5)

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

AC_EXPAND_DIR(DATA_DIR, "$datadir/$PACKAGE")
AC_DEFINE_UNQUOTED(DATA_DIR, "$DATA_DIR", [Directory to install data files in])

AC_EXPAND_DIR(PLUGIN_DIR, "$libdir/$PACKAGE")
AC_DEFINE_UNQUOTED(PLUGIN_DIR, "$PLUGIN_DIR", [Directory to install plugins in])
AC_SUBST(PLUGIN_DIR)

AC_EXPAND_DIR(SYSCONF_DIR, "$sysconfdir")
AC_DEFINE_UNQUOTED(SYSCONF_DIR, "$SYSCONF_DIR", [Directory to install configuration in])
AC_SUBST(SYSCONF_DIR)

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

AC_PATH_GENERIC(edje, , ,
	AC_MSG_ERROR(Cannot find edje: Is edje-config in path?))

AC_PATH_GENERIC(esmart, , ,
	AC_MSG_ERROR(Cannot find esmart: Is esmart-config in path?))

AC_PATH_GENERIC(ewl, , ,
	AC_MSG_ERROR(Cannot find ewl: Is ewl-config in path?))

AC_OUTPUT([
Makefile
data/Makefile
data/fonts/Makefile
data/themes/Makefile
data/themes/default/Makefile
data/themes/default/images/Makefile
data/themes/streamline/Makefile
data/themes/streamline/images/Makefile
data/themes/iio/Makefile
data/themes/iio/collections/Makefile
data/themes/iio/fonts/Makefile
data/themes/iio/img/Makefile
data/themes/iio/programs/Makefile
data/themes/winter/Makefile
data/themes/winter/groups/Makefile
data/themes/winter/macros/Makefile
data/themes/winter/images/Makefile
data/themes/winter/img/Makefile
data/themes/winter/parts/Makefile
data/themes/winter/programs/Makefile
etc/Makefile
m4/Makefile
src/Makefile
])
