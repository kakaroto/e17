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
	EvasCanvas.cpp \
	EvasImage.cpp \
	EvasObject.cpp \
	EvasText.cpp \
	EvasTextblockStyle.cpp \
	EvasLine.cpp \
	EvasRectangle.cpp \
	EvasPolygon.cpp \
	EvasTextblock.cpp \
	EvasGradient.cpp \
	EvasTransform.cpp \
	EvasSmart.cpp \
	EvasExceptions.cpp

libevasxx_la_LIBADD = \
	$(EFL_LIBS)    \
	$(SIGC_LIBS)

