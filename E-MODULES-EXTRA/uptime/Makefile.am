MAINTAINERCLEANFILES = Makefile.in e_modules-uptime.spec

SUBDIRS = po

EDJE_FLAGS = -v \
-id $(top_srcdir) \
-fd $(top_srcdir)

# data files for the module
filesdir = $(datadir)
files_DATA = \
uptime.edj \
module.desktop \
e-module-uptime.edj

EXTRA_DIST = $(files_DATA) \
	     uptime.png \
	     uptime.edc \
	     VeraMono.ttf \
	     e_modules-uptime.spec


# the module .so file
INCLUDES = -I. \
	 -I$(top_srcdir) \
	 -I$(includedir) \
	 @e_cflags@

pkgdir                 = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
		       e_mod_main.h \
		       e_mod_config.c

module_la_LIBADD       = @e_libs@ -lm
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf uptime.edj *~

