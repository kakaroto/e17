## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src data tools doc

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
		   config.h.in config.sub configure install-sh \
		   ltconfig ltmain.sh missing mkinstalldirs \
		   stamp-h.in

bin_SCRIPTS = 

EXTRA_DIST = README AUTHORS COPYING ewl.spec ewl.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ewl.pc

if EWL_ENABLE_GCOV
lcov-reset:
	@rm -rf lcov
	@find . -name "*.gcda" -exec rm {} \;
	@lcov --directory . --zerocounters

lcov-report:
	@mkdir lcov
	@lcov --compat-libtool --directory . --capture --output-file lcov/lcov.info
	@lcov -l lcov/lcov.info | grep -v "`cd $(top_srcdir) && pwd`" | cut -d: -f1 > lcov/remove
	@lcov -r lcov/lcov.info `cat lcov/remove` > lcov/lcov.cleaned.info
	@rm lcov/remove
	@mv lcov/lcov.cleaned.info lcov/lcov.info
	@genhtml -t "$(PACKAGE_STRING)" -o lcov lcov/lcov.info

lcov:
	@make lcov-reset
	@if test "x$(EWL_UNIT_TEST)" = "x" ; then \
	  ewl_test -unit -all; \
	else \
	  ewl_test -unit $(EWL_UNIT_TEST); \
	fi
	@make lcov-report

clean-local:
	@rm -rf lcov
else
lcov-reset:
	@echo "reconfigure with --enable-gcov"

lcov-report:
	@echo "reconfigure with --enable-gcov"

lcov:
	@echo "reconfigure with --enable-gcov"
endif
