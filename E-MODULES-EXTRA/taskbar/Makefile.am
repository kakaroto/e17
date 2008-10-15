MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = data po

# data files for the module
filesdir = $(datadir)
files_DATA = module.desktop e-module-taskbar.edj

EXTRA_DIST = $(files_DATA) \
	     e_modules-taskbar.spec

# the module .so file
INCLUDES               = -I. \
			 -I$(top_srcdir) \
			 -I$(includedir) \
			 @E_CFLAGS@
pkgdir                 = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
			e_mod_main.h \
		        e_mod_config.c

module_la_LIBADD       = @E_LIBS@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

clean-local:
	 rm -rf *~

