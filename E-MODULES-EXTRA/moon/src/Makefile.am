MAINTAINERCLEANFILES = Makefile.in

# the module .so file
AM_CPPFLAGS            = -I. \
			 -I$(top_srcdir) \
			 -I$(includedir) \
			 @e_cflags@

pkgdir                 = $(datadir)/$(MODULE_ARCH)

pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
		         e_mod_main.h \
			 e_mod_config.c \
			 CalcEphem.c \
			 CalcEphem.h \
			 Moon.c
module_la_LIBADD       = @e_libs@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

