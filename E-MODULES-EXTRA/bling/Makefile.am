ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs

SUBDIRS = src

EDJE_FLAGS = -v \
	     -id $(top_srcdir)/images

filesdir = $(datadir)
files_DATA = e-module-bling.edj module.desktop

EXTRA_DIST = module.desktop.in \
             e_modules-bling.spec.in \
	     e-module-bling.edc \
	     images/module_icon.png

%.edj: %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -rf e-module-bling.edj module.desktop e_modules-bling.spec *~

uninstall:
	rm -rf $(DESTDIR)$(datadir)
