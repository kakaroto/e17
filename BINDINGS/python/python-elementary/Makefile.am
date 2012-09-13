ACLOCAL_AMFLAGS = -I m4

cython_ins = elementary/elementary.c_elementary.pyx
cython_outs = \
	elementary/elementary.c_elementary.c \
	elementary/elementary.c_elementary.h
CYTHONFLAGS = @CYTHONFLAGS@
CYTHON_ELEMENTARY_FLAGS = -I@CYTHON_ELEMENTARY_INCLUDEDIR@
EPYDOC = @EPYDOC@

# python-elementary still does not export any public cpython/cython interfaces
# pyincelementarydir = $(includedir)/@PACKAGE@/elementary
# pyincelementary_DATA = ... elementary/elementary.c_elementary.h ...
c_elementary_includes = \
	include/elementary/c_elementary.pxd \
	include/elementary/__init__.py


c_elementary_deps = \
	elementary/elementary.c_elementary_actionslider.pxi \
	elementary/elementary.c_elementary_background.pxi \
	elementary/elementary.c_elementary_box.pxi \
	elementary/elementary.c_elementary_bubble.pxi \
	elementary/elementary.c_elementary_button.pxi \
	elementary/elementary.c_elementary_calendar.pxi \
	elementary/elementary.c_elementary_check.pxi \
	elementary/elementary.c_elementary_clock.pxi \
	elementary/elementary.c_elementary_colorselector.pxi \
	elementary/elementary.c_elementary_configuration.pxi \
	elementary/elementary.c_elementary_conformant.pxi \
	elementary/elementary.c_elementary_ctxpopup.pxi \
	elementary/elementary.c_elementary_dayselector.pxi \
	elementary/elementary.c_elementary_datetime.pxi \
	elementary/elementary.c_elementary_diskselector.pxi \
	elementary/elementary.c_elementary_entry.pxi \
	elementary/elementary.c_elementary_fileselector.pxi \
	elementary/elementary.c_elementary_fileselector_entry.pxi \
	elementary/elementary.c_elementary_fileselector_button.pxi \
	elementary/elementary.c_elementary_flip.pxi \
	elementary/elementary.c_elementary_flipselector.pxi \
	elementary/elementary.c_elementary_frame.pxi \
	elementary/elementary.c_elementary_genlist.pxi \
	elementary/elementary.c_elementary_gengrid.pxi \
	elementary/elementary.c_elementary_gesture_layer.pxi \
	elementary/elementary.c_elementary_grid.pxi \
	elementary/elementary.c_elementary_hover.pxi \
	elementary/elementary.c_elementary_hoversel.pxi \
	elementary/elementary.c_elementary_icon.pxi \
	elementary/elementary.c_elementary_image.pxi \
	elementary/elementary.c_elementary_index.pxi \
	elementary/elementary.c_elementary_innerwindow.pxi \
	elementary/elementary.c_elementary_label.pxi \
	elementary/elementary.c_elementary_layout.pxi \
	elementary/elementary.c_elementary_layout_class.pxi \
	elementary/elementary.c_elementary_list.pxi \
	elementary/elementary.c_elementary_map.pxi \
	elementary/elementary.c_elementary_mapbuf.pxi \
	elementary/elementary.c_elementary_menu.pxi \
	elementary/elementary.c_elementary_multibuttonentry.pxi \
	elementary/elementary.c_elementary_need.pxi \
	elementary/elementary.c_elementary_notify.pxi \
	elementary/elementary.c_elementary_object.pxi \
	elementary/elementary.c_elementary_object_item.pxi \
	elementary/elementary.c_elementary_naviframe.pxi \
	elementary/elementary.c_elementary_panel.pxi \
	elementary/elementary.c_elementary_panes.pxi \
	elementary/elementary.c_elementary_photo.pxi \
	elementary/elementary.c_elementary_photocam.pxi \
	elementary/elementary.c_elementary_plug.pxi \
	elementary/elementary.c_elementary_popup.pxi \
	elementary/elementary.c_elementary_progressbar.pxi \
	elementary/elementary.c_elementary_radio.pxi \
	elementary/elementary.c_elementary_scroller.pxi \
	elementary/elementary.c_elementary_segment_control.pxi \
	elementary/elementary.c_elementary_separator.pxi \
	elementary/elementary.c_elementary_slider.pxi \
	elementary/elementary.c_elementary_slideshow.pxi \
	elementary/elementary.c_elementary_spinner.pxi \
	elementary/elementary.c_elementary_table.pxi \
	elementary/elementary.c_elementary_theme.pxi \
	elementary/elementary.c_elementary_thumb.pxi \
	elementary/elementary.c_elementary_toolbar.pxi \
	elementary/elementary.c_elementary_transit.pxi \
	elementary/elementary.c_elementary_video.pxi \
	elementary/elementary.c_elementary_web.pxi \
	elementary/elementary.c_elementary_window.pxi

# elementary tests are not actual unit tests but a test AND a nice example
examples_files = \
	tests/test.py \
	tests/test.edj \
	tests/cursors.edj \
	tests/test_external.edj \
	tests/test_3d.py \
	tests/test_actionslider.py \
	tests/test_bg.py \
	tests/test_box.py \
	tests/test_button.py \
	tests/test_calendar.py \
	tests/test_check.py \
	tests/test_clock.py \
	tests/test_colorselector.py \
	tests/test_config.py \
	tests/test_ctxpopup.py \
	tests/test_datetime.py \
	tests/test_dayselector.py \
	tests/test_diskselector.py \
	tests/test_entry.py \
	tests/test_external.py \
	tests/test_fileselector.py \
	tests/test_flip.py \
	tests/test_flipselector.py \
	tests/test_frame.py \
	tests/test_genlist.py \
	tests/test_gengrid.py \
	tests/test_grid.py \
	tests/test_hover.py \
	tests/test_hoversel.py \
	tests/test_icon.py \
	tests/test_index.py \
	tests/test_inwin.py \
	tests/test_label.py \
	tests/test_layout.py \
	tests/test_list.py \
	tests/test_mapbuf.py \
	tests/test_map.py \
	tests/test_map2.py \
	tests/test_map3.py \
	tests/test_menu.py \
	tests/test_multibuttonentry.py \
	tests/test_naviframe.py \
	tests/test_notify.py \
	tests/test_panel.py \
	tests/test_panes.py \
	tests/test_photo.py \
	tests/test_popup.py \
	tests/test_progressbar.py \
	tests/test_radio.py \
	tests/test_scroller.py \
	tests/test_segment_control.py \
	tests/test_separator.py \
	tests/test_slideshow.py \
	tests/test_spinner.py \
	tests/test_table.py \
	tests/test_thumb.py \
	tests/test_toolbar.py \
	tests/test_tooltip.py \
	tests/test_video.py \
	tests/test_web.py \
	tests/test_win.py

examples_images_files = \
	tests/images/icon_00.png \
	tests/images/icon_01.png \
	tests/images/icon_02.png \
	tests/images/icon_03.png \
	tests/images/icon_04.png \
	tests/images/icon_05.png \
	tests/images/icon_06.png \
	tests/images/icon_07.png \
	tests/images/icon_08.png \
	tests/images/icon_09.png \
	tests/images/icon_10.png \
	tests/images/icon_11.png \
	tests/images/icon_12.png \
	tests/images/icon_13.png \
	tests/images/icon_14.png \
	tests/images/icon_15.png \
	tests/images/icon_16.png \
	tests/images/icon_17.png \
	tests/images/icon_18.png \
	tests/images/icon_19.png \
	tests/images/icon_20.png \
	tests/images/icon_21.png \
	tests/images/icon_22.png \
	tests/images/icon_23.png \
	tests/images/insanely_huge_test_image.jpg \
	tests/images/logo.png \
	tests/images/logo_small.png \
	tests/images/panel_01.jpg \
	tests/images/plant_01.jpg \
	tests/images/rock_01.jpg \
	tests/images/rock_02.jpg \
	tests/images/sky_01.jpg \
	tests/images/sky_02.jpg \
	tests/images/sky_03.jpg \
	tests/images/sky_04.jpg \
	tests/images/wood_01.jpg \
	tests/images/icon_right_arrow.png \
	tests/images/mystrale.jpg \
	tests/images/mystrale_2.jpg \
	tests/images/clo.png \
	tests/images/under.png \
	tests/images/sky.jpg

tests_files =


MAINTAINERCLEANFILES = \
	m4/libtool.m4 \
	m4/lt~obsolete.m4 \
	m4/ltoptions.m4 \
	m4/ltsugar.m4 \
	m4/ltversion.m4 \
	m4/libtool.m4 \
	ltmain.sh \
	depcomp \
	aclocal.m4 \
	config.guess \
	config.sub \
	configure \
	install-sh \
	Makefile.in \
	missing \
	py-compile \
        config.h.in \
	$(cython_outs)

EXTRA_DIST = \
	AUTHORS \
	COPYING \
	README \
	autogen.sh \
	python-elementary.pc.in \
	$(cython_ins) $(cython_outs) \
	$(c_elementary_includes) \
	$(c_elementary_deps) \
	$(examples_files) \
	$(examples_images_files) \
	$(tests_files)

# do not distcleancheck for generated elementary.c_elementary.[ch]
distcleancheck_listfiles = \
	find . -type f \
		\( -not -name 'elementary.c_elementary.c' -a \
		   -not -name 'elementary.c_elementary.h' \)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = python-elementary.pc

if HAVE_CYTHON
BUILT_SOURCES = $(cython_outs)
$(top_builddir)/elementary/elementary.c_elementary.c $(top_builddir)/elementary/elementary.c_elementary.h: $(top_srcdir)/elementary/elementary.c_elementary.pyx $(c_elementary_deps) $(c_elementary_includes) Makefile
	@$(MKDIR_P) $(top_builddir)/elementary
	$(AM_V_GEN)cd $(top_builddir) && $(PYTHON) -m cython $(CYTHON_ELEMENTARY_FLAGS) -I$(top_srcdir)/include $(CYTHONFLAGS) -o $@ $<
else
$(top_builddir)/elementary/elementary.c_elementary.c $(top_builddir)/elementary/elementary.c_elementary.h: Makefile
	@echo "NOTE: No cython, using pre-generated elementary.c_elementary.[ch]"
	@$(MKDIR_P) $(top_builddir)/elementary
	$(AM_V_GEN)for f in elementary/elementary.c_elementary.c elementary/elementary.c_elementary.h; do if ! cmp $(top_srcdir)/$$f $(top_srcdir)/$$f > /dev/null; then cp $(top_srcdir)/$$f $(top_builddir)/$$f; fi; done
endif

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/elementary \
	-I$(top_srcdir)/include \
	@PYTHON_INCLUDES@ \
	@ELEMENTARY_CFLAGS@

pyexecelementarydir =  $(pyexecdir)/elementary
pyexecelementary_LTLIBRARIES = c_elementary.la
c_elementary_la_LIBADD = @ELEMENTARY_LIBS@
c_elementary_la_LDFLAGS = -module -avoid-version
c_elementary_la_LIBTOOLFLAGS = --tag=disable-static
c_elementary_la_SOURCES = \
	$(top_builddir)/elementary/elementary.c_elementary.c \
	$(top_builddir)/elementary/elementary.c_elementary.h


pyelementarydir = $(pythondir)/elementary
pyelementary_PYTHON = elementary/__init__.py elementary/cursors.py


if WANT_EXAMPLES
examplesdir = $(pkgdatadir)/examples
examples_DATA = $(examples_files)

examplesimagesdir = $(examplesdir)/images
examplesimages_DATA = $(examples_images_files)
endif


.PHONY: doc doc-clean
if HAVE_EPYDOC
clean-local: doc-clean
doc-clean:
	rm -fr doc/api/html
doc: $(pyexecelementary_LTLIBRARIES) $(pyelementarydir_PYTHON)
	mkdir -p doc/api/html
	$(AM_V_GEN)PYTHONPATH="$$PWD:$$PWD/.libs:$$PYTHONPATH" \
		$(EPYDOC) --html -o doc/api/html \
			-n "@PACKAGE@ @VERSION@ API" \
			c_elementary
else
doc-clean:
	@echo "Install epydoc then run ./configure again"
doc:
	@echo "Install epydoc then run ./configure again"
endif
