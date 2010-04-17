MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs

SUBDIRS = src

EDJE_FLAGS = -v \
	     -id $(top_srcdir)/images

filesdir = $(datadir)
files_DATA = e-module-mpdule.edj module.desktop mpdule.edj

EXTRA_DIST = module.desktop.in \
	     e_modules-mpdule.spec.in \
	     e-module-mpdule.edc \
	     mpdule.edc \
	     $(wildcard images/*.png)

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf e-module-mpdule.edj mpdule.edj module.desktop e_modules-mpdule.spec *~

uninstall:
	 rm -rf $(DESTDIR)$(datadir)
