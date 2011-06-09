ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs

SUBDIRS = src data po

EDJE_FLAGS = -v \
	     -id $(top_srcdir)

filesdir = $(datadir)
files_DATA = module_icon.png module.desktop e-module-itask.edj

EXTRA_DIST = module.desktop.in \
	     module_icon.png \
	     e-module-itask.edc

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -rf e-module-itask.edj module.desktop *~

uninstall:
	rm -rf $(DESTDIR)$(datadir)

