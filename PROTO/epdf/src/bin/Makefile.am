
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EPDF_EWL
EPDF_EWL_TEST = epdf_ewl_test
endif

if BUILD_EPDF_ETK
EPDF_ETK_TEST = epdf_etk_test
endif

bin_PROGRAMS = epdf_evas_test epdf_esmart_test $(EPDF_EWL_TEST) $(EPDF_ETK_TEST)

## evas test

epdf_evas_test_SOURCES = \
epdf_evas_test.c

epdf_evas_test_CPPFLAGS =\
-I$(top_srcdir)/src/lib	\
@ECORE_EVAS_CFLAGS@ \
@EPDF_CFLAGS@

epdf_evas_test_LDADD = \
$(top_builddir)/src/lib/libepdf.la \
$(EPDF_LIBS) $(ECORE_EVAS_LIBS)

## esmart test

epdf_esmart_test_SOURCES = \
epdf_esmart_test.c

epdf_esmart_test_CPPFLAGS =\
-I$(top_srcdir)/src/lib	\
@ECORE_EVAS_CFLAGS@ \
@EPDF_CFLAGS@

epdf_esmart_test_LDADD = \
$(top_builddir)/src/lib/libepdf.la \
$(EPDF_LIBS) $(ECORE_EVAS_LIBS)

## ewl test

epdf_ewl_test_SOURCES = \
epdf_ewl_test.c

epdf_ewl_test_CPPFLAGS =\
-I$(top_srcdir)/src/lib	\
@EPDF_CFLAGS@ \
@EWL_CFLAGS@

epdf_ewl_test_LDADD = $(top_builddir)/src/lib/libepdf_ewl.la $(top_builddir)/src/lib/libepdf.la

## etk test

epdf_etk_test_SOURCES = \
epdf_etk_test.c

epdf_etk_test_CPPFLAGS = \
-I$(top_srcdir)/src/lib	\
@EPDF_CFLAGS@ \
@ETK_CFLAGS@

epdf_etk_test_LDADD = $(top_builddir)/src/lib/libepdf_etk.la $(top_builddir)/src/lib/libepdf.la
