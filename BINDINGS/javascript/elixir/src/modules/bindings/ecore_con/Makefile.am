AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

if BUILD_MODULE_ECORE_CON
if !EINA_STATIC_BUILD_ECORE_CON

INCLUDES = -I. -I$(top_srcdir)/src/include @ECORE_CON_CFLAGS@ @elixir_cflags@

pkgdir = $(libdir)/elixir/bindings/ecore-con/$(MODULE_ARCH)

pkg_LTLIBRARIES = module.la

module_la_SOURCES = ecore_con.c

module_la_LIBADD = $(top_builddir)/src/lib/libelixir.la @ECORE_CON_LIBS@
module_la_LDFLAGS = -module -avoid-version -L$(tob_buildir)/src/lib -L$(top_buildir)/src/lib/.libs
module_la_DEPENDENCIES = $(top_builddir)/config.h

endif
endif

EXTRA_DIST = ecore_con.c
