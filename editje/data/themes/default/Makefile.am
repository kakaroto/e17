MAINTAINERCLEANFILES = Makefile.in
SUBDIRS = images elm_images

EDJE_CC = @edje_cc@
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/themes/default/images \
-id $(top_srcdir)/data/themes/default/elm_images

filesdir = $(datadir)/editje/data/themes
files_DATA = \
default.edj

EXTRA_DIST = \
border.edc \
colors.edc \
default.edc \
desktop.edc \
elm_collapsable_list.edc \
elm_collapsable_scroller.edc \
elm_detailwidgets_button.edc \
elm_detailwidgets_scroller.edc \
elm_extensions.edc \
icons.edc \
images.edc \
list.edc \
popups.edc \
shadow.edc \
styles.edc \
timeline.edc \
timeline_transition.edc \
toolbar.edc \
buttons.edc

default.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default/default.edc \
	$(top_builddir)/data/themes/default/default.edj

clean-local:
	rm -f *.edj
