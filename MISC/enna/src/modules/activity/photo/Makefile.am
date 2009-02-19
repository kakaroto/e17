MAINTAINERCLEANFILES = Makefile.in

MODULE = photo

# data files for the module
filesdir = $(libdir)/enna/modules/

# the module .so file
INCLUDES = \
-I. \
-I$(top_srcdir) \
-I$(top_srcdir)/src/modules/$(MODULE) \
-I$(top_srcdir)/src/bin \
-I$(top_srcdir)/src/modules \
@ENNA_CFLAGS@ @LIBEXIF_CFLAGS@

if BUILD_ACTIVITY_PHOTO

pkgdir = $(libdir)/enna/modules

pkg_LTLIBRARIES = activity_photo.la

activity_photo_la_SOURCES= \
enna_module_photo.c

activity_photo_la_LIBADD = @ENNA_LIBS@ @LIBEXIF_LIBS@
activity_photo_la_LDFLAGS = -module -avoid-version
activity_photo_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/modules/activity_$(MODULE).*

endif
