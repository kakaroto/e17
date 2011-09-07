AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in module.desktop

EDJE_FLAGS = -v \
	     -id $(top_srcdir)/data/images

filesdir = $(datadir)
files_DATA = e-module-e-tiling.edj module.desktop

EXTRA_DIST = \
	     e-module-e-tiling.edc
	     module.desktop.in
	     images/module_icon.png

e-module-e-tiling.edj:  e-module-e-tiling.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf e-module-e-tiling.edj module.desktop *~

uninstall:
	 rm -rf $(DESTDIR)$(datadir)
