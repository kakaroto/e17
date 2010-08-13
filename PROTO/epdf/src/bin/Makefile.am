
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EPDF_EWL
EPDF_EWL_TEST = epdf_ewl_test
endif

#bin_PROGRAMS = epdf_evas_test epdf_esmart_test $(EPDF_EWL_TEST)
bin_PROGRAMS = epdf_evas_test

## evas test

epdf_evas_test_SOURCES = epdf_evas_test.c

epdf_evas_test_CPPFLAGS =\
-I$(top_srcdir)/src/lib	\
@EPDFBIN_CFLAGS@ \
@EPDF_CFLAGS@

epdf_evas_test_LDADD = \
$(EPDFBIN_LIBS) \
$(top_builddir)/src/lib/libepdf.la

## esmart test

epdf_esmart_test_SOURCES = epdf_esmart_test.c

epdf_esmart_test_CPPFLAGS =\
-I$(top_srcdir)/src/lib	\
@EPDFBIN_CFLAGS@ \
@EPDF_CFLAGS@

epdf_esmart_test_LDADD = \
$(EPDFBIN_LIBS) \
$(top_builddir)/src/lib/libepdf.la

## ewl test

epdf_ewl_test_SOURCES = epdf_ewl_test.c

epdf_ewl_test_CPPFLAGS =\
-I$(top_srcdir)/src/lib	\
@EPDF_CFLAGS@ \
@EWL_CFLAGS@

epdf_ewl_test_LDADD = $(top_builddir)/src/lib/libepdf_ewl.la $(top_builddir)/src/lib/libepdf.la
