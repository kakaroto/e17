## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I$(top_srcdir)/intl \
	@GTK_CFLAGS@ @evas_cflags@ @edb_cflags@ \
	-I/usr/local/include

bin_PROGRAMS = Imlib2Convert

Imlib2Convert_SOURCES = Imlib2Convert.c

INCLUDES = -I$(srcdir) -I../src -I.. -I. -I$(topdir)/src \
	@GTK_CFLAGS@ @evas_cflags@

Imlib2Convert_LDADD =  @evas_libs@ -lm @GTK_LIBS@ \
	@edb_libs@ -lgevas



