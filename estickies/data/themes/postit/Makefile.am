MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = postit.edc
SUBDIRS = images

EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/themes/postit/images

filesdir = $(pkgdatadir)/themes
files_DATA = postit.edj

THEME_DEP = \
postit.edc

postit.edj: Makefile $(THEME_DEP)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/postit/postit.edc \
	$(top_builddir)/data/themes/postit/postit.edj
			
clean-local:
	rm -rf *.edj
	rm -rf autom4te.cache
