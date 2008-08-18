## Process this file with automake to produce Makefile.in

INCLUDES = \
-I$(top_srcdir)/lib \
@EDJE_CFLAGS@ @ECORE_CFLAGS@ @EVAS_CFLAGS@ \
@ESMART_CFLAGS@

bin_PROGRAMS = enscribe

enscribe_SOURCES = \
enscribe.h \
edje_callbacks.c \
window.c \
main.c

enscribe_LDADD = @EDJE_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ @ESMART_LIBS@
