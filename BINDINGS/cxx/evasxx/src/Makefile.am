## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(EFL_CFLAGS) \
	$(SIGC_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

lib_LTLIBRARIES = \
	libevasxx.la

libevasxx_la_SOURCES = \
	Canvas.cpp \
	Image.cpp \
	Object.cpp \
	Text.cpp \
	TextblockStyle.cpp \
	Line.cpp \
	Rectangle.cpp \
	Polygon.cpp \
	Textblock.cpp \
	Smart.cpp \
	Exceptions.cpp

libevasxx_la_LIBADD = \
	$(EFL_LIBS)    \
	$(SIGC_LIBS)

