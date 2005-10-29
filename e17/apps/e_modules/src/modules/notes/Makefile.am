MAINTAINERCLEANFILES = Makefile.in

# data files for the module
filesdir = $(libdir)/e_modules/notes
files_DATA = \
module_icon.png

EXTRA_DIST = $(files_DATA)

# the module .so file
INCLUDES               = -I. \
			 -I$(top_srcdir) \
			 -I$(includedir) \
			 @esmart_cflags@ \
			 @e_cflags@
pkgdir                 = @e_modules@/notes/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
		         e_mod_main.h
#module_la_LIBADD       = @e_libs@ -lm @esmart_libs@ -lesmart_textarea
module_la_LIBADD       = @e_libs@ -lm @esmart_libs@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h
