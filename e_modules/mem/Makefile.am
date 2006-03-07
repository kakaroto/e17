MAINTAINERCLEANFILES = Makefile.in e_modules-mem.spec

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v

# data files for the module
filesdir = $(datadir)

EXTRA_DIST=e_modules-mem.spec.in e_modules-mem.spec autogen.sh mem.png module_icon.png $(files_DATA)

files_DATA = \
module_icon.png \
mem.edj \
mem.edc \
VeraMono.ttf

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
		       
module_la_LIBADD       = @e_libs@ -lm
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

mem.edj: Makefile mem.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 mem.edc \
	 mem.edj

clean-local:
	 rm -rf *.edj *~

