MAINTAINERCLEANFILES = Makefile.in

MODULE = libplayer

# data files for the module
filesdir = $(libdir)/enna/modules/

# the module .so file
INCLUDES = \
-I. \
-I$(top_srcdir)/src/bin \
@ENNA_CFLAGS@ \
@LIBPLAYER_CFLAGS@

if BUILD_BACKEND_LIBPLAYER

pkgdir =$(libdir)/enna/modules

pkg_LTLIBRARIES = backend_libplayer.la

backend_libplayer_la_SOURCES = \
enna_module_libplayer.c

backend_libplayer_la_LIBADD = @ENNA_LIBS@ @LIBPLAYER_LIBS@
backend_libplayer_la_LDFLAGS = -module -avoid-version
backend_libplayer_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/modules/backend_$(MODULE).*

endif

