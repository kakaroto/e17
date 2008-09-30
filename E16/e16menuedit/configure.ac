dnl Process this file with autoconf to produce a configure script.

AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(e16menuedit2, 0.0.3)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC

dnl Option to enable debug
AC_MSG_CHECKING(whether to enable debugging)
AC_ARG_ENABLE(debug,
        [  --enable-debug=[no/yes]        enables debug build (default=no)],,
                 enable_debug=no)

if [ ! test "x$enable_debug" != "xyes"]; then
  AC_DEFINE(DEBUG, , [Define to enable debug build])
  CFLAGS="$CFLAGS -g"
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

pkg_modules="gtk+-2.0 >= 2.4.0 libglade-2.0 >= 2.3.6 gmodule-2.0 >= 0.17.2"
PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

AC_CHECK_LIB(popt,
             poptGetContext,
             LIBS="${PACKAGE_LIBS} -lpopt",
             echo "*** You need to get libpopt ***";exit
)

AC_PROG_INTLTOOL([0.23])

GETTEXT_PACKAGE=e16menuedit2
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Gettext package.])

dnl Add the languages which your application supports here.
ALL_LINGUAS="de en ko tr fr"
AM_GLIB_GNU_GETTEXT

AC_PATH_PROG(SCROLLKEEPER,scrollkeeper-config,no)
AM_CONDITIONAL(SK, "$SCROLLKEEPER" 2&>/dev/null != no)
AC_SUBST(SK)

AC_PATH_PROG(YELP,yelp,no)

dnl Set PACKAGE_LOCALE_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${ac_default_prefix}/${DATADIRNAME}/locale","")
else
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${prefix}/${DATADIRNAME}/locale","")
fi

dnl Set PACKAGE SOURCE DIR in config.h.
packagesrcdir=`cd $srcdir && pwd`

dnl Set PACKAGE PREFIX
if test "x${prefix}" = "xNONE"; then
  packageprefix=${ac_default_prefix}
else
  packageprefix=${prefix}
fi

dnl Set PACKAGE DATA & DOC DIR
systemdatadir=share
packagedatadir=share/${PACKAGE}-${VERSION}
packagedocdir=doc/${PACKAGE}-${VERSION}

dnl Set PACKAGE DIRS in config.h.
packagepixmapsdir=${systemdatadir}/pixmaps/${PACKAGE}-${VERSION}
systempixmapsdir=${systemdatadir}/pixmaps

dnl Set help dirs
yelphelpdir=${systemdatadir}/gnome/help/${PACKAGE}-${VERSION}

AC_SUBST(packagedatadir)
AC_SUBST(yelphelpdir)
AC_SUBST(packagedocdir)
AC_SUBST(packageprefix)

dnl Subst PACKAGE_DATA_DIR.
NO_PREFIX_PACKAGE_DATA_DIR="${packagedatadir}"
AC_SUBST(NO_PREFIX_PACKAGE_DATA_DIR)
PACKAGE_DATA_DIR="${packageprefix}/${packagedatadir}"
AC_SUBST(PACKAGE_DATA_DIR)

dnl Subst SYSTEM_DATA_DIR.
NO_PREFIX_SYSTEM_DATA_DIR="${systemdatadir}"
AC_SUBST(NO_PREFIX_SYSTEM_DATA_DIR)
SYSTEM_DATA_DIR="${packageprefix}/${systemdatadir}"
AC_SUBST(SYSTEM_DATA_DIR)

dnl Subst PACKAGE_DOC_DIR.
NO_PREFIX_PACKAGE_DOC_DIR="${packagedocdir}"
AC_SUBST(NO_PREFIX_PACKAGE_DOC_DIR)
PACKAGE_DOC_DIR="${packageprefix}/${packagedocdir}"
AC_SUBST(PACKAGE_DOC_DIR)

dnl Subst PACKAGE_PIXMAPS_DIR.
NO_PREFIX_PACKAGE_PIXMAPS_DIR="${packagepixmapsdir}"
AC_SUBST(NO_PREFIX_PACKAGE_PIXMAPS_DIR)
PACKAGE_PIXMAPS_DIR="${packageprefix}/${packagepixmapsdir}"
AC_SUBST(PACKAGE_PIXMAPS_DIR)

dnl Subst SYSTEM_PIXMAPS_DIR.
NO_PREFIX_SYSTEM_PIXMAPS_DIR="${systempixmapsdir}"
AC_SUBST(NO_PREFIX_SYSTEM_PIXMAPS_DIR)
SYSTEM_PIXMAPS_DIR="${packageprefix}/${systempixmapsdir}"
AC_SUBST(SYSTEM_PIXMAPS_DIR)

AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${packageprefix}/${packagedatadir}","")
AC_DEFINE_UNQUOTED(SYSTEM_DATA_DIR, "${packageprefix}/${systemdatadir}","")
AC_DEFINE_UNQUOTED(PACKAGE_DOC_DIR, "${packageprefix}/${packagedocdir}","")
AC_DEFINE_UNQUOTED(PACKAGE_PIXMAPS_DIR, "${packageprefix}/${packagepixmapsdir}","")
AC_DEFINE_UNQUOTED(SYSTEM_PIXMAPS_DIR, "${packageprefix}/${systempixmapsdir}","")
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}","")
AC_DEFINE_UNQUOTED(SCROLLKEEPER, "${SCROLLKEEPER}","")
AC_DEFINE_UNQUOTED(YELP, "${YELP}","")
AC_DEFINE_UNQUOTED(YELP_HELP_DIR, "${packageprefix}/${yelphelpdir}","")

AC_OUTPUT([
Makefile
e16menuedit2.spec
src/Makefile
pixmaps/Makefile
po/Makefile.in
help/Makefile
help/C/Makefile
help/C/figures/Makefile
])
