
MAINTAINERCLEANFILES = Makefile.in

noinst_LTLIBRARIES = libepdf_poppler.la

libepdf_poppler_la_SOURCES = \
epdf_poppler_document.cpp \
epdf_poppler_fontinfo.cpp \
epdf_poppler_index.cpp \
epdf_poppler_main.cpp \
epdf_poppler_page.cpp \
epdf_poppler_page_transition.cpp \
epdf_poppler_postscript.cpp

libepdf_poppler_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-I$(top_srcdir)/src/lib/poppler \
-I$(top_builddir)/src/lib/poppler \
@EPDF_CFLAGS@

libepdf_poppler_la_LIBADD = @EPDF_LIBS@
libepdf_poppler_la_LDFLAGS = -no-undefined @lt_enable_auto_import@

EXTRA_DIST = epdf_poppler_private.h
