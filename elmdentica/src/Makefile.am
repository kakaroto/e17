NTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -DPKGDATADIR=\"$(pkgdatadir)\" \
		-I ../			\
		@PTHREAD_CFLAGS@	\
		@GLIB_CFLAGS@		\
		@SQLITE_CFLAGS@		\
		`curl-config --cflags`	\
		@ELEMENTARY_CFLAGS@	\
		-DLOCALEDIR=\"$(localedir)\" \
		-Wall


AM_LDFLAGS =	@PTHREAD_LIBS@		\
		@GLIB_LIBS@		\
		@SQLITE_LIBS@		\
		`curl-config --libs`	\
		@ELEMENTARY_LIBS@


bin_PROGRAMS = elmdentica

elmdentica_SOURCES =	\
	cJSON.c		\
	curl.c		\
	twitter.c	\
	statusnet.c	\
	settings.c	\
	elmdentica.c

EXTRA_DIST=		\
	cJSON.h		\
	curl.h		\
	gettext.h	\
	twitter.h	\
	statusnet.h	\
	settings.h	\
	elmdentica.h
