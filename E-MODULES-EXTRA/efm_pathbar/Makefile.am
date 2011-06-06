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
files_DATA = module.desktop e-module-efm_pathbar.edj

EXTRA_DIST = module.desktop.in \
	     e_modules-efm_pathbar.spec.in \
	     images/bt_base1.png \
	     images/bt_base2.png \
	     images/bt_hilight.png \
	     images/bt_shine.png \
	     images/left_arrow.png \
	     images/module.png \
	     images/right_arrow.png \
	     e-module-efm_pathbar.edc

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -rf e-module-efm_pathbar.edj module.desktop e_modules-efm_pathbar.spec *~

uninstall:
	rm -rf $(DESTDIR)$(datadir)
