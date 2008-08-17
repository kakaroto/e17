MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I. \
	 -I$(top_srcdir) \
	 @x_cflags@ @IMLIB_CFLAGS@ @ECORE_CFLAGS@ @EVAS_CFLAGS@ @EDJE_CFLAGS@

bin_PROGRAMS = emprint

emprint_SOURCES = emprint.h main.c

emprint_LDADD = @x_libs@ @IMLIB_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ @EDJE_LIBS@


