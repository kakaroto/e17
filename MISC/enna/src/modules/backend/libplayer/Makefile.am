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

if BUILD_LIBPLAYER_BACKEND

pkgdir =$(libdir)/enna/modules

pkg_LTLIBRARIES = enna_module_libplayer.la

enna_module_libplayer_la_SOURCES = \
enna_module_libplayer.c

enna_module_libplayer_la_LIBADD = @ENNA_LIBS@ @LIBPLAYER_LIBS@
enna_module_libplayer_la_LDFLAGS = -module -avoid-version
enna_module_libplayer_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/module/enna_module_$(MODULE).*

endif

