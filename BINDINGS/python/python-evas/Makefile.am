ACLOCAL_AMFLAGS = -I m4

cython_ins = evas/evas.c_evas.pyx
cython_outs = evas/evas.c_evas.c evas/evas.c_evas.h
CYTHONFLAGS = @CYTHONFLAGS@
EPYDOC = @EPYDOC@

pyincevasdir = $(includedir)/@PACKAGE@/evas
c_evas_includes = \
	include/evas/c_evas.pxd \
	include/evas/__init__.py \
	include/evas/evas_object_image_python_extras.h
pyincevas_DATA = $(c_evas_includes) \
	evas/evas.c_evas.h

c_evas_deps = \
	evas/evas.c_evas_canvas_callbacks.pxi \
	evas/evas.c_evas_canvas.pxi \
	evas/evas.c_evas_object_box.pxi \
	evas/evas.c_evas_object_callbacks.pxi \
	evas/evas.c_evas_object_events.pxi \
	evas/evas.c_evas_object_image.pxi \
	evas/evas.c_evas_object_line.pxi \
	evas/evas.c_evas_object_polygon.pxi \
	evas/evas.c_evas_object.pxi \
	evas/evas.c_evas_object_rectangle.pxi \
	evas/evas.c_evas_object_smart.pxi \
	evas/evas.c_evas_object_textblock.pxi \
	evas/evas.c_evas_object_text.pxi \
	evas/evas.c_evas_rect.pxi \
	evas/evas.c_evas_map.pxi \
	include/evas/evas_object_image_python_extras.h

examples_files = \
	examples/01-smart_object.py

tests_files = \
	tests/01-canvas-basics.py \
	tests/02-object-basics.py \
	tests/03-rect.py \
	tests/04-object-image.py \
	tests/05-smart-object.py \
	tests/06-canvas-as-factory.py \
	tests/07-smart_object-as-factory.py \
	tests/08-object-polygon.py \
	tests/10-object_text.py \
	tests/11-decorators.py \
	tests/12-image_as_buffer.py \
	tests/13-image-rotate.py \
	tests/14-image-preload.py \
	tests/icon.png

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
	python-evas.pc.in \
	$(cython_ins) $(cython_outs) \
	$(pyincevas_DATA) \
	$(c_evas_deps) \
	$(examples_files) \
	$(tests_files)

# do not distcleancheck for generated evas.c_evas.[ch]
distcleancheck_listfiles = \
	find . -type f \
		\( -not -name 'evas.c_evas.c' -a \
		   -not -name 'evas.c_evas.h' \)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = python-evas.pc

if HAVE_CYTHON
BUILT_SOURCES = $(cython_outs)
$(top_builddir)/evas/evas.c_evas.c $(top_builddir)/evas/evas.c_evas.h: $(top_srcdir)/evas/evas.c_evas.pyx $(c_evas_deps) $(c_evas_includes) Makefile
	@$(MKDIR_P) $(top_builddir)/evas
	$(AM_V_GEN)cd $(top_builddir) && $(PYTHON) -m cython -I$(top_srcdir)/include $(CYTHONFLAGS) -o $@ $<
else
$(top_builddir)/evas/evas.c_evas.c $(top_builddir)/evas/evas.c_evas.h: Makefile
	@echo "NOTE: No cython, using pre-generated evas.c_evas.[ch]"
	@$(MKDIR_P) $(top_builddir)/evas
	$(AM_V_GEN)for f in evas/evas.c_evas.c evas/evas.c_evas.h; do if ! cmp $(top_srcdir)/$$f $(top_builddir)/$$f >/dev/null; then cp $(top_srcdir)/$$f $(top_builddir)/$$f; fi; done
endif

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/evas \
	-I$(top_srcdir)/include \
	@PYTHON_INCLUDES@ \
	@EVAS_CFLAGS@

pyexecevasdir =  $(pyexecdir)/evas
pyexecevas_LTLIBRARIES = c_evas.la
c_evas_la_LIBADD = @EVAS_LIBS@
c_evas_la_LDFLAGS = -module -avoid-version
c_evas_la_LIBTOOLFLAGS = --tag=disable-static
c_evas_la_SOURCES = \
	$(top_builddir)/evas/evas.c_evas.c \
	$(top_builddir)/evas/evas.c_evas.h \
	evas/evas_object_image_rotate.c \
	evas/evas_object_image_mask.c

pyevasdir = $(pythondir)/evas
pyevas_PYTHON = \
	evas/debug.py \
	evas/decorators.py \
	evas/__init__.py \
	evas/utils.py

if WANT_EXAMPLES
examplesdir = $(pkgdatadir)/examples
examples_DATA = $(examples_files)
endif


.PHONY: doc doc-clean
if HAVE_EPYDOC
clean-local: doc-clean
doc-clean:
	rm -fr doc/api/html
doc: $(pyexecevas_LTLIBRARIES) $(pyevasdir_PYTHON)
	$(AM_V_GEN)PYTHONPATH="$$PWD:$$PWD/.libs:$$PYTHONPATH" \
		$(EPYDOC) --html -o doc/api/html \
			-n "@PACKAGE@ @VERSION@ API" \
			c_evas \
			evas.debug \
			evas.decorators \
			evas.utils
else
doc-clean:
	@echo "Install epydoc then run ./configure again"
doc:
	@echo "Install epydoc then run ./configure again"
endif
