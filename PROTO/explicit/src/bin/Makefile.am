MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = explicitd explicit

explicit_SOURCES  = explicit_client.c
explicit_DEPENDENCIES = $(top_builddir)/src/lib/libexplicit.la
explicit_CFLAGS = @EXPLICIT_CFLAGS@ @EVAS_CFLAGS@ @EDJE_CFLAGS@
explicit_LDADD = @EXPLICIT_LIBS@ @EVAS_LIBS@ @EDJE_LIBS@

explicitd_SOURCES  = explicit_server.c
explicitd_DEPENDENCIES = $(top_builddir)/src/lib/libexplicit_server.la
explicitd_CFLAGS = @EXPLICIT_CFLAGS@ -I../lib
explicitd_LDADD = @EXPLICIT_LIBS@
