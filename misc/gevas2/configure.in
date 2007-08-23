AC_INIT(src/gevas.h)
AM_CONFIG_HEADER(config.h)
VERSION=2.2.1
AM_INIT_AUTOMAKE(gevas, $VERSION )

AM_PATH_GTK_2_0( 2.2.0 )

AC_PROG_CC
AM_PROG_CC_STDC
AC_C_BIGENDIAN
AM_WITH_DMALLOC
AC_C_CONST
AM_ENABLE_SHARED
AM_PROG_LIBTOOL
if test X"$enable_shared" = Xyes; then
  DLLDFLAGS=-export-dynamic
  AC_SUBST(DLLDFLAGS)
fi
AC_PROG_INSTALL
AC_PROG_MAKE_SET


#
# Should we compile edb helper code aswell?
#
edb_cflags=""
edb_libs=""
AC_MSG_CHECKING(whether to enable edb code)
AC_ARG_ENABLE(edb-code, 
  [  --enable-edb-code       enable edb related code], [
      AC_MSG_RESULT(yes)
      AC_DEFINE(BUILD_EDB_CODE, 1, [ Support edb ])
      edb_cflags=`edb-config --cflags`
      edb_libs=`edb-config --libs`" -lz"
  ], AC_MSG_RESULT(no)
)
AC_SUBST(edb_cflags)
AC_SUBST(edb_libs)


have_edje=no
PKG_CHECK_MODULES(EDJE, edje >= 0.4.9 ecore >= 0.9.9, 
[
	have_edje=yes
],
[
	AC_MSG_ERROR([Fatal Error: please install latest edje package.])	
])
AM_CONDITIONAL(HAVE_EDJE, test "$have_edje" = yes)




have_evas=no
PKG_CHECK_MODULES(EVAS, evas >= 0.9.9,
[
	have_evas=yes
],
[
	AC_MSG_ERROR([Fatal Error: please install latest evas package.])
])
AM_CONDITIONAL(HAVE_EVAS, test "$have_evas" = yes)
evas_cflags=$EVAS_CFLAGS
evas_libs=$EVAS_LIBS


dnl Set PACKAGE_DATA_DIR in config.h.
if test "x${datarootdir}" = 'x${prefix}/share'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}",[package data dir])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}",[package data dir])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${datarootdir}/${PACKAGE}",[package data dir])
fi

ICON_INSTALL_DIR="/usr/local/gevas2/icons"
AC_SUBST(ICON_INSTALL_DIR)
AC_DEFINE_UNQUOTED(ICON_INSTALL_DIR, "$ICON_INSTALL_DIR", 1, [ Where to install icons ])

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(evas_cflags)
AC_SUBST(evas_libs)

REQUIREMENTS="edje >= 0.4.9 ecore >= 0.9.9 "
AC_SUBST(REQUIREMENTS)

AC_OUTPUT([
Makefile
src/Makefile
media/Makefile
demo/Makefile
gevas.pc
])
