
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_builddir) \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
@EDJE_CFLAGS@ @EVAS_CFLAGS@

bin_PROGRAMS = edje_smart_thumb

edje_smart_thumb_SOURCES = \
main.c \
main.h

edje_smart_thumb_LDADD = @EDJE_LIBS@ @EVAS_LIBS@
