MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = main.edc
SUBDIRS = img
	  
EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/themes/b_and_w/img \
-fd $(top_srcdir)/data/themes/b_and_w

filesdir = $(pkgdatadir)/themes
files_DATA = b_and_w.edj

THEME_DEP = main.edc

b_and_w.edj: Makefile $(THEME_DEP)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/b_and_w/main.edc \
	$(top_builddir)/data/themes/b_and_w/b_and_w.edj

clean-local:
	rm -f *.edj
	rm -rf autom4te.cache
