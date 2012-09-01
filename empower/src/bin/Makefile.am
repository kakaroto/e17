## Process this file with automake to produce Makefile.in

INCLUDES = -I$(top_srcdir)

bin_PROGRAMS = empower

empower_SOURCES = main.c gui.c pka.c marshal.c

empower_CFLAGS = @ELEMENTARY_CFLAGS@ @ECORE_CFLAGS@ \
                 @EDJE_CFLAGS@ @EVAS_CFLAGS@ @EDBUS_CFLAGS@

empower_LDADD =  $(INTLLIBS) \
                 @ELEMENTARY_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ \
                 @EDJE_LIBS@ @EDBUS_CFLAGS@

EXTRA_DIST = Empower.h gui.h cb.h pka.h marshal.h

MAINTAINERCLEANFILES = Makefile.in config.h.in
