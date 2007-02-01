MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = default.edc colors.edc
SUBDIRS = fonts \
	  images \
	  macros \
	  widgets
	  
EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/themes/default/images \
-fd $(top_srcdir)/data/themes/default/fonts

filesdir = $(pkgdatadir)/themes
files_DATA = default.edj

THEME_DEP = \
default.edc \
colors.edc \
widgets/button.edc \
widgets/check_button.edc \
widgets/colorpicker.edc \
widgets/combobox.edc \
widgets/entry.edc \
widgets/frame.edc \
widgets/iconbox.edc \
widgets/menu.edc \
widgets/notebook.edc \
widgets/paned.edc \
widgets/progress_bar.edc \
widgets/radio_button.edc \
widgets/scrollbar.edc \
widgets/separator.edc \
widgets/slider.edc \
widgets/spinner.edc \
widgets/statusbar.edc \
widgets/text_view.edc \
widgets/toggle_button.edc \
widgets/tooltips.edc \
widgets/tree.edc \
widgets/label.edc \
widgets/windows.edc

default.edj: Makefile $(THEME_DEP)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default/default.edc \
	$(top_builddir)/data/themes/default/default.edj

clean-local:
	rm -f *.edj
	rm -rf autom4te.cache
