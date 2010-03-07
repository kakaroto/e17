MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs

SUBDIRS = src po

EDJE_FLAGS = -v \
	     -id $(top_srcdir)/images

filesdir = $(datadir)
files_DATA = module.desktop e-module-tclock.edj tclock.edj

EXTRA_DIST = module.desktop.in \
	     e_modules-tclock.spec.in \
	     e-module-tclock.edc \
	     tclock.edc \
	     $(wildcard images/*.png)

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf e-module-tclock.edj tclock.edj module.desktop e_modules-tclock.spec *~

uninstall:
	 rm -rf $(DESTDIR)$(datadir)
