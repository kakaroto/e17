## Process this file with automake to produce Makefile.in

INCLUDES = @edje_cflags@ @ecore_cflags@ @edb_cflags@ @evas_cflags@ \
@esmart_cflags@

bin_PROGRAMS = elicit 

elicit_SOURCES = \
        elicit.c Elicit.h \
        callbacks.c callbacks.h \
        util.c util.h 

elicit_LDADD = @edje_libs@ @ecore_libs@ @edb_libs@ @evas_libs@ \
@esmart_libs@

