MAINTAINERCLEANFILES = Makefile.in

# data files for the module
filesdir = $(libdir)/enna/modules/

AM_CPPFLAGS = \
-I$(top_srcdir)/src/modules/udev \
-I$(top_srcdir)/src/bin \
@ENNA_CFLAGS@ \
@enna_module_volume_udev_cflags@

pkgdir = $(libdir)/enna/modules

pkg_LTLIBRARIES = volume_udev.la

volume_udev_la_SOURCES = udev.c

volume_udev_la_LIBADD = @ENNA_LIBS@ @enna_module_volume_udev_libs@
volume_udev_la_LDFLAGS = -module -avoid-version
