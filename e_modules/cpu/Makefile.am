MAINTAINERCLEANFILES = Makefile.in e_modules-cpu.spec

SUBDIRS = po

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v

EXTRA_DIST = $(files_DATA) cpu.png e_modules-cpu.spec e_modules-cpu.spec.in 

# data files for the module
filesdir = $(datadir)

files_DATA = \
module_icon.png \
cpu.edj \
cpu.edc \
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

cpu.edj: Makefile cpu.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 cpu.edc \
	 cpu.edj

clean-local:
	 rm -rf *.edj *~

