MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = etk_test.etk
	  
EVOLVE_CC = $(top_srcdir)/src/bin/evolve_cc

filesdir = $(pkgdatadir)/interfaces/etk_test
files_DATA = etk_test.eet

INTERFACE_DEP = \
etk_test.etk

etk_test.eet: Makefile $(INTERFACE_DEP)
	$(EVOLVE_CC) \
	-DETK_DEFAULT_THEME=@etk_prefix@/share/etk/themes/default.edj \
	$(top_srcdir)/data/interfaces/etk_test/etk_test.etk \
	$(top_builddir)/data/interfaces/etk_test/etk_test.eet

clean-local:
	rm -f *.eet
	rm -rf autom4te.cache
