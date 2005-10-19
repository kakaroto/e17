MAINTAINERCLEANFILES = Makefile.in

# data files for the module
filesdir = $(libdir)/e_modules/rain
files_DATA = \
cloud.png \
drop-s.png \
drop-m.png \
drop-l.png \
module_icon.png


EXTRA_DIST = $(files_DATA)

# the module .so file
INCLUDES               = -I. \
			 -I$(top_srcdir) \
			 -I$(includedir) \
			 @e_cflags@
pkgdir                 = @e_modules@/rain/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
		         e_mod_main.h
module_la_LIBADD       = @e_libs@ -lm
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h
