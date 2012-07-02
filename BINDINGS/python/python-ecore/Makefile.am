ACLOCAL_AMFLAGS = -I m4

cython_dist = \
	ecore/ecore.c_ecore.pyx \
	ecore/x/ecore.x.c_ecore_x.pyx \
	ecore/x/ecore.x.screensaver.pyx \
	ecore/imf/ecore.imf.c_ecore_imf.pyx \
	ecore/file/ecore.file.c_ecore_file.pyx \
	ecore/win32/ecore.win32.c_ecore_win32.pyx \
	ecore/evas/ecore.evas.c_ecore_evas.pyx

cython_ins = ecore/ecore.c_ecore.pyx
cython_outs = ecore/ecore.c_ecore.c ecore/ecore.c_ecore.h
doc_modules = c_ecore

pyincecoredir = $(includedir)/@PACKAGE@/ecore
c_ecore_includes = \
	include/ecore/c_ecore.pxd \
	include/ecore/__init__.py
pyincecore_DATA = $(c_ecore_includes) \
	ecore/ecore.c_ecore.h

c_ecore_x_includes = \
	include/ecore/x/c_ecore_x.pxd \
	include/ecore/x/screensaver.pxd \
	include/ecore/x/__init__.py
if WANT_ECORE_X
cython_ins += ecore/x/ecore.x.c_ecore_x.pyx ecore/x/ecore.x.screensaver.pyx
cython_outs += ecore/x/ecore.x.c_ecore_x.c ecore/x/ecore.x.screensaver.c
doc_modules += c_ecore_x screensaver
pyincecorexdir = $(includedir)/@PACKAGE@/ecore/x
pyincecorex_DATA = $(c_ecore_x_includes)
endif

c_ecore_imf_includes = \
	include/ecore/imf/c_ecore_imf.pxd \
	include/ecore/imf/__init__.py
if WANT_ECORE_IMF
cython_ins += ecore/imf/ecore.imf.c_ecore_imf.pyx
cython_outs += ecore/imf/ecore.imf.c_ecore_imf.c
doc_modules += c_ecore_imf
pyincecoreimfdir = $(includedir)/@PACKAGE@/ecore/imf
pyincecoreimf_DATA = $(c_ecore_imf_includes)
endif

c_ecore_file_includes = \
	include/ecore/file/c_ecore_file.pxd \
	include/ecore/file/__init__.py
if WANT_ECORE_FILE
cython_ins += ecore/file/ecore.file.c_ecore_file.pyx
cython_outs += ecore/file/ecore.file.c_ecore_file.c
doc_modules += c_ecore_file
pyincecorefiledir = $(includedir)/@PACKAGE@/ecore/file
pyincecorefile_DATA = $(c_ecore_file_includes)
endif

c_ecore_win32_includes = \
	include/ecore/win32/c_ecore_win32.pxd \
	include/ecore/win32/__init__.py
if WANT_ECORE_WIN32
cython_ins += ecore/win32/ecore.win32.c_ecore_win32.pyx
cython_outs += ecore/win32/ecore.win32.c_ecore_win32.c
doc_modules += c_ecore_win32
pyincecorewin32dir = $(includedir)/@PACKAGE@/ecore/win32
pyincecorewin32_DATA = $(c_ecore_win32_includes)
endif

c_ecore_evas_includes = \
	include/ecore/evas/c_ecore_evas.pxd \
	include/ecore/evas/__init__.py
if WANT_ECORE_EVAS
cython_ins += ecore/evas/ecore.evas.c_ecore_evas.pyx
cython_outs += ecore/evas/ecore.evas.c_ecore_evas.c
doc_modules += c_ecore_evas ecore.evas.utils
pyincecoreevasdir = $(includedir)/@PACKAGE@/ecore/evas
pyincecoreevas_DATA = $(c_ecore_evas_includes)
endif

CYTHONFLAGS = @CYTHONFLAGS@
CYTHON_ECORE_EVAS_FLAGS = -I@CYTHON_ECORE_EVAS_INCLUDEDIR@
EPYDOC = @EPYDOC@

c_ecore_deps = \
	ecore/ecore.c_ecore_animator.pxi \
	ecore/ecore.c_ecore_events.pxi \
	ecore/ecore.c_ecore_exe.pxi \
	ecore/ecore.c_ecore_fd_handler.pxi \
	ecore/ecore.c_ecore_idle_enterer.pxi \
	ecore/ecore.c_ecore_idle_exiter.pxi \
	ecore/ecore.c_ecore_idler.pxi \
	ecore/ecore.c_ecore_timer.pxi

c_ecore_x_deps = \
	ecore/x/ecore.x.c_ecore_x_events.pxi \
	ecore/x/ecore.x.c_ecore_x_window.pxi

c_ecore_imf_deps =

c_ecore_file_deps = \
	ecore/file/ecore.file.c_ecore_file_download.pxi

c_ecore_win32_deps = \
	ecore/win32/ecore.win32.c_ecore_win32_window.pxi

c_ecore_evas_deps = \
	ecore/evas/ecore.evas.c_ecore_evas_base.pxi \
	ecore/evas/ecore.evas.c_ecore_evas_base_x11.pxi \
	ecore/evas/ecore.evas.c_ecore_evas_buffer.pxi \
	ecore/evas/ecore.evas.c_ecore_evas_directfb.pxi \
	ecore/evas/ecore.evas.c_ecore_evas_fb.pxi \
	ecore/evas/ecore.evas.c_ecore_evas_gl_x11.pxi \
	ecore/evas/ecore.evas.c_ecore_evas_software_x11_16.pxi \
	ecore/evas/ecore.evas.c_ecore_evas_software_x11.pxi \
	ecore/evas/ecore.evas.c_ecore_evas_xrender_x11.pxi

examples_files = \
	examples/01-timers.py \
	examples/02-animators.py

examples_ecore_evas_files = \
	examples/ecore_evas/00-empty.py \
	examples/ecore_evas/01-empty-resizable.py \
	examples/ecore_evas/02-centered-rectangle.py \
	examples/ecore_evas/03-image.py \
	examples/ecore_evas/04-animation-moving.py \
	examples/ecore_evas/05-new.py \
	examples/ecore_evas/icon.png

examples_ecore_x_files = \
	examples/x/mplayer_embed.py \
	examples/x/window_creation.py

examples_ecore_file_files =
	examples/file/01-file-download.py

tests_files = \
	tests/01-basics.py \
	tests/02-timer.py \
	tests/03-animator.py \
	tests/04-idler.py \
	tests/05-idle_enterer.py \
	tests/06-idle_exiter.py \
	tests/07-fd_handler.py \
	tests/08-exe.py

tests_ecore_evas_files = \
	tests/ecore-evas/01-basics.py \
	tests/ecore-evas/01-basics-software_x11.py

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
	python-ecore.pc.in \
	python-ecore-x.pc.in \
	python-ecore-imf.pc.in \
	python-ecore-file.pc.in \
	python-ecore-win32.pc.in \
	python-ecore-evas.pc.in \
	$(cython_dist) $(cython_outs) \
	$(pyincecore_DATA) \
	$(c_ecore_x_includes) \
	$(c_ecore_imf_includes) \
	$(c_ecore_file_includes) \
	$(c_ecore_win32_includes) \
	$(c_ecore_evas_includes) \
	$(c_ecore_deps) \
	$(c_ecore_x_deps) \
	$(c_ecore_imf_deps) \
	$(c_ecore_file_deps) \
	$(c_ecore_win32_deps) \
	$(c_ecore_evas_deps) \
	$(examples_files) \
	$(examples_ecore_evas_files) \
	$(examples_ecore_x_files) \
	$(examples_ecore_file_files) \
	$(tests_files) \
	$(tests_ecore_evas_files)

# do not distcleancheck for generated ecore.c_ecore.[ch]
distcleancheck_listfiles = \
	find . -type f \
		\( -not -name 'ecore.c_ecore.c' -a \
		   -not -name 'ecore.c_ecore.h' -a \
		   -not -name 'ecore.x.c_ecore_x.c' -a \
		   -not -name 'ecore.x.screensaver.c' -a \
		   -not -name 'ecore.imf.c_ecore_imf.c' -a \
		   -not -name 'ecore.file.c_ecore_file.c' -a \
		   -not -name 'ecore.win32.c_ecore_win32.c' -a \
		   -not -name 'ecore.evas.c_ecore_evas.c' \)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = python-ecore.pc

if HAVE_CYTHON
BUILT_SOURCES = $(cython_outs)
$(top_builddir)/ecore/ecore.c_ecore.c $(top_builddir)/ecore/ecore.c_ecore.h: $(top_srcdir)/ecore/ecore.c_ecore.pyx $(c_ecore_deps) $(c_ecore_includes) Makefile
	@$(MKDIR_P) $(top_builddir)/ecore
	$(AM_V_GEN)cd $(top_builddir) && $(PYTHON) -m cython -I$(top_srcdir)/include $(CYTHONFLAGS) -o $@ $<

$(top_builddir)/ecore/x/ecore.x.c_ecore_x.c $(top_builddir)/ecore/x/ecore.x.screensaver.c: $(top_srcdir)/ecore/x/ecore.x.c_ecore_x.pyx $(top_srcdir)/ecore/x/ecore.x.screensaver.pyx $(c_ecore_x_deps) $(pyincecorex_DATA) Makefile
	@$(MKDIR_P) $(top_builddir)/ecore/x
	$(AM_V_GEN)cd $(top_builddir) && $(PYTHON) -m cython -I$(top_srcdir)/include $(CYTHONFLAGS) -o $@ $<

$(top_builddir)/ecore/imf/ecore.imf.c_ecore_imf.c: $(top_srcdir)/ecore/imf/ecore.imf.c_ecore_imf.pyx $(c_ecore_imf_deps) $(pyincecoreimf_DATA) Makefile
	@$(MKDIR_P) $(top_builddir)/ecore/imf
	$(AM_V_GEN)cd $(top_builddir) && $(PYTHON) -m cython -I$(top_srcdir)/include $(CYTHONFLAGS) -o $@ $<

$(top_builddir)/ecore/file/ecore.file.c_ecore_file.c: $(top_srcdir)/ecore/file/ecore.file.c_ecore_file.pyx $(c_ecore_file_deps) $(pyincecorefile_DATA) Makefile
	@$(MKDIR_P) $(top_builddir)/ecore/file
	$(AM_V_GEN)cd $(top_builddir) && $(PYTHON) -m cython -I$(top_srcdir)/include $(CYTHONFLAGS) -o $@ $<

$(top_builddir)/ecore/win32/ecore.win32.c_ecore_win32.c: $(top_srcdir)/ecore/win32/ecore.win32.c_ecore_win32.pyx $(c_ecore_win32_deps) $(pyincecorewin32_DATA) Makefile
	@$(MKDIR_P) $(top_builddir)/ecore/win32
	$(AM_V_GEN)cd $(top_builddir) && $(PYTHON) -m cython -I$(top_srcdir)/include $(CYTHONFLAGS) -o $@ $<

$(top_builddir)/ecore/evas/ecore.evas.c_ecore_evas.c: $(top_srcdir)/ecore/evas/ecore.evas.c_ecore_evas.pyx $(c_ecore_evas_deps) $(pyincecoreevas_DATA) Makefile
	@$(MKDIR_P) $(top_builddir)/ecore/evas
	$(AM_V_GEN)cd $(top_builddir) && $(PYTHON) -m cython $(CYTHON_ECORE_EVAS_FLAGS) -I$(top_srcdir)/include $(CYTHONFLAGS) -o $@ $<

else
$(top_builddir)/ecore/win32/ecore.win32.c_ecore_win32.c:
	@echo "Unfortunately you need cython to generate your own $^"
	/bin/false

$(top_builddir)/ecore/ecore.c_ecore.c $(top_builddir)/ecore/ecore.c_ecore.h $(top_builddir)/ecore/x/ecore.x.c_ecore_x.c $(top_builddir)/ecore/x/ecore.x.screensaver.c $(top_builddir)/ecore/imf/ecore.imf.c_ecore_imf.c $(top_builddir)/ecore/file/ecore.file.c_ecore_file.c $(top_builddir)/ecore/evas/ecore.evas.c_ecore_evas.c: Makefile
	@echo "NOTE: No cython, using pre-generated ecore.c_ecore.[ch]"
	@$(MKDIR_P) $(top_builddir)/ecore
	$(AM_V_GEN)for f in ecore/ecore.c_ecore.c ecore/ecore.c_ecore.h ecore/x/ecore.x.c_ecore_x.c ecore/x/ecore.x.screensaver.c ecore/imf/ecore.imf.c_ecore_imf.c ecore/file/ecore.file.c_ecore_file.c ecore/evas/ecore.evas.c_ecore_evas.c; do if ! test -f $(top_srcdir)/$$f; then continue; elif ! cmp $(top_srcdir)/$$f $(top_srcdir)/$$f >/dev/null; then cp $(top_srcdir)/$$f $(top_builddir)/$$f; fi; done
endif

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/ecore \
	-I$(top_srcdir)/ecore/x \
	-I$(top_srcdir)/ecore/imf \
	-I$(top_srcdir)/ecore/file \
	-I$(top_srcdir)/ecore/win32 \
	-I$(top_srcdir)/ecore/evas \
	-I$(top_srcdir)/include \
	-I@CYTHON_ECORE_EVAS_INCLUDEDIR@ \
	@PYTHON_INCLUDES@ \
	@ECORE_CFLAGS@ \
	@ECORE_X_CFLAGS@ \
	@ECORE_IMF_CFLAGS@ \
	@ECORE_FILE_CFLAGS@ \
	@ECORE_WIN32_CFLAGS@ \
	@ECORE_EVAS_CFLAGS@

pyexececoredir = $(pyexecdir)/ecore
pyexececore_LTLIBRARIES = c_ecore.la
c_ecore_la_LIBADD = @ECORE_LIBS@
c_ecore_la_LDFLAGS = -module -avoid-version
c_ecore_la_LIBTOOLFLAGS = --tag=disable-static
c_ecore_la_SOURCES = \
	$(top_builddir)/ecore/ecore.c_ecore.c \
	$(top_builddir)/ecore/ecore.c_ecore.h

pyecoredir = $(pythondir)/ecore
pyecore_PYTHON = ecore/__init__.py

if WANT_ECORE_X
pyexececorexdir = $(pyexecdir)/ecore/x
pyexececorex_LTLIBRARIES = c_ecore_x.la
c_ecore_x_la_LIBADD = @ECORE_X_LIBS@
c_ecore_x_la_LDFLAGS = -module -avoid-version
c_ecore_x_la_LIBTOOLFLAGS = --tag=disable-static
c_ecore_x_la_SOURCES = $(top_builddir)/ecore/x/ecore.x.c_ecore_x.c

pyecorexdir = $(pythondir)/ecore/x
pyecorex_PYTHON = ecore/x/__init__.py

pkgconfig_DATA += python-ecore-x.pc
endif

if WANT_ECORE_IMF
pyexececoreimfdir = $(pyexecdir)/ecore/imf
pyexececoreimf_LTLIBRARIES = c_ecore_imf.la
c_ecore_imf_la_LIBADD = @ECORE_IMF_LIBS@
c_ecore_imf_la_LDFLAGS = -module -avoid-version
c_ecore_imf_la_LIBTOOLFLAGS = --tag=disable-static
c_ecore_imf_la_SOURCES = $(top_builddir)/ecore/imf/ecore.imf.c_ecore_imf.c

pyecoreimfdir = $(pythondir)/ecore/imf
pyecoreimf_PYTHON = ecore/imf/__init__.py

pkgconfig_DATA += python-ecore-imf.pc
endif

if WANT_ECORE_FILE
pyexececorefiledir = $(pyexecdir)/ecore/file
pyexececorefile_LTLIBRARIES = c_ecore_file.la
c_ecore_file_la_LIBADD = @ECORE_FILE_LIBS@
c_ecore_file_la_LDFLAGS = -module -avoid-version
c_ecore_file_la_LIBTOOLFLAGS = --tag=disable-static
c_ecore_file_la_SOURCES = $(top_builddir)/ecore/file/ecore.file.c_ecore_file.c

pyecorefiledir = $(pythondir)/ecore/file
pyecorefile_PYTHON = ecore/file/__init__.py

pkgconfig_DATA += python-ecore-file.pc
endif

if WANT_ECORE_WIN32
pyexececorewin32dir = $(pyexecdir)/ecore/win32
pyexececorewin32_LTLIBRARIES = c_ecore_win32.la
c_ecore_win32_la_LIBADD = @ECORE_WIN32_LIBS@
c_ecore_win32_la_LDFLAGS = -module -avoid-version
c_ecore_win32_la_LIBTOOLFLAGS = --tag=disable-static
c_ecore_win32_la_SOURCES = $(top_builddir)/ecore/win32/ecore.win32.c_ecore_win32.c

pyecorewin32dir = $(pythondir)/ecore/win32
pyecorewin32_PYTHON = ecore/win32/__init__.py

pkgconfig_DATA += python-ecore-win32.pc
endif

if WANT_ECORE_EVAS
pyexececoreevasdir = $(pyexecdir)/ecore/evas
pyexececoreevas_LTLIBRARIES = c_ecore_evas.la
c_ecore_evas_la_LIBADD = @ECORE_EVAS_LIBS@
c_ecore_evas_la_LDFLAGS = -module -avoid-version
c_ecore_evas_la_LIBTOOLFLAGS = --tag=disable-static
c_ecore_evas_la_SOURCES = $(top_builddir)/ecore/evas/ecore.evas.c_ecore_evas.c

pyecoreevasdir = $(pythondir)/ecore/evas
pyecoreevas_PYTHON = ecore/evas/__init__.py

pkgconfig_DATA += python-ecore-evas.pc
endif


if WANT_EXAMPLES
examplesdir = $(pkgdatadir)/examples
examples_DATA = $(examples_files)

if WANT_ECORE_X
examplesecorexdir = $(pkgdatadir)/examples/x
examplesecorex_DATA = $(examples_ecore_x_files)
endif

if WANT_ECORE_EVAS
examplesecoreevasdir = $(pkgdatadir)/examples/ecore-evas
examplesecoreevas_DATA = $(examples_ecore_evas_files)
endif
endif


.PHONY: doc doc-clean
if HAVE_EPYDOC
clean-local: doc-clean
doc-clean:
	rm -fr doc/api/html
doc: $(pkgpyexec_LTLIBRARIES) $(pkgpython_PYTHON)
	$(AM_V_GEN)PYTHONPATH="$$PWD:$$PWD/.libs:$$PYTHONPATH" \
		$(EPYDOC) --html -o doc/api/html \
			-n "@PACKAGE@ @VERSION@ API" \
			$(doc_modules)
else
doc-clean:
	@echo "Install epydoc then run ./configure again"
doc:
	@echo "Install epydoc then run ./configure again"
endif
