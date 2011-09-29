MAINTAINERCLEANFILES = Makefile.in
INCLUDES = -I. \
	   -I$(top_srcdir) \
	   -I$(includedir) \
	   @EDJE_CFLAGS@


pkgdir = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES = module.la
module_la_SOURCES = \
	eenvaders_evas_smart.c \
	eenvaders_edje_external.c

module_la_LIBADD = @EDJE_LIBS@
module_la_LDFLAGS = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

bin_PROGRAMS = eenvaders

eenvaders_SOURCES = \
	eenvaders_evas_smart.c \
	eenvaders.c
eenvaders_CPPFLAGS = \
	@EENVADER_CFLAGS@
eenvaders_LDADD = \
	@EENVADER_LIBS@

clean-local:
	rm -rf *~
