AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

AM_LDFLAGS =    @ELEMENTARY_LIBS@
AM_CFLAGS =	@ELEMENTARY_CFLAGS@ -Wall	\
		-DPACKAGE_MODULE_DIR=\"$(libdir)/assurances/\" \
		-DLIBENSURE_DIR=\"$(libdir)/\" \
	       	-rdynamic

bin_PROGRAMS = ensure

ensure_SOURCES = \
	display.c  	\
	enconfig.c	\
	enedj.c  	\
	ensure.c     	\
	parser.c	\
	results.c	\
	enasn.c    	\
	errors.c	\
	enobj.c 	\
	entree.c	\
	hidden.c	\
	display.h  	\
	enconfig.h	\
	enedj.h  	\
	ensure.h	\
	entree.h	\
	enasn.h    	\
	enobj.h  	\
	parser.h	\
	results.h	\
	hidden.h	\
	errors.h


lib_LTLIBRARIES	= libensure.la
libdir = $(prefix)/lib/ensure/

libensure_la_SOURCES =		\
	libensure.c

libensure_la_LDFLAGS      = -module -avoid-version -rdynamic
libensure_la_DEPENDENCIES = $(top_builddir)/config.h
libensure_la_LIBADD       = @ELEMENTARY_LIBS@


