dnl $Id$

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

AC_EXPAND_DIR(SYSCONF_DIR, "$sysconfdir")
AC_DEFINE_UNQUOTED(SYSCONF_DIR, "$SYSCONF_DIR", [Directory to install configuration in])
AC_SUBST(SYSCONF_DIR)

AC_ARG_ENABLE(debug,
	[  --enable-debug          debug build (default: no)], [
		AC_DEFINE([DEBUG], [], [Define to 1 if you want debugging output])
		CFLAGS=`echo "$CFLAGS -g -Wall" | sed s/" -s"//`
	])

AC_PATH_GENERIC(ecore, , ,
	AC_MSG_ERROR(Cannot find ecore: Is ecore-config in path?))

AC_PATH_GENERIC(evas, , ,
	AC_MSG_ERROR(Cannot find evas: Is evas-config in path?))

AC_PATH_GENERIC(edje, , ,
	AC_MSG_ERROR(Cannot find edje: Is edje-config in path?))

AC_PATH_GENERIC(esmart, , ,
	AC_MSG_ERROR(Cannot find esmart: Is esmart-config in path?))

dnl Check for libdl
tmp=$LIBS
AC_CHECK_HEADERS(dlfcn.h, , AC_MSG_ERROR(Cannot find dlfcn.h))
AC_CHECK_LIB(dl, dlsym, DL_LIBS="-ldl", AC_MSG_ERROR(Cannot find libdl))
AC_SUBST(DL_LIBS)
LIBS=$tmp

dnl I'd like to use XIPH_PATH_VORBIS here, but it seems to be quite
dnl unstable and there's no pkgconfig file either :/
tmp=$LIBS

AC_CHECK_LIB(vorbis, vorbis_info_init,
	[have_vorbis="yes"; LIBS="$LIBS -lvorbis"], have_vorbis="no")

if test "x$have_vorbis" = "xyes"; then
	AC_CHECK_LIB(vorbisfile, ov_open,
		[
			VORBIS_LIBS="-lvorbis -lvorbisfile -logg"
			AC_SUBST(VORBIS_LIBS)
		], have_vorbis="no")
fi

LIBS=$tmp
AM_CONDITIONAL(HAVE_VORBIS, test "$have_vorbis" = "yes")

dnl Check for libavcodec
tmp=$LIBS

AC_CHECK_LIB(avcodec, avcodec_init,
	[
		have_avcodec="yes"
		
		AVCODEC_CFLAGS="-I/usr/include/ffmpeg"
		AVCODEC_LIBS="-lavcodec"
		
		AC_SUBST(AVCODEC_CFLAGS)
		AC_SUBST(AVCODEC_LIBS)
	], have_avcodec="no")

LIBS=$tmp
AM_CONDITIONAL(HAVE_AVCODEC, test "$have_avcodec" = "yes")

dnl Check for OSS
AC_CHECK_HEADERS(sys/soundcard.h machine/soundcard.h, [have_oss="yes"; break], have_oss="no")
AM_CONDITIONAL(HAVE_OSS, test "$have_oss" = "yes")

dnl Check for ALSA
PKG_CHECK_MODULES(ALSA, alsa >= 0.9, have_alsa="yes", have_alsa="no")
AM_CONDITIONAL(HAVE_ALSA, test "x$have_alsa" = "xyes")

dnl Check for Solaris audio stuff
AC_CHECK_HEADER(sys/audioio.h, have_solaris="yes", have_solaris="no")
AM_CONDITIONAL(HAVE_SOLARIS, test "$have_solaris" = "yes")

AC_OUTPUT([
Makefile
data/Makefile
data/fonts/Makefile
data/images/Makefile
etc/Makefile
m4/Makefile
src/Makefile
src/input/Makefile
src/input/avcodec/Makefile
src/input/vorbis/Makefile
src/output/Makefile
src/output/alsa/Makefile
src/output/oss/Makefile
src/output/solaris/Makefile
])

echo
echo "$PACKAGE $VERSION"
echo
echo
echo "Input Plugins"
echo
echo "AVCodec.......: $have_avcodec"
echo "Ogg Vorbis....: $have_vorbis"
echo
echo
echo "Output Plugins"
echo
echo "OSS...........: $have_oss"
echo "ALSA..........: $have_alsa"
echo "Solaris.......: $have_solaris"
echo
