
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EPDF_EWL
EPDF_EWL_WIDGET = ewl_pdf.c
EPDF_EWL_HEADER = ewl_pdf.h
endif

if BUILD_EPDF_ETK
EPDF_ETK_WIDGET = etk_pdf.c
EPDF_ETK_HEADER = etk_pdf.h
endif

INCLUDES = 			\
-I. -I$(top_srcdir)/src/lib	\
@EVAS_CFLAGS@ @ECORE_CFLAGS@ @EWL_CFLAGS@ @ETK_CFLAGS@ @POPPLER_CFLAGS@

lib_LTLIBRARIES = libepdf.la

EPDFHEADERS = \
Epdf.h \
poppler_document.h \
poppler_enum.h \
poppler_fontinfo.h \
poppler_forward.h \
poppler.h \
poppler_index.h \
poppler_page.h \
poppler_page_transition.h \
esmart_pdf.h \
$(EPDF_EWL_HEADER) \
$(EPDF_ETK_HEADER)

libepdf_la_SOURCES = \
poppler.cpp \
poppler_document.cpp \
poppler_fontinfo.cpp \
poppler_index.cpp \
poppler_page.cpp \
poppler_page_transition.cpp \
esmart_pdf.c \
$(EPDF_EWL_WIDGET) \
$(EPDF_ETK_WIDGET)

installed_headersdir = $(prefix)/include/epdf
installed_headers_DATA = $(EPDFHEADERS)

libepdf_la_LIBADD = @POPPLER_LIBS@ @ETK_LIBS@ @EWL_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@
