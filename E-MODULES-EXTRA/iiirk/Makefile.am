MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs \
                       ABOUT-NLS

SUBDIRS = src po

EDJE_FLAGS = -v \
	     -id $(top_srcdir)/images

# data files for the module
filesdir =  $(datadir)

files_DATA = module.desktop \
	     e-module-iiirk.edj

EXTRA_DIST = $(files_DATA) \
	     images/module_icon.png

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -rf e-module-iiirk.edj module.desktop *~
uninstall:
	rm -rf $(DESTDIR)$(datadir)
	 
