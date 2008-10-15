MAINTAINERCLEANFILES = Makefile.in e_modules-wlan.spec

SUBDIRS = po

EDJE_FLAGS = -v \
-id $(top_srcdir) \
-fd $(top_srcdir)

# data files for the module
filesdir = $(datadir)

files_DATA = module.desktop \
e-module-wlan.edj \
wlan.edj

EXTRA_DIST = $(files_DATA) \
				wlan.edc \
				VeraMono.ttf \
				wlan.png \
				e_modules-wlan.spec

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
	 rm -rf wlan.edj *~
