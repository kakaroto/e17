## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(EFL_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

lib_LTLIBRARIES = \
	libemotionxx.la

libemotionxx_la_SOURCES = \
	Object.cpp       \
	SPUObject.cpp       \
	VideoObject.cpp       \
	AudioObject.cpp

libemotionxx_la_LIBADD = \
	$(EFL_LIBS)

