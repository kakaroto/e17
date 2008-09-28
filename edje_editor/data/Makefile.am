MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = edje_editor.edc \
             blank.edc \
             edje_editor.desktop \
             images/backdrop.png \
             images/bg1.png \
             images/bg1_i.png \
             images/bg2.png \
             images/bg2_i.png \
             images/bg3.png \
             images/fopa100.png \
             images/fopa50.png \
             images/fopa25.png \
             images/frame_bg.png \
             images/frame_bg_label.png \
             images/lbl_group.png \
             images/lbl_desc.png \
             images/lbl_image.png \
             images/lbl_part.png \
             images/lbl_pos.png \
             images/lbl_prog.png \
             images/lbl_rect.png \
             images/lbl_script.png \
             images/lbl_text.png \
             images/lbl_grad.png \
             images/lbl_fill.png \
             images/logo.png \
             images/image.png \
             images/rect.png \
             images/text.png \
             images/grad_h.png \
             images/grad_linear.png \
             images/grad_radial.png \
             images/grad_rectangular.png \
             images/grad_angular.png \
             images/grad_sinusoidal.png \
             images/swallow.png \
             images/swallow2.png \
             images/spectra_arrow.png \
             images/spectra_icon.png \
             images/spectra_color.png \
             images/group.png \
             images/desc.png \
             images/e17.png \
             images/none.png \
             images/focus.png \
             images/fakewin.png \
             images/rel1.png \
             images/rel2.png \
             images/move.png \
             images/miniarrow.png \
             images/gimp.png \
             images/data_icon.png \
             images/colors_icon.png \
             edje_editor.png

SUBDIRS = fonts

EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/images \
-fd $(top_srcdir)/data/fonts

filesdir = $(pkgdatadir)
files_DATA = edje_editor.edj blank.edj

desktopdir   = $(datadir)/applications
desktop_DATA = edje_editor.desktop

#http://standards.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html
icondir = $(datadir)/icons/hicolor/48x48/apps
icon_DATA = edje_editor.png


edje_editor.edj: Makefile
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/edje_editor.edc \
	$(top_builddir)/data/edje_editor.edj

blank.edj: Makefile
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/blank.edc \
	$(top_builddir)/data/blank.edj

clean-local:
	rm -f *.edj
	rm -rf autom4te.cache
