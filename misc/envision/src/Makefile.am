# $Id$

AM_CFLAGS = @ECORE_CFLAGS@ @EVAS_CFLAGS@ @EDJE_CFLAGS@ \
	@EMOTION_CFLAGS@

bin_PROGRAMS = envision

envision_SOURCES = \
	envision.c      envision.h \
	callbacks.c     envision_config.h \
	envision_config.c

envision_LDADD = @ECORE_LIBS@ @EVAS_LIBS@ @EDJE_LIBS@ \
	@EMOTION_LIBS@
