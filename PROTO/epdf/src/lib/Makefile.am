
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EPDF_MUPDF

SUBDIRS = mupdf

else

SUBDIRS = poppler

endif

lib_LTLIBRARIES = libepdf.la

installed_headersdir = $(includedir)/epdf

dist_installed_headers_DATA = \
Epdf.h \
epdf_document.h \
epdf_enum.h \
epdf_fontinfo.h \
epdf_forward.h \
epdf_main.h \
epdf_index.h \
epdf_page.h \
epdf_page_transition.h \
epdf_postscript.h \
esmart_pdf.h

libepdf_la_SOURCES = esmart_pdf.c

libepdf_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
@EFL_EPDF_BUILD@ \
@EPDF_CFLAGS@

if BUILD_EPDF_MUPDF

libepdf_la_LIBADD = mupdf/libepdf_mupdf.la mupdf/libfonts.la mupdf/libcmaps.la mupdf/libmupdf.la mupdf/libfitzdraw.la mupdf/libfitz.la

else

libepdf_la_LIBADD = poppler/libepdf_poppler.la

endif

libepdf_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -version-info @version_info@
