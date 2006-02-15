
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EPDF_EWL
EPDF_EWL_WIDGET = ewl_pdf.c
endif

if BUILD_EPDF_ETK
EPDF_ETK_WIDGET = etk_pdf.c
endif

INCLUDES = 			\
-I. -I$(top_srcdir)/src/lib	\
@EVAS_CFLAGS@ @ECORE_CFLAGS@ @EWL_CFLAGS@ @ETK_CFLAGS@ @POPPLER_CFLAGS@

lib_LTLIBRARIES = libepdf.la

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

libepdf_la_LIBADD = @POPPLER_LIBS@ @ETK_LIBS@ @EWL_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@
