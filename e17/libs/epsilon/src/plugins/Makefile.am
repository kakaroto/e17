
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS          = \
-I. \
-I$(top_srcdir) \
-I$(top_srcdir)/src/include \
-I$(top_srcdir)/src/lib \
@ECORE_CFLAGS@ \
@XINE_CFLAGS@

pkgdir =  $(libdir)/epsilon/plugins/

if HAVE_XINE
XINE_L = xine_thumbnailer.la
else
XINE_L =
endif


pkg_LTLIBRARIES = $(XINE_L)

xine_thumbnailer_la_SOURCES = xine_thumbnailer.c
xine_thumbnailer_la_LDFLAGS = -module -avoid-version
xine_thumbnailer_la_LIBADD = @XINE_LIBS@ @ECORE_LIBS@
