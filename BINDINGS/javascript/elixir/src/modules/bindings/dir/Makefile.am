AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I. -I$(top_srcdir)/src/include @EINA_CFLAGS@ @elixir_cflags@

if BUILD_MODULE_DIR
if !EINA_STATIC_BUILD_DIR

pkgdir = $(libdir)/elixir/bindings/dir/$(MODULE_ARCH)

pkg_LTLIBRARIES = module.la

module_la_SOURCES = dir_module.c

module_la_LIBADD = $(top_builddir)/src/lib/libelixir.la @EINA_LIBS@
module_la_LDFLAGS = -module -avoid-version -L$(tob_buildir)/src/lib -L$(top_buildir)/src/lib/.libs
module_la_DEPENDENCIES = $(top_builddir)/config.h

include_HEADERS =

endif
endif

EXTRA_DIST = dir_module.c
