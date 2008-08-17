dnl Process this file with autoconf to produce a configure script.
AC_INIT(configure.in)

dnl Checks for programs.
dnl AC_PROG_CPP
dnl AC_PROG_CXX (when using cpp)
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC

MED_VERSION=0.01
VERSION=$MED_VERSION
PARENT_PACKAGE=enlightenment

AM_INIT_AUTOMAKE(med, $VERSION)
AM_CONFIG_HEADER(config.h)

dnl Set PACKAGE_DATA_DIR in config.h.
if test "x${datadir}" = 'x${prefix}/share'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PARENT_PACKAGE}")
  else
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PARENT_PACKAGE}")
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${datadir}/${PARENT_PACKAGE}")
fi


dnl Checks for libraries.
dnl Replace `main' with a function in -lImlib2: (evas already requires this)
dnl AC_CHECK_LIB(Imlib2, main)
dnl Replace `main' with a function in -lXext:
dnl AC_CHECK_LIB(Xext, main)

dnl Check for Ebits.
AC_ARG_WITH(ebits,
        [  --with-ebits=DIR          use ebits in <DIR>],
        [CFLAGS="$CFLAGS -I$withval/include"
         LIBS="-L$withval/lib $LIBS"])

AC_PATH_GENERIC(ebits,, [
    AC_SUBST(ebits_libs)
    AC_SUBST(ebits_cflags)],
    AC_MSG_ERROR(Cannot find ebits: Is ebits-config in path?))
ebits_libs=`ebits-config --libs`
ebits_cflags=`ebits-config --cflags`

dnl Check for Ecore.
AC_ARG_WITH(ecore,
        [  --with-ecore=DIR          use ecore in <DIR>],
        [CFLAGS="$CFLAGS -I$withval/include"
         LIBS="-L$withval/lib $LIBS"])

AC_PATH_GENERIC(ecore,, [
    AC_SUBST(ecore_libs)
    AC_SUBST(ecore_cflags)],
    AC_MSG_ERROR(Cannot find ecore: Is ecore-config in path?))
ecore_libs=`ecore-config --libs`
ecore_cflags=`ecore-config --cflags`

dnl Check for Edb.
AC_ARG_WITH(edb,
        [  --with-edb=DIR          use edb in <DIR>],
        [CFLAGS="$CFLAGS -I$withval/include"
         LIBS="-L$withval/lib $LIBS"])

AC_PATH_GENERIC(edb,, [
    AC_SUBST(edb_libs)
    AC_SUBST(edb_cflags)],
    AC_MSG_ERROR(Cannot find edb: Is edb-config in path?))
edb_libs=`edb-config --libs`
edb_cflags=`edb-config --cflags`

dnl Check for Evas.
AC_ARG_WITH(evas,
        [  --with-evas=DIR          use evas in <DIR>],
        [CFLAGS="$CFLAGS -I$withval/include"
         LIBS="-L$withval/lib $LIBS"])

AC_PATH_GENERIC(evas,, [
    AC_SUBST(evas_libs)
    AC_SUBST(evas_cflags)],
    AC_MSG_ERROR(Cannot find evas: Is evas-config in path?))
evas_libs=`evas-config --libs`
evas_cflags=`evas-config --cflags`


dnl Checks for header files.
AC_PATH_X
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_UID_T
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T

dnl Checks for library functions.
AC_FUNC_FNMATCH
AC_CHECK_FUNCS(mkdir putenv strdup)

AC_OUTPUT(Makefile src/Makefile)
