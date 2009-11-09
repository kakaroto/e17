ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs

SUBDIRS = src data

EDJE_FLAGS = -v \
	     -id $(top_srcdir)/data/images

filesdir = $(datadir)
files_DATA = module.desktop weather.edj
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = eweather.pc

EXTRA_DIST = module.desktop.in \
	     weather.edc

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -rf weather.edj module.desktop *~

uninstall:
	rm -rf $(DESTDIR)$(datadir)
