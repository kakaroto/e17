
MAINTAINERCLEANFILES = Makefile.in

noinst_LTLIBRARIES = libepdf_poppler.la

libepdf_poppler_la_SOURCES = \
epdf_main.cpp \
epdf_document.cpp \
epdf_fontinfo.cpp \
epdf_index.cpp \
epdf_page.cpp \
epdf_page_transition.cpp \
epdf_postscript.cpp

libepdf_poppler_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-I$(top_srcdir)/src/lib/poppler \
-I$(top_builddir)/src/lib/poppler \
@EPDF_CFLAGS@

libepdf_poppler_la_LIBADD = @EPDF_LIBS@

EXTRA_DIST = epdf_private.h
