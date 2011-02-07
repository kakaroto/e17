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
	libecorexx.la

libecorexx_la_SOURCES = \
	Application.cpp\
	EvasWindow.cpp \
	XWindow.cpp \
	Animator.cpp \
	Timer.cpp \
	Job.cpp \
	EvasWindowFB.cpp \
	EvasWindowGLX11.cpp \
	EvasWindowSoftwareX11.cpp \
	EvasWindowXRenderX11.cpp

libecorexx_la_LIBADD = \
	$(EFL_LIBS)

