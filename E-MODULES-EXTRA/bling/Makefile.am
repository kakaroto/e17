ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
                       config.sub configure depcomp install-sh ltmain.sh \
		       missing config.rpath mkinstalldirs

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

EDJE_CC = @EDJE_CC@
EDJE_FLAGS = -v \
-id $(top_srcdir)/images \
-fd $(top_srcdir)/fonts

filesdir = $(datadir)
files_DATA = module.desktop \
             e-module-bling.edj

EXTRA_DIST = $(files_DATA) \
             e_modules-bling.spec

%.edj: %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -rf *~

uninstall-hook:
	rm -rf $(datadir)

