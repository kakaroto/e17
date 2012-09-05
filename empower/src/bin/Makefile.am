## Process this file with automake to produce Makefile.in

INCLUDES = -I$(top_srcdir) \
           -DPACKAGE_BIN_DIR=\"$(bindir)\" \
           -DPACKAGE_DATA_DIR=\"$(pkgdatadir)\"

bin_PROGRAMS = empower

empower_SOURCES = main.c gui.c pka.c marshal.c

empower_CFLAGS = @EMPOWER_CFLAGS@

empower_LDADD =  $(INTLLIBS) \
                 @EMPOWER_LIBS@

EXTRA_DIST = Empower.h gui.h cb.h pka.h marshal.h

MAINTAINERCLEANFILES = Makefile.in config.h.in
