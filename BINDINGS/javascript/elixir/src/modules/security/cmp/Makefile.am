AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

if BUILD_MODULE_CMP
if !EINA_STATIC_BUILD_CMP

INCLUDES = -I. -I$(top_srcdir)/src/include @EINA_CFLAGS@ @elixir_cflags@

pkgdir = $(libdir)/elixir/security/cmp/$(MODULE_ARCH)

pkg_LTLIBRARIES = module.la

module_la_SOURCES = cmp_security.c

module_la_LIBADD = $(top_builddir)/src/lib/libelixir.la @EINA_LIBS@
module_la_LDFLAGS = -module -avoid-version -L$(tob_buildir)/src/lib -L$(top_buildir)/src/lib/.libs
module_la_DEPENDENCIES = $(top_builddir)/config.h

endif
endif

EXTRA_DIST = cmp_security.c

