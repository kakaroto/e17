MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = evolve.etk
	  
EVOLVE_CC = $(top_srcdir)/src/bin/evolve_cc

filesdir = $(pkgdatadir)/interfaces/evolve
files_DATA = evolve.eet

INTERFACE_DEP = \
evolve.etk

evolve.eet: Makefile $(INTERFACE_DEP)
	$(EVOLVE_CC) \
	-DETK_DEFAULT_THEME=@etk_prefix@/share/etk/themes/default.edj \
	$(top_srcdir)/data/interfaces/evolve/evolve.etk \
	$(top_builddir)/data/interfaces/evolve/evolve.eet

clean-local:
	rm -f *.eet
	rm -rf autom4te.cache
