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
	libewlxx.la

libewlxx_la_SOURCES = \
	EwlApplication.cpp\
	EwlObject.cpp \
	EwlWidget.cpp \
	EwlContainer.cpp \
	EwlOverlay.cpp \
	EwlBox.cpp \
	EwlHBox.cpp \
	EwlVBox.cpp \
	EwlButton.cpp \
	EwlWindow.cpp \
	EwlEmbed.cpp \
	EvasEwl.cpp

libewlxx_la_LIBADD = \
	$(EFL_LIBS)

