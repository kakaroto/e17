
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EPDF_EWL
EPDF_EWL_WIDGET = ewl_pdf.c
EPDF_EWL_HEADER = ewl_pdf.h
endif

if BUILD_EPDF_ETK
EPDF_ETK_WIDGET = etk_pdf.c
EPDF_ETK_HEADER = etk_pdf.h
endif

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/lib	\
@EWL_CFLAGS@ \
@ETK_CFLAGS@ \
@EPDF_CFLAGS@

lib_LTLIBRARIES = libepdf.la

EPDFHEADERS = \
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
epdf_private.h \
esmart_pdf.h \
$(EPDF_EWL_HEADER) \
$(EPDF_ETK_HEADER)

libepdf_la_SOURCES = \
epdf_main.cpp \
epdf_document.cpp \
epdf_fontinfo.cpp \
epdf_index.cpp \
epdf_page.cpp \
epdf_page_transition.cpp \
epdf_postscript.cpp \
esmart_pdf.c \
$(EPDF_EWL_WIDGET) \
$(EPDF_ETK_WIDGET)

installed_headersdir = $(prefix)/include/epdf
installed_headers_DATA = $(EPDFHEADERS)

libepdf_la_LIBADD  = @ETK_LIBS@ @EWL_LIBS@ @EPDF_LIBS@
libepdf_la_LDFLAGS = @create_shared_lib@ -version-info @version_info@

EXTRA_DIST = $(EPDFHEADERS)
