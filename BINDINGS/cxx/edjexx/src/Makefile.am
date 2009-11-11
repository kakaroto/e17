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
	libedjexx.la

libedjexx_la_SOURCES = \
	EdjeBase.cpp  \
	EdjePart.cpp  \
	EdjeObject.cpp  \
	EdjeExceptions.cpp

libedjexx_la_LIBADD = \
	$(EFL_LIBS)

