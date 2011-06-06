ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs

SUBDIRS = src data

if HAVE_PO

SUBDIRS += po

endif

EDJE_FLAGS = -v \
	     -id $(top_srcdir)/data/themes/images

filesdir = $(datadir)
files_DATA = e-module-language.edj module.desktop

EXTRA_DIST = module.desktop.in \
	     e_modules-language.spec.in \
	     e-module-language.edc \
	     data/themes/images/module_icon.png

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -rf e-module-language.edj module.desktop e_modules-language.spec *~

uninstall:
	rm -rf $(DESTDIR)$(datadir)
