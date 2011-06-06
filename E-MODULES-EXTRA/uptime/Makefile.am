ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs

SUBDIRS = src

if HAVE_PO

SUBDIRS += po

endif

EDJE_FLAGS = -v \
	     -id $(top_srcdir)/images

filesdir = $(datadir)
files_DATA = uptime.edj module.desktop e-module-uptime.edj

EXTRA_DIST = module.desktop.in \
	     e_modules-uptime.spec.in \
	     e-module-uptime.edc \
	     uptime.edc \
	     images/module_icon.png

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -rf uptime.edj e-module-uptime.edj module.desktop e_modules-uptime.spec *~

uninstall:
	rm -rf $(DESTDIR)$(datadir)

