MAINTAINERCLEANFILES = Makefile.in e_modules-net.spec

SUBDIRS = po

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v
EXTRA_DIST=$(files_DATA) eth_dev.png module_icon.png e_modules-net.spec e_modules-net.spec.in
# data files for the module
filesdir = $(datadir)

files_DATA = \
module_icon.png \
net.edj \
net.edc \
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

net.edj: Makefile net.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 net.edc \
	 net.edj

clean-local:
	 rm -rf *.edj *~

