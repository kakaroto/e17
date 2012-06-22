ACLOCAL_AMFLAGS = -I m4

cython_dist = ethumb/ethumb.c_ethumb.pyx ethumb/ethumb.client.pyx

cython_ins = ethumb/ethumb.c_ethumb.pyx
cython_outs = ethumb/ethumb.c_ethumb.c
doc_modules = c_ethumb

if WANT_ETHUMB_CLIENT
cython_ins += ethumb/ethumb.client.pyx
cython_outs += ethumb/ethumb.client.c
doc_modules += client
endif

CYTHONFLAGS = @CYTHONFLAGS@
CYTHON_ETHUMB_CLIENT_FLAGS = -I@CYTHON_ETHUMB_CLIENT_INCLUDEDIR@
EPYDOC = @EPYDOC@

c_ethumb_includes = include/ethumb/c_ethumb.pxd include/ethumb/__init__.py
c_ethumb_deps =
c_ethumb_client_includes = include/ethumb/client.pxd
c_ethumb_client_deps =

examples_files =
examples_ethumb_client_files = examples/01-simple.py

tests_files =
tests_ethumb_client_files =

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
	python-ethumb.pc.in \
	python-ethumb_client.pc.in \
	$(cython_dist) $(cython_outs) \
	$(c_ethumb_includes) \
	$(c_ethumb_deps) \
	$(c_ethumb_client_includes) \
	$(c_ethumb_client_deps) \
	$(examples_files) \
	$(examples_ethumb_client_files) \
	$(tests_files) \
	$(tests_ethumb_client_files)

# do not distcleancheck for generated ethumb.c_ethumb.[ch]
distcleancheck_listfiles = \
	find . -type f \
		\( -not -name 'ethumb.c_ethumb.c' -a \
		   -not -name 'ethumb.client.c' \)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = python-ethumb.pc

if HAVE_CYTHON
BUILT_SOURCES = $(cython_outs)
$(top_builddir)/ethumb/ethumb.c_ethumb.c $(top_builddir)/ethumb/ethumb.c_ethumb.h: $(top_srcdir)/ethumb/ethumb.c_ethumb.pyx $(c_ethumb_deps) $(c_ethumb_includes) Makefile
	@$(MKDIR_P) $(top_builddir)/ethumb
	$(AM_V_GEN)cd $(top_builddir) && $(PYTHON) -m cython -I$(top_srcdir)/include $(CYTHONFLAGS) -o $@ $<

$(top_builddir)/ethumb/ethumb.client.c: $(top_srcdir)/ethumb/ethumb.client.pyx $(c_ethumb_client_deps) $(c_ethumb_client_includes) Makefile
	@$(MKDIR_P) $(top_builddir)/ethumb
	$(AM_V_GEN)cd $(top_builddir) && $(PYTHON) -m cython -I$(top_srcdir)/include $(CYTHONFLAGS) -o $@ $<

else
$(top_builddir)/ethumb/ethumb.c_ethumb.c $(top_builddir)/ethumb/ethumb.client.c: Makefile
	@echo "NOTE: No cython, using pre-generated ethumb.c_ethumb.[ch]"
	@$(MKDIR_P) $(top_builddir)/ethumb
	$(AM_V_GEN)for f in ethumb/ethumb.c_ethumb.c ethumb/ethumb.client.c; do if ! test -f $(top_srcdir)/$$f; then continue; elif ! cmp $(top_srcdir)/$$f $(top_srcdir)/$$f >/dev/null; then cp $(top_srcdir)/$$f $(top_builddir)/$$f; fi; done
endif

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/ethumb \
	-I$(top_srcdir)/include \
	@PYTHON_INCLUDES@ \
	@ETHUMB_CFLAGS@ \
	@ETHUMB_CLIENT_CFLAGS@

pyexecethumbdir = $(pyexecdir)/ethumb
pyexecethumb_LTLIBRARIES = c_ethumb.la
c_ethumb_la_LIBADD = @ETHUMB_LIBS@
c_ethumb_la_LDFLAGS = -module -avoid-version
c_ethumb_la_LIBTOOLFLAGS = --tag=disable-static
c_ethumb_la_SOURCES = \
	$(top_builddir)/ethumb/ethumb.c_ethumb.c

pyethumbdir = $(pythondir)/ethumb
pyethumb_PYTHON = ethumb/__init__.py

if WANT_ETHUMB_CLIENT
pyexecethumbclientdir = $(pyexecdir)/ethumb/
pyexecethumbclient_LTLIBRARIES = client.la
client_la_LIBADD = @ETHUMB_CLIENT_LIBS@
client_la_LDFLAGS = -module -avoid-version
client_la_LIBTOOLFLAGS = --tag=disable-static
client_la_SOURCES = $(top_builddir)/ethumb/ethumb.client.c

pkgconfig_DATA += python-ethumb_client.pc
endif


if WANT_EXAMPLES
examplesdir = $(pkgdatadir)/examples
examples_DATA = $(examples_files)

if WANT_ETHUMB_CLIENT
examplesethumbclientdir = $(pkgdatadir)/examples/ethumb-client
examplesethumbclient_DATA = $(examples_ethumb_client_files)
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
