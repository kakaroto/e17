AUTOMAKE_OPTIONS	= 1.4 foreign

MAINTAINERCLEANFILES	= Makefile.in

INCLUDES		= -I. -I$(top_srcdir)/src/include @elixir_cflags@

if !EINA_STATIC_BUILD_ELIXIR

pkgdir = $(libdir)/elixir/bindings/elixir/$(MODULE_ARCH)

pkg_LTLIBRARIES = module.la

module_la_SOURCES = main_module.c

module_la_LIBADD = $(top_builddir)/src/lib/libelixir.la
module_la_LDFLAGS = -module -avoid-version -L$(top_builddir)/src/lib -L$(top_builddir)/src/lib/.libs @elixir_libs@
module_la_DEPENDENCIES = $(top_srcdir)/src/include/Elixir.h

include_HEADERS =

endif

EXTRA_DIST = main_module.c
