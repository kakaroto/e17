MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = images

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id images -fd fonts

# data files for the module
filesdir = $(libdir)/e_modules/weather

files_DATA = \
module_icon.png \
weather.edj \
dir.xml

EXTRA_DIST = $(files_DATA)

# the module .so file
INCLUDES = -I. \
			  -I$(top_srcdir) \
			  -I$(includedir) \
			   @e_cflags@

pkgdir                 = @e_modules@/weather/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
			 e_mod_main.h

module_la_LIBADD       = @e_libs@ -lm
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

weather.edj: Makefile weather.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 weather.edc \
	 weather.edj

clean-local:
	 rm -rf *.edj *~

