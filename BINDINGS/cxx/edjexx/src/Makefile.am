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
	Base.cpp  \
	Part.cpp  \
	Object.cpp  \
	Exceptions.cpp  \
	Edit.cpp  \
	PartEdit.cpp  \
	StateEdit.cpp  \
	ImageStateEdit.cpp  \
	TextStateEdit.cpp  \
	GradientStateEdit.cpp  \
	ExternalParam.cpp

libedjexx_la_LIBADD = \
	$(EFL_LIBS)

