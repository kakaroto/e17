ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs

SUBDIRS = src images

if HAVE_PO

SUBDIRS += po

endif

EDJE_FLAGS = -v \
	     -id $(top_srcdir)/images

filesdir = $(datadir)
files_DATA = module.desktop e-module-weather.edj weather.edj

EXTRA_DIST = module.desktop.in \
	     e_modules-weather.spec.in \
	     e-module-weather.edc \
	     weather.edc

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -rf e-module-weather.edj weather.edj module.desktop e_modules-weather.spec *~

uninstall:
	rm -rf $(DESTDIR)$(datadir)
