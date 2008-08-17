MAINTAINERCLEANFILES = Makefile.in

lib_LTLIBRARIES=libentrance_smarts.la

libentrance_smarts_la_HEADERS = \
	Entrance_Smart.h \
	Esmart_Preview.h

libentrance_smarts_ladir = $(prefix)/include


libentrance_smarts_la_SOURCES = \
	entrance_smart.c \
	esmart_preview.c \
	$(libentrance_smarts_la_HEADERS)
	
libentrance_smarts_la_LIBADD  = @ecore_libs@ @evas_libs@ @edje_libs@
libentrance_smarts_la_CFLAGS = @ecore_cflags@ @evas_cflags@ @edje_cflags@
