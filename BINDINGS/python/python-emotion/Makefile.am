ACLOCAL_AMFLAGS = -I m4

cython_ins = emotion/emotion.c_emotion.pyx
cython_outs = emotion/emotion.c_emotion.c
CYTHONFLAGS = @CYTHONFLAGS@
CYTHON_EMOTION_FLAGS = -I@CYTHON_EMOTION_INCLUDEDIR@
EPYDOC = @EPYDOC@

c_emotion_includes = include/emotion/c_emotion.pxd include/emotion/__init__.py

# emotion tests are not actual unit tests but a test AND a nice example
examples_files = examples/emotion_test.py

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
	python-emotion.pc.in \
	$(cython_ins) $(cython_outs) \
	$(c_emotion_includes) \
	$(examples_files) \
	$(tests_files)

# do not distcleancheck for generated emotion.c_emotion.c
distcleancheck_listfiles = \
	find . -type f -not -name 'emotion.c_emotion.c'

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = python-emotion.pc

if HAVE_CYTHON
BUILT_SOURCES = $(cython_outs)
$(top_builddir)/emotion/emotion.c_emotion.c: $(top_srcdir)/emotion/emotion.c_emotion.pyx $(c_emotion_includes) Makefile
	@$(MKDIR_P) $(top_builddir)/emotion
	$(AM_V_GEN)cd $(top_builddir) && $(PYTHON) -m cython $(CYTHON_EMOTION_FLAGS) -I$(top_srcdir)/include $(CYTHONFLAGS) -o $@ $<
else
$(top_builddir)/emotion/emotion.c_emotion.c: Makefile
	@echo "NOTE: No cython, using pre-generated emotion.c_emotion.[ch]"
	@$(MKDIR_P) $(top_builddir)/emotion
	$(AM_V_GEN)for f in emotion/emotion.c_emotion.c; do if ! cmp $(top_srcdir)/$$f $(top_srcdir)/$$f > /dev/null; then cp $(top_srcdir)/$$f $(top_builddir)/$$f; fi; done
endif

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/emotion \
	-I$(top_srcdir)/include \
	@PYTHON_INCLUDES@ \
	@EMOTION_CFLAGS@

pyexecemotiondir =  $(pyexecdir)/emotion
pyexecemotion_LTLIBRARIES = c_emotion.la
c_emotion_la_LIBADD = @EMOTION_LIBS@
c_emotion_la_LDFLAGS = -module -avoid-version
c_emotion_la_LIBTOOLFLAGS = --tag=disable-static
c_emotion_la_SOURCES = $(top_builddir)/emotion/emotion.c_emotion.c

pyemotiondir = $(pythondir)/emotion
pyemotion_PYTHON = emotion/__init__.py


if WANT_EXAMPLES
examplesdir = $(pkgdatadir)/examples
examples_DATA = $(examples_files)
endif


.PHONY: doc doc-clean
if HAVE_EPYDOC
clean-local: doc-clean
doc-clean:
	rm -fr doc/api/html
doc: $(pyexecemotion_LTLIBRARIES) $(pyemotiondir_PYTHON)
	$(AM_V_GEN)PYTHONPATH="$$PWD:$$PWD/.libs:$$PYTHONPATH" \
		$(EPYDOC) --html -o doc/api/html \
			-n "@PACKAGE@ @VERSION@ API" \
			c_emotion
else
doc-clean:
	@echo "Install epydoc then run ./configure again"
doc:
	@echo "Install epydoc then run ./configure again"
endif
