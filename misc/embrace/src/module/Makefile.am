## $Id$

MAINTAINERCLEANFILES = Makefile.in
MODULE = embrace

if BUILD_MODULE

# data files for the module
filesdir = $(libdir)/$(MODULE)/module
#files_DATA = \
#module_icon.png

EXTRA_DIST = $(files_DATA)

# the module .so file
INCLUDES               = -I. \
                         -I$(top_srcdir) \
                         -I$(includedir) \
                         @ENLIGHTENMENT_CFLAGS@
pkgdir                 = @e_module_dir@/$(MODULE)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
			 embrace.c \
			 embrace.h \
			 embrace_plugin.c \
			 embrace_plugin.h \
			 mailbox.c \
			 mailbox.h
module_la_LIBADD       = @ENLIGHTENMENT_LIBS@ -lm
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h
endif
