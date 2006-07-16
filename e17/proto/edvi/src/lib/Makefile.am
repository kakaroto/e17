
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EDVI_EWL
EDVI_EWL_WIDGET = ewl_dvi.c
EDVI_EWL_HEADER = ewl_dvi.h
endif

if BUILD_EDVI_ETK
EDVI_ETK_WIDGET = etk_dvi.c
EDVI_ETK_HEADER = etk_dvi.h
endif

INCLUDES = 			\
-I. -I$(top_srcdir)/src/lib	\
@EVAS_CFLAGS@ @ECORE_CFLAGS@ @EWL_CFLAGS@ @ETK_CFLAGS@ @DVILIB2_CFLAGS@

lib_LTLIBRARIES = libedvi.la

EDVIHEADERS = \
Edvi.h \
edvi.h \
edvi_device.h \
edvi_document.h \
edvi_enum.h \
edvi_forward.h \
edvi_framebuffer.h \
edvi_page.h \
edvi_property.h \
esmart_dvi.h \
$(EDVI_EWL_HEADER) \
$(EDVI_ETK_HEADER)

libedvi_la_SOURCES = \
edvi.c \
edvi_device.c \
edvi_document.c \
edvi_framebuffer.c \
edvi_page.c \
edvi_property.c \
esmart_dvi.c \
$(EDVI_EWL_WIDGET) \
$(EDVI_ETK_WIDGET) \
$(EDVIHEADERS) \
edvi_private.h

installed_headersdir = $(prefix)/include/edvi
installed_headers_DATA = $(EDVIHEADERS)

libedvi_la_LIBADD = @DVILIB2_LIBS@ @ETK_LIBS@ @EWL_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@
