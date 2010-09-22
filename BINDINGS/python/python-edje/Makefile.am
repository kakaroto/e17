ACLOCAL_AMFLAGS = -I m4

cython_ins = edje/edje.c_edje.pyx edje/edit/c_edit.pyx
cython_outs = \
	edje/edje.c_edje.c edje/edje.c_edje.h \
	edje/edit/c_edit.c edje/edit/c_edit.h
CYTHONFLAGS = @CYTHONFLAGS@
CYTHON_EDJE_FLAGS = -I@CYTHON_EDJE_INCLUDEDIR@
EPYDOC = @EPYDOC@

pyincedjedir = $(includedir)/@PACKAGE@/edje
c_edje_includes = \
	include/edje/c_edje.pxd \
	include/edje/__init__.py
pyincedje_DATA = $(c_edje_includes) \
	edje/edje.c_edje.h

pyincedjeeditdir = $(includedir)/@PACKAGE@/edje/edit
c_edje_edit_includes = \
	include/edje/edit/__init__.py \
	include/edje/edit/c_edit.pxd
pyincedjeedit_DATA = $(c_edje_edit_includes) \
	edje/edit/c_edit.h

c_edje_deps = \
	edje/edje.c_edje_external.pxi \
	edje/edje.c_edje_message.pxi \
	edje/edje.c_edje_object.pxi

c_edje_edit_deps = \
	edje/edit/c_edit_object_color_class.pxi \
	edje/edit/c_edit_object_group.pxi \
	edje/edit/c_edit_object_part.pxi \
	edje/edit/c_edit_object_programs.pxi \
	edje/edit/c_edit_object.pxi \
	edje/edit/c_edit_object_state.pxi \
	edje/edit/c_edit_object_text_style.pxi

examples_files = \
	examples/01-swallow.py \
	examples/01-swallow.edc

examples_evas_demo_01_app_launcher = \
	examples/evas-demo/01-app_launcher/01-app_launcher.edc \
	examples/evas-demo/01-app_launcher/01-app_launcher.py \
	examples/evas-demo/01-app_launcher/audio_player.png \
	examples/evas-demo/01-app_launcher/background.jpeg \
	examples/evas-demo/01-app_launcher/image_viewer.png \
	examples/evas-demo/01-app_launcher/video_player.png \
	examples/evas-demo/01-app_launcher/web_browser.png

examples_evas_demo_02_vkbd = \
	examples/evas-demo/02-vkbd/02-vkbd.edc \
	examples/evas-demo/02-vkbd/02-vkbd.py \
	examples/evas-demo/02-vkbd/background.png \
	examples/evas-demo/02-vkbd/backspace.png \
	examples/evas-demo/02-vkbd/backspace-selected.png \
	examples/evas-demo/02-vkbd/keyboard-background.png \
	examples/evas-demo/02-vkbd/key-default.png \
	examples/evas-demo/02-vkbd/key-default-selected.png \
	examples/evas-demo/02-vkbd/key-special.png \
	examples/evas-demo/02-vkbd/key-special-selected.png \
	examples/evas-demo/02-vkbd/shift.png \
	examples/evas-demo/02-vkbd/shift-selected.png

examples_evas_demo_03_kinetic_list = \
	examples/evas-demo/03-kinetic_list/03-kinetic_list.edc \
	examples/evas-demo/03-kinetic_list/03-kinetic_list.py \
	examples/evas-demo/03-kinetic_list/background.png \
	examples/evas-demo/03-kinetic_list/listitem.png \
	examples/evas-demo/03-kinetic_list/thumb_1.jpg \
	examples/evas-demo/03-kinetic_list/thumb_2.jpg \
	examples/evas-demo/03-kinetic_list/thumb_3.jpg \
	examples/evas-demo/03-kinetic_list/thumb_4.jpg \
	examples/evas-demo/03-kinetic_list/thumb_5.jpg \
	examples/evas-demo/03-kinetic_list/thumb_6.jpg \
	examples/evas-demo/03-kinetic_list/thumb_7.jpg \
	examples/evas-demo/03-kinetic_list/thumb_8.jpg

tests_files = \
	tests/01-basics.py
	tests/02-decorators.py
	tests/theme.edc
	tests/theme.edj

tests_edit_files = \
	tests/edit/01-edje.py \
	tests/edit/02-group.py \
	tests/edit/03-parts.py \
	tests/edit/04-states.py \
	tests/edit/groups.edc \
	tests/edit/theme.edc


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
	python-edje.pc.in \
	$(cython_ins) $(cython_outs) \
	$(pyincedje_DATA) \
	$(pyincedjeedit_DATA) \
	$(c_edje_deps) \
	$(c_edje_edit_deps) \
	$(examples_files) \
	$(examples_evas_demo_01_app_launcher) \
	$(examples_evas_demo_02_vkbd) \
	$(examples_evas_demo_03_kinetic_list) \
	$(tests_files) \
	$(tests_edit_files)

# do not distcleancheck for generated edje.c_edje.[ch]
distcleancheck_listfiles = \
	find . -type f \
		\( -not -name 'edje.c_edje.c' -a \
		   -not -name 'edje.c_edje.h' -a \
		   -not -name 'c_edit.c' -a \
		   -not -name 'c_edit.h' \)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = python-edje.pc

if HAVE_CYTHON
BUILT_SOURCES = $(cython_outs)
$(top_builddir)/edje/edje.c_edje.c $(top_builddir)/edje/edje.c_edje.h: $(top_srcdir)/edje/edje.c_edje.pyx $(c_edje_deps) $(c_edje_includes) Makefile
	@$(MKDIR_P) $(top_builddir)/edje
	$(AM_V_GEN)cd $(top_builddir) && $(PYTHON) -m cython $(CYTHON_EDJE_FLAGS) -I$(top_srcdir)/include $(CYTHONFLAGS) -o $@ $<

$(top_builddir)/edje/edit/c_edit.c $(top_builddir)/edje/edit/c_edit.h: $(top_srcdir)/edje/edit/c_edit.pyx $(c_edje_edit_deps) $(c_edje_edit_includes) Makefile
	@$(MKDIR_P) $(top_builddir)/edje/edit
	$(AM_V_GEN)cd $(top_builddir) && $(PYTHON) -m cython $(CYTHON_EDJE_FLAGS) -I$(top_srcdir)/include $(CYTHONFLAGS) -o $@ $<
else
$(top_builddir)/edje/edje.c_edje.c $(top_builddir)/edje/edje.c_edje.h $(top_builddir)/edje/edit/c_edit.c $(top_builddir)/edje/edit/c_edit.h: Makefile
	@echo "NOTE: No cython, using pre-generated edje.c_edje.[ch]"
	@$(MKDIR_P) $(top_builddir)/edje/edit
	$(AM_V_GEN)for f in edje/edje.c_edje.c edje/edje.c_edje.h edje/edit/c_edit.c edje/edit/c_edit.h; do if ! cmp $(top_srcdir)/$$f $(top_srcdir)/$$f > /dev/null; then cp $(top_srcdir)/$$f $(top_builddir)/$$f; fi; done
endif

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/edje \
	-I$(top_srcdir)/edje/edit \
	-I$(top_srcdir)/include \
	@PYTHON_INCLUDES@ \
	@EDJE_CFLAGS@ \
	-DEDJE_EDIT_IS_UNSTABLE_AND_I_KNOW_ABOUT_IT

pyexecedjedir =  $(pyexecdir)/edje
pyexecedje_LTLIBRARIES = c_edje.la
c_edje_la_LIBADD = @EDJE_LIBS@
c_edje_la_LDFLAGS = -module -avoid-version
c_edje_la_LIBTOOLFLAGS = --tag=disable-static
c_edje_la_SOURCES = \
	$(top_builddir)/edje/edje.c_edje.c \
	$(top_builddir)/edje/edje.c_edje.h

pyedjedir = $(pythondir)/edje
pyedje_PYTHON = \
	edje/decorators.py \
	edje/__init__.py

pyexecedjeeditdir =  $(pyexecdir)/edje/edit
pyexecedjeedit_LTLIBRARIES = c_edit.la
c_edit_la_LIBADD = @EDJE_LIBS@
c_edit_la_LDFLAGS = -module -avoid-version
c_edit_la_LIBTOOLFLAGS = --tag=disable-static
c_edit_la_SOURCES = \
	$(top_builddir)/edje/edit/c_edit.c \
	$(top_builddir)/edje/edit/c_edit.h

pyedjeeditdir = $(pythondir)/edje/edit
pyedjeedit_PYTHON = edje/edit/__init__.py


if WANT_EXAMPLES
examplesdir = $(pkgdatadir)/examples
examples_DATA = $(examples_files)

examples01dir = $(examplesdir)/evas-demo/01-app_launcher
examples01_DATA = $(examples_evas_demo_01_app_launcher)

examples02dir = $(examplesdir)/evas-demo/02-vkbd
examples02_DATA = $(examples_evas_demo_02_vkbd)

examples03dir = $(examplesdir)/evas-demo/03-kinetic_list
examples03_DATA = $(examples_evas_demo_03_kinetic_list)
endif


.PHONY: doc doc-clean
if HAVE_EPYDOC
clean-local: doc-clean
doc-clean:
	rm -fr doc/api/html
doc: $(pyexecedje_LTLIBRARIES) $(pyedjedir_PYTHON)
	$(AM_V_GEN)PYTHONPATH="$$PWD:$$PWD/.libs:$$PYTHONPATH" \
		$(EPYDOC) --html -o doc/api/html \
			-n "@PACKAGE@ @VERSION@ API" \
			c_edje \
			edje.decorators \
			c_edje_edit
else
doc-clean:
	@echo "Install epydoc then run ./configure again"
doc:
	@echo "Install epydoc then run ./configure again"
endif
