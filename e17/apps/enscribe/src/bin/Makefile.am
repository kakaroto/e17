## Process this file with automake to produce Makefile.in

INCLUDES = \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@edje_cflags@ @ecore_cflags@ @evas_cflags@ \
@esmart_cflags@

bin_PROGRAMS = enscribe

enscribe_SOURCES = \
enscribe.h \
edje_callbacks.c \
window.c \
main.c

enscribe_LDADD =  @edje_libs@ @ecore_libs@ @evas_libs@ \
@esmart_libs@  -lesmart_container -lesmart_textarea
	      
