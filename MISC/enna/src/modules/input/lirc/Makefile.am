MAINTAINERCLEANFILES = Makefile.in

MODULE = lirc

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

if BUILD_LIRC_MODULE

pkgdir = $(libdir)/enna/modules

pkg_LTLIBRARIES = enna_module_lirc.la

enna_module_lirc_la_SOURCES= \
enna_module_lirc.c

enna_module_lirc_la_LIBADD = @ENNA_LIBS@ @LIRC_LIBS@
enna_module_lirc_la_LDFLAGS = -module -avoid-version
enna_module_lirc_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/module/enna_module_$(MODULE).*

endif