dnl $Id$

AC_INIT(src/envision.c)

AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE(envision, 0.1)

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

AC_EXPAND_DIR(DATA_DIR, "$datadir/envision")
AC_DEFINE_UNQUOTED(DATA_DIR, "$DATA_DIR", [Directory to install data files in])

AC_EXPAND_DIR(PLUGIN_DIR, "$libdir/envision")
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

PKG_CHECK_MODULES(ENVISION,
		  ecore
		  evas
		  edje
		  emotion)

AC_OUTPUT([
Makefile
data/Makefile
data/themes/Makefile
data/themes/trakvision/Makefile
data/themes/trakvision/images/Makefile
data/themes/trakvision/fonts/Makefile
m4/Makefile
src/Makefile
])

echo
echo "$PACKAGE $VERSION"
echo
echo
