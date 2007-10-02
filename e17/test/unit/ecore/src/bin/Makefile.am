MAINTAINERCLEANFILES = Makefile.in


AM_CPPFLAGS = -DPACKAGE_DATA_DIR=\"$(datadir)\"

bin_PROGRAMS = ecore_unit_tests

ecore_unit_tests_SOURCES = ecore_unit_tests.c ecore_file_test.c
ecore_unit_tests_CFLAGS = $(ECORE_UNIT_TESTS_CFLAGS)
ecore_unit_tests_LDADD = $(ECORE_UNIT_TESTS_LIBS)

EXTRA_DIST = ecore_file_test.h
