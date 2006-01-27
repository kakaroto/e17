MAINTAINERCLEANFILES = Makefile.in

# data files for the module
filesdir = $(datadir)
files_DATA = \
module_icon.png

EXTRA_DIST = $(files_DATA)

# the module .so file
INCLUDES               = -I. \
			 -I$(top_srcdir) \
			 -I$(includedir) \
			 @exml_cflags@ \
			 @e_cflags@
			 
pkgdir                 = $(datadir)/$(MODULE_ARCH)

pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
		         e_mod_main.h
			 
module_la_LIBADD       = @exml_libs@ @e_libs@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h
