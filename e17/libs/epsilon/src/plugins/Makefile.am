
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS          = @ECORE_CFLAGS@ \
		       -I. -I$(top_srcdir) -I$(top_srcdir)/src/include \
		       -I$(top_srcdir)/src/lib

pkgdir =  $(libdir)/epsilon/plugins/

if HAVE_XINE
XINE_L = xine_thumbnailer.la
else
XINE_L =
endif


pkg_LTLIBRARIES = $(XINE_L)

xine_thumbnailer_la_SOURCES = xine_thumbnailer.c
xine_thumbnailer_la_LDFLAGS = -module -avoid-version
xine_thumbnailer_la_LIBADD = @XINE_LIBS@ @IMLIB2_LIBS@ @ECORE_LIBS@
xine_thumbnailer_la_CPPFLAGS = @XINE_CFLAGS@ @IMLIB2_CFLAGS@ @ECORE_CFLAGS@
