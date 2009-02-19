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

if BUILD_INPUT_LIRC

pkgdir = $(libdir)/enna/modules

pkg_LTLIBRARIES = input_lirc.la

input_lirc_la_SOURCES= \
enna_module_lirc.c

input_lirc_la_LIBADD = @ENNA_LIBS@ @LIRC_LIBS@
input_lirc_la_LDFLAGS = -module -avoid-version
input_lirc_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/modules/input_$(MODULE).*

endif