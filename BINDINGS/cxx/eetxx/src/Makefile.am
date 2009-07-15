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
	libeetxx.la

libeetxx_la_SOURCES = \
	EetDocument.cpp\
	EetList.cpp \
	EetChunk.cpp \
	ChunkIterator.cpp

libeetxx_la_LIBADD = \
	$(EFL_LIBS)

