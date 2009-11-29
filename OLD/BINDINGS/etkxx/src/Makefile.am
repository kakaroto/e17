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
	libetkxx.la

libetkxx_la_SOURCES = \
	EtkApplication.cpp\
	EtkBox.cpp \
	EtkButton.cpp \
	EtkContainer.cpp \
	EtkHBox.cpp \
	EtkImage.cpp \
	EtkObject.cpp \
	EtkTopLevel.cpp \
	EtkWidget.cpp \
	EtkWindow.cpp

libetkxx_la_LIBADD = \
	$(EFL_LIBS)

