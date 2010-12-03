AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

if !EINA_STATIC_BUILD_TEXT

INCLUDES = -I. -I$(top_srcdir)/src/include @EINA_CFLAGS@ @elixir_cflags@

pkgdir = $(libdir)/elixir/loaders/text/$(MODULE_ARCH)

pkg_LTLIBRARIES = module.la

module_la_SOURCES = text_loader.c

module_la_LIBADD = $(top_builddir)/src/lib/libelixir.la @EINA_LIBS@
module_la_LDFLAGS = -module -avoid-version -L$(tob_buildir)/src/lib -L$(top_buildir)/src/lib/.libs
module_la_DEPENDENCIES = $(top_builddir)/config.h

endif

EXTRA_DIST = text_loader.c

