AC_INIT(trivial-sprite/gevasanim.c)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(gevas-examples, 0.0.1)

AM_PATH_GTK(1.2.0, , AC_MSG_ERROR(Cannot find GTK: Is gtk-config in path?))


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

dnl Set PACKAGE_DATA_DIR in config.h.
if test "x${datadir}" = 'x${prefix}/share'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}")
  else
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}")
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${datadir}/${PACKAGE}")
fi

evas_cflags=`evas-config --cflags`
evas_libs=`evas-config --libs`
edb_cflags=`edb-config --cflags`
edb_libs=`edb-config --libs`
gtk_cflags=`gtk-config --cflags`


AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(evas_cflags)
AC_SUBST(evas_libs)
AC_SUBST(edb_cflags)
AC_SUBST(edb_libs)
AC_SUBST(gtk_cflags)

AC_OUTPUT([Makefile trivial-sprite/Makefile trivial-sprite-edb/Makefile])
