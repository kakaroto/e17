ACLOCAL_AMFLAGS = -I m4
SUBDIRS = po themes

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 ABOUT-NLS config.h.in \
   config.guess config.rpath config.sub configure depcomp ltmain.sh \
   install-sh missing mkinstalldirs m4

# data files for the module
filesdir = $(datadir)
files_DATA = \
module.desktop \
e-module-penguins.edj

EXTRA_DIST = $(files_DATA) e_modules-penguins.spec

# the module .so file
INCLUDES               = -I. \
			 -I$(top_srcdir) \
			 -I$(includedir) \
			 @E_CFLAGS@
pkgdir                 = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
		         e_mod_main.h \
		         e_mod_config.c \
		         e_mod_config.h
module_la_LIBADD       = @E_LIBS@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h
