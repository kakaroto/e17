###############################################################################
##
## $Id$
##
## Copyright (C) 2004 Embrace project.
##
## This program is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by the
## Free Software Foundation; either version 2, or (at your option) any
## later version.
##
## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
###############################################################################

AC_PREREQ(2.53)

AC_INIT(embrace, 0.0.1, , embrace)
AC_CONFIG_SRCDIR([src/embrace.c])

AM_INIT_AUTOMAKE([dist-bzip2])
AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_INSTALL

AM_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_HEADER_STDC
AC_HEADER_STDBOOL

AH_VERBATIM([_GNU_SOURCE], [
/* Enable GNU extensions. */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
])

AH_VERBATIM([__USE_GNU], [
/* Enable GNU extensions. */
#ifndef __USE_GNU
# define __USE_GNU
#endif
])

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

AC_ARG_WITH(maildir,
	[  --with-maildir          build the maildir plugin (default: yes)],
	[build_maildir=$withval], build_maildir=yes)

AM_CONDITIONAL(BUILD_MAILDIR, test x$build_maildir = xyes)

AC_ARG_WITH(mbox,
	[  --with-mbox             build the mbox plugin (default: yes)],
	[build_mbox=$withval], build_mbox=yes)

AM_CONDITIONAL(BUILD_MBOX, test x$build_mbox = xyes)

AC_ARG_WITH(pop3,
	[  --with-pop3             build the pop3 plugin (default: yes)],
	[build_pop3=$withval], build_pop3=yes)

AM_CONDITIONAL(BUILD_POP3, test x$build_pop3 = xyes)

AC_PATH_GENERIC(ecore, , ,
	AC_MSG_ERROR(Cannot find ecore: Is ecore-config in path?))

AC_PATH_GENERIC(evas, , ,
	AC_MSG_ERROR(Cannot find evas: Is evas-config in path?))

AC_PATH_GENERIC(edje, , ,
	AC_MSG_ERROR(Cannot find edje: Is edje-config in path?))

AC_PATH_GENERIC(esmart, , ,
	AC_MSG_ERROR(Cannot find esmart: Is esmart-config in path?))

AC_CHECK_LIB(ltdl, lt_dlopen, ,
	AC_MSG_ERROR(Cannot find libltdl))

AC_CHECK_HEADERS(ltdl.h, ,
	AC_MSG_ERROR(Cannot find ltdl.h))

DL_LIBS=-lltdl
AC_SUBST(DL_LIBS)

AC_CONFIG_FILES([
Makefile
data/Makefile
data/themes/Makefile
data/themes/default/Makefile
data/themes/default/fonts/Makefile
data/themes/default/images/Makefile
data/themes/default/parts/Makefile
data/themes/default/programs/Makefile
m4/Makefile
src/Makefile
src/plugins/Makefile
src/plugins/maildir/Makefile
src/plugins/mbox/Makefile
src/plugins/pop3/Makefile
])

AC_OUTPUT

echo
echo "$PACKAGE $VERSION"
echo
echo
echo "Plugins"
echo
echo "Maildir...............: $build_maildir"
echo "mbox..................: $build_mbox"
echo "POP3..................: $build_pop3"
echo
echo
echo "$PACKAGE $VERSION will be installed in $prefix"
echo
echo "Type 'make' ('gmake' on some systems) to compile $PACKAGE."
echo

