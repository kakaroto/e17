
MAINTAINERCLEANFILES = Makefile.in

lib_LTLIBRARIES = libepdf.la

if BUILD_EPDF_EWL
lib_LTLIBRARIES += libepdf_ewl.la
endif


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

if BUILD_EPDF_EWL
EPDFHEADERS += ewl_pdf.h
endif

installed_headersdir = $(prefix)/include/epdf
installed_headers_DATA = $(EPDFHEADERS)

libepdf_la_SOURCES = \
epdf_main.cpp \
epdf_document.cpp \
epdf_fontinfo.cpp \
epdf_index.cpp \
epdf_page.cpp \
epdf_page_transition.cpp \
epdf_postscript.cpp \
esmart_pdf.c \
$(EPDF_EWL_WIDGET)

libepdf_la_CPPFLAGS = @EPDF_CFLAGS@
libepdf_la_LIBADD = @EPDF_LIBS@
libepdf_la_LDFLAGS = -no-undefined -version-info @version_info@

if BUILD_EPDF_EWL

libepdf_ewl_la_SOURCES = ewl_pdf.c
libepdf_ewl_la_CPPFLAGS = @EWL_CFLAGS@ @EPDF_CFLAGS@
libepdf_ewl_la_LIBADD = @EWL_LIBS@ @EPDF_LIBS@
libepdf_ewl_la_LDFLAGS = -no-undefined -version-info @version_info@

endif

EXTRA_DIST = $(EPDFHEADERS)
