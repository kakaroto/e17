AC_INIT(src/eplayer.c)

AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE(eplayer, 0.7)

AC_PROG_CC
AC_ISC_POSIX
AC_PROG_INSTALL
AC_HEADER_STDC
AC_C_BIGENDIAN

AC_EXPAND_DIR(DATA_DIR, "$datadir/eplayer")
AC_DEFINE_UNQUOTED(DATA_DIR, "$DATA_DIR", [Directory to install data files in])

AC_ARG_ENABLE(debug,
	[  --enable-debug          debug build (default: no)], [
		AC_DEFINE([DEBUG], [], [Define to 1 if you want debugging output])
		CFLAGS=`echo "$CFLAGS -g -Wall" | sed s/" -s"//`
	])

dnl I'd like to use XIPH_PATH_VORBIS here, but it seems to be quite
dnl unstable and there's no pkgconfig file either :/
AC_CHECK_LIB(vorbis, vorbis_info_init, ,
	AC_MSG_ERROR(Cannot find libvorbis))

AC_CHECK_LIB(vorbisfile, ov_open, ,
	AC_MSG_ERROR(Cannot find libvorbisfile))

XIPH_PATH_AO(, AC_MSG_ERROR(Cannot find libao))

AC_PATH_GENERIC(ecore, , ,
	AC_MSG_ERROR(Cannot find ecore: Is ecore-config in path?))

AC_PATH_GENERIC(evas, , ,
	AC_MSG_ERROR(Cannot find evas: Is evas-config in path?))

AC_PATH_GENERIC(edje, , ,
	AC_MSG_ERROR(Cannot find edje: Is edje-config in path?))

AC_PATH_GENERIC(esmart, , ,
	AC_MSG_ERROR(Cannot find esmart: Is esmart-config in path?))

AC_OUTPUT([
Makefile
data/Makefile
data/fonts/Makefile
data/images/Makefile
m4/Makefile
src/Makefile
])
