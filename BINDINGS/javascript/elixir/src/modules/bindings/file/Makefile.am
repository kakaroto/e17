AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I. -I$(top_srcdir)/src/include @EINA_CFLAGS@ @elixir_cflags@

if BUILD_MODULE_FILE
if !EINA_STATIC_BUILD_FILE

pkgdir = $(libdir)/elixir/bindings/file/$(MODULE_ARCH)

pkg_LTLIBRARIES = module.la

module_la_SOURCES = file_module.c

module_la_LIBADD = $(top_builddir)/src/lib/libelixir.la @EINA_LIBS@
module_la_LDFLAGS = -module -avoid-version -L$(tob_buildir)/src/lib -L$(top_buildir)/src/lib/.libs
module_la_DEPENDENCIES = $(top_builddir)/config.h

include_HEADERS =

endif
endif

EXTRA_DIST = file_module.c
