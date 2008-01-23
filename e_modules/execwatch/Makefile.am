MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = 

EDJE_FLAGS = -v \
-id $(top_srcdir) \
-fd $(top_srcdir)

files_DATA = \
module.desktop \
e-module-execwatch.edj \
execwatch.edj

EXTRA_DIST = $(files_DATA) \
execwatch.edc \
VeraMono.ttf \
VeraBd.ttf \
execwatch.png \
e_modules-execwatch.spec

# data files for the module
filesdir = $(datadir)

# the module .so file
INCLUDES = -I. \
	 -I$(top_srcdir) \
	 -I$(includedir) \
	 @e_cflags@

pkgdir                 = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
		       e_mod_main.h \
			   e_mod_config.c \
			   e_mod_config.h

module_la_LIBADD       = @e_libs@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf execwatch.edj *~
