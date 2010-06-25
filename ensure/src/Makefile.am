AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

AM_LDFLAGS =    @ELEMENTARY_LIBS@
AM_CFLAGS =	@ELEMENTARY_CFLAGS@ -Wall	\
		-DPACKAGE_MODULE_DIR=\"$(libdir)/ensure/assurances/\" -rdynamic

bin_PROGRAMS = ensure

ensure_SOURCES = \
	display.c  	\
	enedj.c  	\
	ensure.c     	\
	parser.c	\
	enasn.c    	\
	enobj.c 	\
	display.h  	\
	enedj.h  	\
	ensure.h	\
	enasn.h    	\
	enobj.h  	\
	parser.h


lib_LTLIBRARIES	= libensure.la

libensure_la_SOURCES =		\
	libensure.c

libensure_la_LDFLAGS      = -module -avoid-version -rdynamic
libensure_la_DEPENDENCIES = $(top_builddir)/config.h
libensure_la_LIBADD       = @ELEMENTARY_LIBS@


