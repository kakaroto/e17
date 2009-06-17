SUBDIRS = other
MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = gui.edc \
				 images/e17_mini_button_shadow2.png \
				 images/e17_fileman_thumb_bg.png \
				 images/fit_to_window.png \
				 images/one_to_one.png \
				 images/zoom_in.png \
				 images/zoom_out.png \
				 images/tiles.png

EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/images

guidir = $(pkgdatadir)
gui_DATA = gui.edj

gui.edj: Makefile gui.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/gui.edc \
	$(top_builddir)/data/gui.edj

clean-local:
	rm -f *.edj
