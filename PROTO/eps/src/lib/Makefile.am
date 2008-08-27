
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EPS_EWL
EPS_EWL_WIDGET = ewl_ps.c
EPS_EWL_HEADER = ewl_ps.h
endif

if BUILD_EPS_ETK
EPS_ETK_WIDGET = etk_ps.c
EPS_ETK_HEADER = etk_ps.h
endif

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/lib	\
@EWL_CFLAGS@ \
@ETK_CFLAGS@ \
@EPS_CFLAGS@

lib_LTLIBRARIES = libeps.la

EPSHEADERS = \
Eps.h \
eps_main.h \
eps_document.h \
eps_enum.h \
eps_page.h \
eps_forward.h \
esmart_ps.h \
eps_private.h \
$(EPS_EWL_HEADER) \
$(EPS_ETK_HEADER)

libeps_la_SOURCES = \
eps_main.c \
eps_document.c \
eps_page.c \
esmart_ps.c \
$(EPS_EWL_WIDGET) \
$(EPS_ETK_WIDGET)

installed_headersdir = $(prefix)/include/eps
installed_headers_DATA = $(EPSHEADERS)

libeps_la_LIBADD  = @ETK_LIBS@ @EWL_LIBS@ @EPS_LIBS@
libeps_la_LDFLAGS = -version-info @version_info@
libeps_la_DEPENDENCIES = $(top_builddir)/config.h

EXTRA_DIST = \
Eps.h \
eps_main.h \
eps_document.h \
eps_enum.h \
eps_page.h \
eps_forward.h \
esmart_ps.h \
eps_private.h \
ewl_ps.h \
etk_ps.h
