ACLOCAL_AMFLAGS = -I m4

EPYDOC = @EPYDOC@

tests_files = \
	tests/test-bt-list-handles.py \
	tests/test-bt-search-devices.py \
	tests/test-signal-emitter.py \
	tests/test-signal-recipient.py \
	tests/test-signal-simple.py

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
        config.h.in

EXTRA_DIST = \
	AUTHORS \
	COPYING \
	README \
	autogen.sh \
	python-edbus.pc.in \
	$(tests_files)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = python-edbus.pc

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/edbus \
	@PYTHON_INCLUDES@ \
	@EDBUS_CFLAGS@

pyexecedbusdir =  $(pyexecdir)
pyexecedbus_LTLIBRARIES = e_dbus.la
e_dbus_la_LIBADD = @EDBUS_LIBS@
e_dbus_la_LDFLAGS = -module -avoid-version
e_dbus_la_LIBTOOLFLAGS = --tag=disable-static
e_dbus_la_SOURCES = $(top_builddir)/module.c


.PHONY: doc doc-clean
if HAVE_EPYDOC
clean-local: doc-clean
doc-clean:
	rm -fr doc/api/html
doc: $(pyexecedbus_LTLIBRARIES) $(pyedbusdir_PYTHON)
	$(AM_V_GEN)PYTHONPATH="$$PWD:$$PWD/.libs:$$PYTHONPATH" \
		$(EPYDOC) --html -o doc/api/html \
			-n "@PACKAGE@ @VERSION@ API" \
			e_dbus
else
doc-clean:
	@echo "Install epydoc then run ./configure again"
doc:
	@echo "Install epydoc then run ./configure again"
endif
