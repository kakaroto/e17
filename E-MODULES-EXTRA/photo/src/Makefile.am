MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I. \
	   -I$(top_srcdir) \
           -I$(includedir) \
	   @E_CFLAGS@

pkgdir = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES = module.la
module_la_SOURCES = $(wildcard *.c) \
		    $(wildcard *.h)

module_la_LIBADD = @E_LIBS@
module_la_LDFLAGS = -module -avoid-version -s
module_la_DEPENDENCIES = $(top_builddir)/config.h

clean-local:
	rm -rf *~
