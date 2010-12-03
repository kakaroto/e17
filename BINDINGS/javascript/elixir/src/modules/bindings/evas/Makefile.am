AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

if BUILD_MODULE_EVAS
if !EINA_STATIC_BUILD_EVAS

INCLUDES = -I. -I$(top_srcdir)/src/include @EVAS_CFLAGS@

if BUILD_MODULE_ECORE_EVAS
INCLUDES += @ECORE_EVAS_CFLAGS@
endif

pkgdir = $(libdir)/elixir/bindings/evas/$(MODULE_ARCH)

pkg_LTLIBRARIES = module.la

module_la_SOURCES = evas-bindings.c evas-const-binding.c evas-events-binding.c \
		evas-int-params.c evas-object-2-params.c evas-object-binding.c \
		evas-object-boolean-params.c evas-object-color-params.c \
		evas-object-image-binding.c evas-object-int-2-params.c \
		evas-object-int-params.c evas-object-line-binding.c \
		evas-object-params.c evas-object-string-params.c \
		evas-object-textblock.c evas-object-text.c evas-params.c \
		evas-string-params.c void-params.c evas-smart.c \
		evas-object-int-3-params.c evas-object-double-2-params.c \
		evas-object-int-4-params.c evas-object-double-params.c

module_la_LIBADD = $(top_builddir)/src/lib/libelixir.la @EVAS_LIBS@

if BUILD_MODULE_ECORE_EVAS
module_la_LIBADD += @ECORE_EVAS_LIBS@
endif

module_la_LDFLAGS = -module -avoid-version -L$(tob_buildir)/src/lib -L$(top_buildir)/src/lib/.libs
module_la_DEPENDENCIES = $(top_builddir)/config.h

endif
endif

EXTRA_DIST =  evas-bindings.c evas-const-binding.c evas-events-binding.c \
		evas-int-params.c evas-object-2-params.c evas-object-binding.c \
		evas-object-boolean-params.c evas-object-color-params.c \
		evas-object-image-binding.c evas-object-int-2-params.c \
		evas-object-int-params.c evas-object-line-binding.c \
		evas-object-params.c evas-object-string-params.c \
		evas-object-textblock.c evas-object-text.c evas-params.c \
		evas-string-params.c void-params.c evas-bindings.h evas-smart.c \
		evas-object-int-3-params.c evas-object-double-2-params.c \
		evas-object-int-4-params.c evas-object-double-params.c
