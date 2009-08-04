SUBDIRS = plugins

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I. \
	   -I$(top_srcdir) \
	   -I$(includedir) \
	   @ETHUMB_CFLAGS@ \
	   @E_CFLAGS@

pkgdir = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES = module.la

DRAWERHEADERS = Drawer.h

module_la_SOURCES = $(DRAWERHEADERS) \
		    e_mod_main.h \
		    e_mod_main.c \
                    e_mod_config.c

module_la_LIBADD = @ETHUMB_LIBS@ @E_LIBS@
module_la_LDFLAGS = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

installed_headersdir = $(prefix)/include/drawer
installed_headers_DATA = $(DRAWERHEADERS)

clean-local:
	rm -rf *~
