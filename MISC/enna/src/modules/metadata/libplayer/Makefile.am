MAINTAINERCLEANFILES = Makefile.in

MODULE = libplayer

# data files for the module
filesdir = $(libdir)/enna/modules/

# the module .so file
INCLUDES = \
-I. \
-I$(top_srcdir) \
-I$(top_srcdir)/src/modules/$(MODULE) \
-I$(top_srcdir)/src/bin \
-I$(top_srcdir)/src/modules \
@ENNA_CFLAGS@ @LIBPLAYER_CFLAGS@

if BUILD_METADATA_LIBPLAYER

pkgdir = $(libdir)/enna/modules

pkg_LTLIBRARIES = metadata_libplayer.la

metadata_libplayer_la_SOURCES= \
libplayer.c codecs.c

metadata_libplayer_la_LIBADD = @ENNA_LIBS@ @LIBPLAYER_LIBS@
metadata_libplayer_la_LDFLAGS = -module -avoid-version
metadata_libplayer_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/modules/metadata_$(MODULE).*

endif
