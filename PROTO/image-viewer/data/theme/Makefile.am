AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

EDJE_FLAGS = -v -id $(top_srcdir)/data/theme -fd $(top_srcdir)/data/theme

filesdir = $(datadir)/image-viewer/themes
files_DATA = default.edj

EXTRA_DIST = \
default.edc \
next.png \
prev.png \
bg.png \
shadow.png \
gadman_border.png \
gadman_frame.png

default.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/theme/default.edc \
	$(top_builddir)/data/theme/default.edj

clean-local:
	rm -f *.edj
