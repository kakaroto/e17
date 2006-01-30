SUBDIRS = po

MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v

# data files for the module
filesdir = $(datadir)

files_DATA = \
module_icon.png \
monitor.edj \
monitor.edc \
bg.png \
cpu.png \
eth_dev.png \
memory.png \
over.png \
over_res.png \
wireless.png \
VeraMono.ttf

EXTRA_DIST = $(files_DATA) e_modules-monitor.spec

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
		       e_mod_config.h \
		       flow_chart.c \
		       flow_chart.h \
		       linux_2.6.c \
		       linux_2.6.h 

module_la_LIBADD       = @e_libs@ -lm
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

monitor.edj: Makefile monitor.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 monitor.edc \
	 monitor.edj

clean-local:
	 rm -rf *.edj *~

