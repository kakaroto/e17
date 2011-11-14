ACLOCAL_AMFLAGS = -I m4

cython_ins = elementary/elementary.c_elementary.pyx
cython_outs = elementary/elementary.c_elementary.c
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
	elementary/elementary.c_elementary_anchorblock.pxi \
	elementary/elementary.c_elementary_anchorview.pxi \
	elementary/elementary.c_elementary_background.pxi \
	elementary/elementary.c_elementary_box.pxi \
	elementary/elementary.c_elementary_bubble.pxi \
	elementary/elementary.c_elementary_button.pxi \
	elementary/elementary.c_elementary_check.pxi \
	elementary/elementary.c_elementary_clock.pxi \
	elementary/elementary.c_elementary_entry.pxi \
	elementary/elementary.c_elementary_fileselector.pxi \
	elementary/elementary.c_elementary_fileselector_entry.pxi \
	elementary/elementary.c_elementary_flip.pxi \
	elementary/elementary.c_elementary_frame.pxi \
	elementary/elementary.c_elementary_genlist.pxi \
	elementary/elementary.c_elementary_gengrid.pxi \
	elementary/elementary.c_elementary_hover.pxi \
	elementary/elementary.c_elementary_hoversel.pxi \
	elementary/elementary.c_elementary_icon.pxi \
	elementary/elementary.c_elementary_image.pxi \
	elementary/elementary.c_elementary_innerwindow.pxi \
	elementary/elementary.c_elementary_label.pxi \
	elementary/elementary.c_elementary_layout.pxi \
	elementary/elementary.c_elementary_list.pxi \
	elementary/elementary.c_elementary_menu.pxi \
	elementary/elementary.c_elementary_notify.pxi \
	elementary/elementary.c_elementary_object.pxi \
	elementary/elementary.c_elementary_pager.pxi \
	elementary/elementary.c_elementary_panel.pxi \
	elementary/elementary.c_elementary_photo.pxi \
	elementary/elementary.c_elementary_progressbar.pxi \
	elementary/elementary.c_elementary_radio.pxi \
	elementary/elementary.c_elementary_scrolled_entry.pxi \
	elementary/elementary.c_elementary_scroller.pxi \
	elementary/elementary.c_elementary_separator.pxi \
	elementary/elementary.c_elementary_slider.pxi \
	elementary/elementary.c_elementary_spinner.pxi \
	elementary/elementary.c_elementary_table.pxi \
	elementary/elementary.c_elementary_toggle.pxi \
	elementary/elementary.c_elementary_toolbar.pxi \
	elementary/elementary.c_elementary_widget_item.pxi \
	elementary/elementary.c_elementary_window.pxi

# elementary tests are not actual unit tests but a test AND a nice example
examples_files = \
	tests/test_anchorblock.py \
	tests/test_anchorview.py \
	tests/test_bg.py \
	tests/test_box.py \
	tests/test_button.py \
	tests/test_check.py \
	tests/test_clock.py \
	tests/test.edj \
	tests/cursors.edj \
	tests/test_entry.py \
	tests/test_fileselector.py \
	tests/test_flip.py \
	tests/test_genlist.py \
	tests/test_gengrid.py \
	tests/test_hover.py \
	tests/test_hoversel.py \
	tests/test_icon.py \
	tests/test_inwin.py \
	tests/test_layout.py \
	tests/test_list.py \
	tests/test_menu.py \
	tests/test_notify.py \
	tests/test_pager.py \
	tests/test_panel.py \
	tests/test_progressbar.py \
	tests/test.py \
	tests/test_radio.py \
	tests/test_scroller.py \
	tests/test_separator.py \
	tests/test_spinner.py \
	tests/test_table.py \
	tests/test_toggle.py \
	tests/test_toolbar.py \
	tests/test_tooltip.py
examples_images_files = \
	tests/images/icon_00.png \
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
	tests/images/wood_01.jpg

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
	find . -type f -not -name 'elementary.c_elementary.c'

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = python-elementary.pc

if HAVE_CYTHON
BUILT_SOURCES = $(cython_outs)
$(top_builddir)/elementary/elementary.c_elementary.c: $(top_srcdir)/elementary/elementary.c_elementary.pyx $(c_elementary_deps) $(c_elementary_includes) Makefile
	@$(MKDIR_P) $(top_builddir)/elementary
	$(AM_V_GEN)cd $(top_builddir) && $(PYTHON) -m cython $(CYTHON_ELEMENTARY_FLAGS) -I$(top_srcdir)/include $(CYTHONFLAGS) -o $@ $<
else
$(top_builddir)/elementary/elementary.c_elementary.c: Makefile
	@echo "NOTE: No cython, using pre-generated elementary.c_elementary.[ch]"
	@$(MKDIR_P) $(top_builddir)/elementary
	$(AM_V_GEN)for f in elementary/elementary.c_elementary.c; do if ! cmp $(top_srcdir)/$$f $(top_srcdir)/$$f > /dev/null; then cp $(top_srcdir)/$$f $(top_builddir)/$$f; fi; done
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
c_elementary_la_SOURCES = $(top_builddir)/elementary/elementary.c_elementary.c

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
