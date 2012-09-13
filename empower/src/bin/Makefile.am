## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir) \
              -DPACKAGE_BIN_DIR=\"$(bindir)\" \
              -DPACKAGE_DATA_DIR=\"$(pkgdatadir)\" \
              -Wall -Werror

bin_PROGRAMS = empower

empower_SOURCES = main.c gui.c pka.c marshal.c ckit.c logind.c

empower_CFLAGS = @EMPOWER_CFLAGS@

empower_LDADD =  $(LTLIBINTL) \
                 @EMPOWER_LIBS@

EXTRA_DIST = Empower.h gui.h pka.h marshal.h ckit.h logind.h

MAINTAINERCLEANFILES = Makefile.in config.h.in
