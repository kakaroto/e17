
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_builddir) \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
@EDJE_CFLAGS@ @EVAS_CFLAGS@

bin_PROGRAMS = edje_list

edje_list_SOURCES = \
main.c \
main.h

edje_list_LDADD = @EDJE_LIBS@ @EVAS_LIBS@
