SPEC_FILE = esound.spec

AUDIOFILE_LIBS = @AUDIOFILE_LIBS@
LIBESDDSP = @LIBESDDSP@

INCLUDES = @AUDIOFILE_CFLAGS@ -DSYSCONFDIR="\"$(sysconfdir)\""

bin_SCRIPTS = \
	@ESDDSP@	\
	esd-config

EXTRA_SCRIPTS = esddsp

EXTRA_DIST = \
        esd.conf        \
	esd.m4 		\
	TIPS 		\
	test-script 	\
	test.wav 	\
	getopt.h 	\
	TODO

lib_LTLIBRARIES = libesd.la @LIBESDDSP@

libesd_la_SOURCES = \
	esdlib.c	\
	esdmgr.c	\
	esdfile.c	\
	esd_config.c	\
	audio.c		\
	genrand.c	\
	genrand.h

libesd_la_LDFLAGS = -version-info @ESD_VERSION_INFO@

libesd_la_LIBADD = @AUDIOFILE_LIBS@ -lm

EXTRA_LTLIBRARIES = \
	libesddsp.la

libesddsp_la_LDFLAGS = -rpath $(libdir) -version-info @ESD_VERSION_INFO@

libesddsp_la_LIBADD = @DL_LIB@ -lm

libesddsp_la_SOURCES = \
	esddsp.c

EXTRA_libesd_la_SOURCES = \
	audio_aix.c	\
	audio_alsa.c	\
	audio_hpux.c	\
	audio_irix.c	\
	audio_none.c	\
	audio_oss.c	\
	audio_mklinux.c	\
	audio_solaris.c	

include_HEADERS = \
	esd.h

bin_PROGRAMS = \
	esd		\
	esdcat		\
	esdctl		\
	esdfilt		\
	esdmon		\
	esdrec		\
	esdsample	\
	esdloop		\
	@ESDPLAY@

EXTRA_PROGRAMS = \
	esdplay

esd_SOURCES = 	\
	esd.c 		\
	clients.c	\
	filter.c	\
	mix.c		\
	players.c	\
	proto.c		\
	samples.c	\
	esd-server.h

esdcat_SOURCES = \
	esdcat.c

esdctl_SOURCES = \
	esdctl.c

esdfilt_SOURCES = \
	esdfilt.c

esdmon_SOURCES = \
	esdmon.c

esdrec_SOURCES = \
	esdrec.c

esdsample_SOURCES = \
	esdsample.c

esdloop_SOURCES = \
	esdloop.c

EXTRA_esdplay_SOURCES = \
	esdplay.c

LDADD = $(AUDIOFILE_LIBS) libesd.la -lm

LIBOBJS = @LIBOBJS@

esd_LDADD = $(LIBOBJS) $(WRAP_LIB) $(LDADD)
esdctl_LDADD = $(LIBOBJS) $(LDADD)
esdplay_LDADD = $(LIBOBJS) $(LDADD)

m4datadir = $(datadir)/aclocal
m4data_DATA = esd.m4

esdconfdir = $(sysconfdir)
esdconf_DATA = esd.conf

$(PACKAGE)-$(VERSION).tar.gz: dist

rpm: $(PACKAGE)-$(VERSION).tar.gz
	cp $(PACKAGE)-$(VERSION).tar.gz /usr/src/redhat/SOURCES
	rpm -ba $(SPEC_FILE)

dist-hook: esound.spec
	cp $(SPEC_FILE) $(distdir)
	mkdir $(distdir)/docs
	cp $(srcdir)/docs/esound.sgml $(distdir)/docs

CLEANFILES=esound.spec
