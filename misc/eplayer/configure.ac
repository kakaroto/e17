AC_INIT(src/eplayer.c)

AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE(eplayer, 0.7)

AC_PROG_CC
AC_ISC_POSIX
AC_HEADER_STDC

AC_SUBST(CFLAGS)
AC_SUBST(LDFLAGS)

## The Simplistic Way
AC_CHECK_LIB(ao,ao_driver_id)
AC_CHECK_LIB(vorbis,vorbis_info_init)
AC_CHECK_LIB(vorbisfile,ov_open)
#AC_CHECK_LIB(ecore,ecore_init)
AC_CHECK_LIB(ecore_evas,ecore_evas_init)

## The more complete way
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
m4/Makefile
src/Makefile
])
