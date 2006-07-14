MAINTAINERCLEANFILES = Makefile.in e_modules-wlan.spec

SUBDIRS = po

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v \
-id $(top_srcdir) \
-fd $(top_srcdir)

# data files for the module
filesdir = $(datadir)

files_DATA = \
module_icon.png \
module.eap \
wlan.edj

EXTRA_DIST = $(files_DATA) \
				wlan.edc \
				VeraMono.ttf \
				wlan.png \
				module_icon.png \
				e_modules-wlan.spec \
				e_modules-wlan.spec.in

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
	 rm -rf *.edj *~

