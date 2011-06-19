MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = 					\
-I$(top_srcdir)/src/enlil 			\
-I$(top_builddir)/src/enlil 			\
-DPACKAGE_BIN_DIR=\"$(bindir)\" 		\
-DPACKAGE_LIB_DIR=\"$(libdir)\" 		\
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" 	\
-DPACKAGE_BUILD_DIR=\"`pwd`/$(top_builddir)\" 	\
@CHECK_CFLAGS@					\
@ENLIL_CFLAGS@

if EFL_ENABLE_TESTS

check_PROGRAMS = enlil_suite

enlil_suite_SOURCES = 	\
enlil_suite.c 		\
enlil_test_sync.c		

enlil_suite_LDADD = @CHECK_LIBS@ $(top_builddir)/src/enlil/libenlil.la -lm



endif

EXTRA_DIST =  enlil_suite.h
