AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

if BUILD_MODULE_EIO
if !EINA_STATIC_BUILD_EIO

INCLUDES = -I. -I$(top_srcdir)/src/include @EIO_CFLAGS@ @elixir_cflags@

pkgdir = $(libdir)/elixir/bindings/eio/$(MODULE_ARCH)

pkg_LTLIBRARIES = module.la

module_la_SOURCES = eio_module.c

module_la_LIBADD = $(top_builddir)/src/lib/libelixir.la @EIO_LIBS@
module_la_LDFLAGS = -module -avoid-version -L$(tob_buildir)/src/lib -L$(top_buildir)/src/lib/.libs
module_la_DEPENDENCIES = $(top_builddir)/config.h

endif
endif

EXTRA_DIST = eio_module.c

