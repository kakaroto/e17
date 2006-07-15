## Process this file with automake to produce Makefile.in

INCLUDES = @edje_cflags@ @ecore_cflags@ @edb_cflags@ @evas_cflags@ \
@esmart_cflags@ @imlib2_cflags@ @eet_cflags@

bin_PROGRAMS = elicit 

elicit_SOURCES = \
        elicit.c Elicit.h \
        callbacks.c callbacks.h \
        util.c util.h \
        conf.c conf.h \
        swatches.c swatches.h \
        shots.c shots.h \
        themes.c themes.h \
				spectrum.c spectrum.h \
				zoom.c zoom.h

elicit_LDADD = @edje_libs@ @ecore_libs@ @edb_libs@ @evas_libs@ \
@esmart_libs@ @imlib2_libs@ @eet_libs@ -lesmart_draggies -lesmart_container

