AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

if BUILD_MODULE_EDJE
if !EINA_STATIC_BUILD_EDJE

INCLUDES = -I. -I$(top_srcdir)/src/include @EDJE_CFLAGS@

pkgdir = $(libdir)/elixir/bindings/edje/$(MODULE_ARCH)

pkg_LTLIBRARIES = module.la

module_la_SOURCES = edje_module.c edje_layout_module.c

module_la_LIBADD = $(top_builddir)/src/lib/libelixir.la @EDJE_LIBS@
module_la_LDFLAGS = -module -avoid-version -L$(tob_buildir)/src/lib -L$(top_buildir)/src/lib/.libs
module_la_DEPENDENCIES = $(top_builddir)/config.h

endif
endif

EXTRA_DIST = edje_module.c edje_layout_module.c

