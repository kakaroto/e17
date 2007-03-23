MAINTAINERCLEANFILES = Makefile.in Makefile

SUBDIRS = bits fonts images

EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/themes/ephoto/images \
-fd $(top_srcdir)/data/themes/ephoto/fonts

themedir = $(pkgdatadir)/themes/
theme_DATA = \
ephoto.edj

EXTRA_DIST = \
	bits/button.edc \
	bits/combo.edc \
	bits/entry.edc \
	bits/label.edc \
	bits/scrollbar.edc \
	bits/scrollpane.edc \
	bits/select.edc \
	bits/spacer.edc \
	bits/tree.edc \
	bits/window.edc

ephoto.edj: Makefile $(EXTRA_DIST)
	edje_cc $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/ephoto/main.edc \
	$(top_builddir)/data/themes/ephoto/ephoto.edj

clean-local:
	rm -f *.edj
