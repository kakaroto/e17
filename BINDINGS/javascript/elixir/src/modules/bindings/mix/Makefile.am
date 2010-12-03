AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

if BUILD_MODULE_MIX
if !EINA_STATIC_BUILD_MIX

INCLUDES = -I. -I$(top_srcdir)/src/include @sdl_cflags@ @elixir_cflags@

pkgdir = $(libdir)/elixir/bindings/mix/$(MODULE_ARCH)

pkg_LTLIBRARIES = module.la

module_la_SOURCES = mixer_module.c

module_la_LIBADD = $(top_builddir)/src/lib/libelixir.la @sdl_libs@ -lSDL_mixer @elixir_libs@
module_la_LDFLAGS = -module -avoid-version -L$(tob_buildir)/src/lib -L$(top_buildir)/src/lib/.libs
module_la_DEPENDENCIES = $(top_builddir)/config.h

endif
endif

EXTRA_DIST = mixer_module.c

