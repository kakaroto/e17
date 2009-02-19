MAINTAINERCLEANFILES = Makefile.in

MODULE = mtab

# data files for the module
filesdir = $(libdir)/enna/modules/

# the module .so file
INCLUDES = \
-I. \
-I$(top_srcdir) \
-I$(top_srcdir)/src/modules/$(MODULE) \
-I$(top_srcdir)/src/bin \
-I$(top_srcdir)/src/modules \
@ENNA_CFLAGS@

if BUILD_VOLUME_MTAB

pkgdir = $(libdir)/enna/modules

pkg_LTLIBRARIES = volume_mtab.la

volume_mtab_la_SOURCES= enna_module_mtab.c

volume_mtab_la_LIBADD = @ENNA_LIBS@
volume_mtab_la_LDFLAGS = -module -avoid-version
volume_mtab_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/modules/volume_$(MODULE).*

endif
