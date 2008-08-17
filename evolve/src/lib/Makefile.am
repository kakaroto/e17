## Process this file with automake to produce Makefile.in

.o.so:  
	$(CC) $(INCLUDES) $(LDFLAGS) $< -o $@

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
	@EVAS_CFLAGS@ \
	@ETK_CFLAGS@ \
	@EET_CFLAGS@

CLEANFILES = \
	libevolve_la-evolve.h \
	libevolve_la-evolve.c \
	evolve.c

lib_LTLIBRARIES = libevolve.la

EVOLVEHEADERS = \
	Evolve.h \
	evolve_constructor.h \
	evolve_parse.h \
	evolve_property.h \
	evolve_private.h \
	evolve_widget.h \
	evolve_signal.h \
	evolve_gui.h \
	evolve_util.h
	

EVOLVEFILES = \
	evolve.y \
	evolve.l \
	evolve_lib.c \
	evolve_constructor.c \
	evolve_parse.c \
	evolve_property.c \
	evolve_widget.c \
	evolve_signal.c \
	evolve_gui.c \
	evolve_util.c \
	$(EVOLVEHEADERS)

libevolve_la_SOURCES = $(EVOLVEFILES)

installed_headersdir = $(prefix)/include/evolve
installed_headers_DATA = $(EVOLVEHEADERS)

libevolve_la_YFLAGS		= -d
libevolve_la_LIBADD		= @EVAS_LIBS@ @ETK_LIBS@ @EET_LIBS@ @LEXLIB@ -lm
libevolve_la_DEPENDENCIES	= $(top_builddir)/src/config.h
libevolve_la_LDFLAGS		= -version-info 1:0:1
libevolve_la_CFLAGS		= -Wall
