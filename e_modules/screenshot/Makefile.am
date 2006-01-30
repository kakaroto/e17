SUBDIRS = po

MAINTAINERCLEANFILES = Makefile.in mkinstalldirs

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v

# data files for the module
filesdir =  $(datadir)

files_DATA = \
module_icon.png \
screenshot.edj \
screenshot.edc \
VeraBd.ttf

EXTRA_DIST = $(files_DATA) e_modules-screenshot.spec

# the module .so file
INCLUDES = -I. \
			  -I$(top_srcdir) \
			  -I$(includedir) \
			   @e_cflags@

pkgdir                 =  $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
		         e_mod_main.h \
			 e_mod_config.c \
			 e_mod_config.h
module_la_LIBADD       = @e_libs@ -lm
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

screenshot.edj: Makefile screenshot.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 screenshot.edc \
	 screenshot.edj

clean-local:
	 rm -rf *.edj *~
