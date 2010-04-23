
MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = poppler

lib_LTLIBRARIES = libepdf.la

if BUILD_EPDF_EWL
lib_LTLIBRARIES += libepdf_ewl.la
endif

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

if BUILD_EPDF_EWL

dist_installed_headers_DATA += ewl_pdf.h

endif

libepdf_la_SOURCES = esmart_pdf.c

libepdf_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
@EPDF_CFLAGS@

libepdf_la_LIBADD = poppler/libepdf_poppler.la
libepdf_la_LDFLAGS = -no-undefined -version-info @version_info@

if BUILD_EPDF_EWL

libepdf_ewl_la_SOURCES = ewl_pdf.c
libepdf_ewl_la_CPPFLAGS = @EWL_CFLAGS@ poppler/libepdf_poppler.la
libepdf_ewl_la_LIBADD = @EWL_LIBS@ @EPDF_LIBS@
libepdf_ewl_la_LDFLAGS = -no-undefined -version-info @version_info@

endif
