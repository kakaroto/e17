NTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -DPKGDATADIR=\"$(pkgdatadir)\" \
		-I ../			\
		@PTHREAD_CFLAGS@	\
		@LIBXML_CFLAGS@		\
		@JSON_CFLAGS@		\
		@GLIB_CFLAGS@		\
		@SQLITE_CFLAGS@		\
		`curl-config --cflags`	\
		@ELEMENTARY_CFLAGS@	\
		-DLOCALEDIR=\"$(localedir)\" \
		-Wall


AM_LDFLAGS =	@PTHREAD_LIBS@		\
		@LIBXML_LIBS@		\
		@JSON_LIBS@		\
		@GLIB_LIBS@		\
		@SQLITE_LIBS@		\
		`curl-config --libs`	\
		@ELEMENTARY_LIBS@


bin_PROGRAMS = elmdentica

elmdentica_SOURCES =	\
	curl.c		\
	twitter.c	\
	settings.c	\
	elmdentica.c

EXTRA_DIST=		\
	curl.h		\
	gettext.h	\
	twitter.h	\
	settings.h	\
	elmdentica.h
