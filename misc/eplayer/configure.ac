AC_INIT(src/eplayer.c)

AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE(eplayer, 0.7)

AC_PROG_CC
AC_ISC_POSIX
AC_HEADER_STDC
AC_ARG_PROGRAM
AM_PROG_LIBTOOL

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

## The Simplistic Way
AC_CHECK_LIB(ao,ao_driver_id)
AC_CHECK_LIB(vorbis,vorbis_info_init)
AC_CHECK_LIB(vorbisfile,ov_open)
#AC_CHECK_LIB(ecore,ecore_init)
AC_CHECK_LIB(ecore_evas,ecore_evas_init)

## The more complete way

PROG="ecore-config";
AC_PATH_PROG(ECORE_CONFIG, $PROG, "", $PATH)
if [ test -z $ECORE_CONFIG ]; then
  echo $PROG " is not in your \$PATH. Please ensure it is.";
  echo "Read the manual page for you shell as to how to extend your path.";
  echo "FATAL ERROR. ABORT.";
  exit -1;
fi
ecore_cflags=`$ECORE_CONFIG --cflags`
ecore_libs=`$ECORE_CONFIG --libs`
AC_SUBST(ecore_cflags)
AC_SUBST(ecore_libs)


PROG="evas-config";
AC_PATH_PROG(EVAS_CONFIG, $PROG, "", $PATH)
if [ test -z $EVAS_CONFIG ]; then
  echo $PROG " is not in your \$PATH. Please ensure it is.";
  echo "Read the manual page for you shell as to how to extend your path.";
  echo "FATAL ERROR. ABORT.";
  exit -1;
fi
evas_cflags=`$EVAS_CONFIG --cflags`
evas_libs=`$EVAS_CONFIG --libs`
AC_SUBST(evas_cflags)
AC_SUBST(evas_libs)

PROG="edje-config";
AC_PATH_PROG(EDJE_CONFIG, $PROG, "", $PATH)
if [ test -z $EVAS_CONFIG ]; then
  echo $PROG " is not in your \$PATH. Please ensure it is.";
  ebscho "Read the manual page for you shele as to how to extend your path.";
  echo "FATAL ERROR. ABORT.";
  exit -1;
fi
edje_cflags=`$EDJE_CONFIG --cflags`
edje_libs=`$EDJE_CONFIG --libs`
AC_SUBST(edje_cflags)
AC_SUBST(edje_libs)


AC_OUTPUT([
Makefile
src/Makefile
])
