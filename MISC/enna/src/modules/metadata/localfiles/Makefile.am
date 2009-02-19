MAINTAINERCLEANFILES = Makefile.in

MODULE = localfiles

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

if BUILD_METADATA_LOCALFILES

pkgdir = $(libdir)/enna/modules

pkg_LTLIBRARIES = metadata_localfiles.la

metadata_localfiles_la_SOURCES= \
localfiles.c

metadata_localfiles_la_LIBADD = @ENNA_LIBS@
metadata_localfiles_la_LDFLAGS = -module -avoid-version
metadata_localfiles_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/modules/localfiles_$(MODULE).*

endif
