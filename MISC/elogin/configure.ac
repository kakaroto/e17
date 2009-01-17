dnl Process this file with autoconf to produce a configure script.

AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(elogin, 0.0.2)
AM_CONFIG_HEADER(src/config.h)

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC

dnl Set PACKAGE_DATA_DIR in config.h.
if test "x${datadir}" = 'x${prefix}/share'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}", [PACKAGE_DATA_DIR])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}", [PACKAGE_DATA_DIR])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${datadir}/${PACKAGE}", [PACKAGE_DATA_DIR])
fi

if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PREFIX, "/usr/local/", [PREFIX])
else
    AC_DEFINE_UNQUOTED(PREFIX, "${prefix}", [PREFIX])
fi

dnl Set PACKAGE_SOURCE_DIR in config.h.
packagesrcdir=`cd $srcdir && pwd`
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}", [PACKAGE_SOURCE_DIR])

dnl Use -Wall if we have gcc.
changequote(,)dnl
if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;;
  esac
fi
changequote([,])dnl

withval=""
AC_ARG_WITH(pam-prefix,
[      --with-pam-prefix=<prefix>   specify where pam files go],[
if test x$withval != x; then
   AC_MSG_RESULT("PAM files will be installed in prefix ${withval}.")
fi])
if test x$withval != x; then
	PAM_PREFIX_UNEXPANDED="$withval"
else
	PAM_PREFIX_UNEXPANDED="$sysconfdir"
fi
PAM_PREFIX=`eval echo $PAM_PREFIX_UNEXPANDED`
AC_SUBST(PAM_PREFIX)

AC_ARG_WITH(edb,
	[ --with-edb=DIR		use edb in <DIR>],
	[ CFLAGS="$CFLAGS -I$withval/include"
	LIBS="-L$withval/lib $LIBS"])
AC_ARG_WITH(ebg,
	[ --with-ebg=DIR		use ebg in <DIR>],
	[ CFLAGS="$CFLAGS -I$withval/include"
	LIBS="-L$withval/lib $LIBS"])
AC_ARG_WITH(evas,
	[ --with-evas=DIR		use evas in <DIR>],
	[ CFLAGS="$CFLAGS -I$withval/include"
	LIBS="-L$withval/lib $LIBS"])
AC_ARG_WITH(ecore,
	[ --with-ecore=DIR		use ecore in <DIR>],
	[ CFLAGS="$CFLAGS -I$withval/include"
	LIBS="-L$withval/lib $LIBS"])

AC_PATH_GENERIC(edb, 1.0.3, [
	AC_SUBST(edb_libs)
	AC_SUBST(edb_cflags) ],
	AC_MSG_ERROR(Cannot find edb: Is edb-config in path?))
edb_libs=`edb-config --libs`
edb_cflags=`edb-config --cflags`
AC_SUBST(edb_libs)
AC_SUBST(edb_cflags)

AC_PATH_GENERIC(ecore, 0.0.2, [
	AC_SUBST(ecore_libs)
	AC_SUBST(ecore_cflags) ],
	AC_MSG_ERROR(Cannot find ecore: Is ecore-config in path?))
ecore_libs=`ecore-config --libs`
ecore_cflags=`ecore-config --cflags`
AC_SUBST(ecore_libs)
AC_SUBST(ecore_cflags)

AC_PATH_GENERIC(evas, 1.0.0, [
	AC_SUBST(evas_libs)
	AC_SUBST(evas_cflags) ],
	AC_MSG_ERROR(Cannot find evas: Is evas-config in path?))
evas_libs=`evas-config --libs`
evas_cflags=`evas-config --cflags`
AC_SUBST(evas_libs)
AC_SUBST(evas_cflags)

AC_PATH_GENERIC(ebg, 1.0.0, [
	AC_SUBST(ebg_libs)
	AC_SUBST(ebg_cflags) ],
	AC_MSG_ERROR(Cannot find ebg: Is ebg-config in path?))
ebg_libs=`ebg-config --libs`
ebg_cflags=`ebg-config --cflags`
AC_SUBST(ebg_libs)
AC_SUBST(ebg_cflags)

AC_PATH_GENERIC(estyle, 0.0.1, [
	AC_SUBST(estyle_libs)
	AC_SUBST(estyle_cflags) ],
	AC_MSG_ERROR(Cannot find estyle: Is estyle-config in path?))
estyle_libs=`estyle-config --libs`
estyle_cflags=`estyle-config --cflags`
AC_SUBST(estyle_libs)
AC_SUBST(estyle_cflags)

AC_PATH_GENERIC(ewd, 0.0.1, [
	AC_SUBST(ewd_libs)
	AC_SUBST(ewd_cflags) ],
	AC_MSG_ERROR(Cannot find ewd: Is ewd-config in path?))
ewd_libs=`ewd-config --libs`
ewd_cflags=`ewd-config --cflags`
AC_SUBST(ewd_libs)
AC_SUBST(ewd_cflags)

AC_CHECK_HEADERS(security/pam_appl.h, [
    LIBS="$LIBS -lpam"
    VRFY="verify-pam"
    AC_DEFINE(HAVE_PAM, 1, [pam support enabled])])

#AC_CONFIG_FILES([src/daemon/elogin_wrapper:src/daemon/elogin_wrapper.in],
#				[chmod a+x src/daemon/elogin_wrapper])

AC_OUTPUT([
Makefile
src/Makefile
src/client/Makefile
src/daemon/Makefile
src/daemon/elogin_wrapper
data/init.d/elogin
])

