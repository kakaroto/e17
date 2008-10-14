MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = images po

EDJE_FLAGS = -v \
-id $(top_srcdir)/images \
-fd $(top_srcdir)/fonts

# data files for the module
filesdir = $(datadir)

files_DATA = module.desktop \
e-module-forecasts.edj \
forecasts.edj

EXTRA_DIST = $(files_DATA) \
				e_modules-forecasts.spec \
				forecasts.edc \
				fonts/VeraBd.ttf \
				fonts/vera.ttf

# the module .so file
INCLUDES = -I. \
			  -I$(top_srcdir) \
			  -I$(includedir) \
			   @E_CFLAGS@

pkgdir                 = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
			 e_mod_main.h \
			 e_mod_config.c \
			 e_mod_config.h

module_la_LIBADD       = @E_LIBS@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf forecasts.edj *~
