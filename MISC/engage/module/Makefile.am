MAINTAINERCLEANFILES = Makefile.in

# data files for the module
filesdir = $(datadir)
files_DATA = \
module.eap

EXTRA_DIST = $(files_DATA)

# the module .so file
INCLUDES               = -I. \
			 -I$(top_srcdir) \
			 -I$(includedir) \
			 @e_cflags@
pkgdir                 = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
                         tray.c \
                         e_mod_config.h \
                         e_mod_config.c \
                         e_mod_main.h
module_la_LIBADD       = @e_libs@ -lm
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

module.eap:
	@edje_cc@ -v module_icon.edc module.eap
	enlightenment_eapp module.eap \
	-set-name "Engage" \
	-set-generic "gadget" \
	-set-comment "Engage module for e17" \
	-set-exe "null" \
	-set-icon-class "null"
