# $Id$

AC_INIT(src/eplayer.c)

AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE(eplayer, 0.7)

AC_PROG_CC
AC_ISC_POSIX
AC_PROG_INSTALL
AC_HEADER_STDC
AC_C_BIGENDIAN
AM_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_EXPAND_DIR(DATA_DIR, "$datadir/eplayer")
AC_DEFINE_UNQUOTED(DATA_DIR, "$DATA_DIR", [Directory to install data files in])

AC_EXPAND_DIR(PLUGIN_DIR, "$libdir/eplayer")
AC_DEFINE_UNQUOTED(PLUGIN_DIR, "$PLUGIN_DIR", [Directory to install plugins in])
AC_SUBST(PLUGIN_DIR)

AC_ARG_ENABLE(debug,
	[  --enable-debug          debug build (default: no)], [
		AC_DEFINE([DEBUG], [], [Define to 1 if you want debugging output])
		CFLAGS=`echo "$CFLAGS -g -Wall" | sed s/" -s"//`
	])

dnl Check for libdl
tmp=$LIBS
AC_CHECK_HEADERS(dlfcn.h, , AC_MSG_ERROR(Cannot find dlfcn.h))
AC_CHECK_LIB(dl, dlsym, DL_LIBS="-ldl", AC_MSG_ERROR(Cannot find libdl))
AC_SUBST(DL_LIBS)
LIBS=$tmp

dnl I'd like to use XIPH_PATH_VORBIS here, but it seems to be quite
dnl unstable and there's no pkgconfig file either :/
tmp=$LIBS

AC_CHECK_LIB(vorbis, vorbis_info_init, ,
	AC_MSG_ERROR(Cannot find libvorbis))

AC_CHECK_LIB(vorbisfile, ov_open, ,
	AC_MSG_ERROR(Cannot find libvorbisfile))

VORBIS_LIBS="-lvorbis -lvorbisfile -logg"
AC_SUBST(VORBIS_LIBS)
LIBS=$tmp

AC_PATH_GENERIC(ecore, , ,
	AC_MSG_ERROR(Cannot find ecore: Is ecore-config in path?))

AC_PATH_GENERIC(evas, , ,
	AC_MSG_ERROR(Cannot find evas: Is evas-config in path?))

AC_PATH_GENERIC(edje, , ,
	AC_MSG_ERROR(Cannot find edje: Is edje-config in path?))

AC_PATH_GENERIC(esmart, , ,
	AC_MSG_ERROR(Cannot find esmart: Is esmart-config in path?))

dnl Check for OSS
AC_CHECK_HEADERS(sys/soundcard.h machine/soundcard.h)

if test "${ac_cv_header_sys_soundcard_h}" = "yes" || test "${ac_cv_header_machine_soundcard_h}" = "yes"; then
	have_oss="yes"
else
	have_oss="no"
fi

AM_CONDITIONAL(HAVE_OSS, test "$have_oss" = "yes")

AC_OUTPUT([
Makefile
data/Makefile
data/fonts/Makefile
data/images/Makefile
m4/Makefile
src/Makefile
src/output/Makefile
src/output/oss/Makefile
])

echo
echo "$PACKAGE $VERSION"
echo
echo "Output Plugins"
echo
echo "OSS...........: ${ac_cv_header_sys_soundcard_h}"
echo
