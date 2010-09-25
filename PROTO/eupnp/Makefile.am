ACLOCAL_AMFLAGS = -I m4

MAINTAINERCLEANFILES = \
	Makefile.in \
	aclocal.m4 \
	compile \
	config.guess \
	config.h.in \
	config.sub \
	configure \
	depcomp \
	install-sh \
	ltmain.sh \
	missing

SUBDIRS = src

if EUPNP_BUILD_EXAMPLES
   SUBDIRS += examples
endif

EXTRA_DIST = \
	README \
	AUTHORS \
	COPYING \
	m4/ac-plugins.m4 \
	m4/efl_coverage.m4 \
	m4/efl_doxygen.m4 \
	m4/efl_tests.m4 \
	m4/ac_attribute.m4

.PHONY: doc tools

if USE_MODULE_EUPNP_ECORE
   pecore = eupnp-ecore.pc
endif

if USE_MODULE_EUPNP_AV
   pav = eupnp-av.pc
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = eupnp-0.pc $(pecore) $(pav)

# Tools

tools:
if EUPNP_BUILD_TOOLS
	@echo "entering tools/"
	make
	make -C tools all
else
	@echo "reconfigure with --enable-tools"
endif

# Documentation
doc:
	@echo "entering doc/"
	make -C doc doc

# Tests

if EFL_ENABLE_TESTS

check-local:
	@./src/tests/eupnp_suite

else

check-local:
	@echo "reconfigure with --enable-tests"

endif


# Coverage report

if EFL_ENABLE_COVERAGE
lcov-reset:
	@rm -rf coverage
	@find . -name "*.gcda" -exec rm {} \;
	@lcov --directory . --zerocounters

lcov-report:
	@mkdir coverage
	@lcov --compat-libtool --directory $(top_srcdir)/src --capture --output-file coverage/coverage.info
	@lcov -l coverage/coverage.info | grep -v "`cd $(top_srcdir) && pwd`" | cut -d: -f1 > coverage/remove
	@lcov -r coverage/coverage.info `cat coverage/remove` > coverage/coverage.cleaned.info
	@rm coverage/remove
	@mv coverage/coverage.cleaned.info coverage/coverage.info
	@genhtml -t "$(PACKAGE_STRING)" -o coverage coverage/coverage.info

coverage:
	@make lcov-reset
	@make check
	@make lcov-report
else
lcov-reset:
	@echo "reconfigure with --enable-coverage"

lcov-report:
	@echo "reconfigure with --enable-coverage"

coverage:
	@echo "reconfigure with --enable-tests --enable-coverage"
endif

clean-local:
	@rm -rf coverage
