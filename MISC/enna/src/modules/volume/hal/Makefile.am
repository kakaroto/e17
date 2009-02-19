MAINTAINERCLEANFILES = Makefile.in

MODULE = hal

# data files for the module
filesdir = $(libdir)/enna/modules/

# the module .so file
INCLUDES = \
-I. \
-I$(top_srcdir) \
-I$(top_srcdir)/src/modules/$(MODULE) \
-I$(top_srcdir)/src/bin \
-I$(top_srcdir)/src/modules \
@ENNA_CFLAGS@ @EHAL_CFLAGS@

if BUILD_VOLUME_HAL

pkgdir = $(libdir)/enna/modules

pkg_LTLIBRARIES = volume_hal.la

volume_hal_la_SOURCES= \
enna_module_hal.c hal_storage.c hal_volume.c

volume_hal_la_LIBADD = @ENNA_LIBS@ @EHAL_LIBS@
volume_hal_la_LDFLAGS = -module -avoid-version
volume_hal_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/modules/volume_$(MODULE).*

endif