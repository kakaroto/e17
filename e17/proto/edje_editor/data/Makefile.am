MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = edje_editor.edc
SUBDIRS = fonts \
	  images
	  
EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/images \
-fd $(top_srcdir)/data/fonts

filesdir = $(pkgdatadir)
files_DATA = edje_editor.edj

edje_editor.edj: Makefile
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/edje_editor.edc \
	$(top_builddir)/data/edje_editor.edj

clean-local:
	rm -f *.edj
	rm -rf autom4te.cache
