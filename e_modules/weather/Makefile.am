MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = images po

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v \
-id $(top_srcdir)/images \
-fd $(top_srcdir)/fonts

# data files for the module
filesdir = $(datadir)

files_DATA = \
module_icon.png \
module.eap \
weather.edj

EXTRA_DIST = $(files_DATA) \
				e_modules-weather.spec \
				weather.edc \
				fonts/VeraBd.ttf \
				fonts/vera.ttf

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
			
module_la_LIBADD       = @e_libs@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

weather.edj: Makefile weather.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 weather.edc \
	 weather.edj

clean-local:
	 rm -rf *.edj *~

