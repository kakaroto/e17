MAINTAINERCLEANFILES = Makefile.in

# data files for the module
filesdir = $(datadir)
files_DATA = \
module.desktop \
module.edj

EXTRA_DIST = $(files_DATA) \
e_modules-bling.spec \
compmgr.h

# the module .so file
INCLUDES               = -I. \
			 -I$(top_srcdir) \
			 -I$(includedir) \
			 @e_cflags@ @x_cflags@
pkgdir                 = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = compmgr.c \
								 e_mod_main.c \
		         e_mod_main.h \
			 e_mod_config.c \
			 e_mod_config.h
module_la_LIBADD       = @e_libs@ @x_libs@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

clean-local:
	 rm -rf *~
uninstall:
	rm -rf $(datadir)
