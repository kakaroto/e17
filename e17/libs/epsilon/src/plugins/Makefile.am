# Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS     = 1.7 foreign

LDFLAGS              = -rdynamic -shared
INCLUDES             = -I. -I$(top_srcdir) -I$(top_srcdir)/src/include \
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
xine_thumbnailer_la_LIBADD = @xine_LIBS@
xine_thumbnailer_la_CFLAGS = @xine_CFLAGS@

