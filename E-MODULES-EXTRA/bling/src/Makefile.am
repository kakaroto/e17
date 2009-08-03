MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I. \
	   -I$(top_srcdir) \
	   -I$(includedir) \
	   @E_CFLAGS@ @x_cflags@

pkgdir = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES = module.la
module_la_SOURCES = compmgr.c \
                    compmgr.h \
		    e_mod_main.c \
		    e_mod_main.h \
		    e_mod_config.c \
		    e_mod_config.h

module_la_LIBADD = @E_LIBS@ @x_libs@
module_la_LDFLAGS = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

clean-local:
	rm -rf *~
