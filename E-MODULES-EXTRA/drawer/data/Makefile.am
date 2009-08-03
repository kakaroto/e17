ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in

EDJE_FLAGS = -v \
	     -id images

# data files for the module
filesdir =  $(datadir)

files_DATA = e-module-drawer.edj

EXTRA_DIST = e-module-drawer.edc \
	     $(wildcard images/*.png) \
	     $(wildcard images/*.svg)

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf e-module-drawer.edj *~

uninstall:
	 rm -rf $(DESTDIR)$(datadir)

