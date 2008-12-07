ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs

EDJE_FLAGS = -v -id images # -fd fonts

# data files for the module
filesdir =  $(datadir)

files_DATA = e-module-drawer.edj

EXTRA_DIST = $(files_DATA) \
	     e-module-drawer.edc

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf e-module-drawer.edj *~
uninstall:
	 rm -rf $(DESTDIR)$(datadir)

