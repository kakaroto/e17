
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EPDF_EWL
EPDF_EWL_TEST = epdf_ewl_test
endif

if BUILD_EPDF_ETK
EPDF_ETK_TEST = epdf_etk_test
endif

INCLUDES = 			\
-I$(top_srcdir)/src/lib	\
@EVAS_CFLAGS@ @ECORE_CFLAGS@

bin_PROGRAMS = epdf_evas_test epdf_esmart_test $(EPDF_EWL_TEST) $(EPDF_ETK_TEST)

epdf_evas_test_SOURCES =			\
       epdf_evas_test.c

epdf_evas_test_LDADD =				\
	$(top_builddir)/src/lib/libepdf.la	\
	$(EVAS_LIBS)				\
	$(ECORE_LIBS)

epdf_esmart_test_SOURCES =			\
       epdf_esmart_test.c

epdf_esmart_test_LDADD =			\
	$(top_builddir)/src/lib/libepdf.la	\
	$(EVAS_LIBS)				\
	$(ECORE_LIBS)

epdf_ewl_test_SOURCES =				\
       epdf_ewl_test.c

epdf_ewl_test_CFLAGS = @EWL_CFLAGS@

epdf_ewl_test_LDADD =				\
	$(top_builddir)/src/lib/libepdf.la	\
	$(EVAS_LIBS)				\
	$(ECORE_LIBS)				\
	$(EWL_LIBS)

epdf_etk_test_SOURCES =				\
       epdf_etk_test.c

epdf_etk_test_CFLAGS = @ETK_CFLAGS@

epdf_etk_test_LDADD =				\
	$(top_builddir)/src/lib/libepdf.la	\
	$(EVAS_LIBS)				\
	$(ECORE_LIBS)

if BUILD_EPDF_EWL
pkgdir                  = @EWL_TEST_LIBS_DIR@
pkg_LTLIBRARIES         = libewl_test_pdf.la

libewl_test_pdf_la_SOURCES = ewl_test_pdf.c
libewl_test_pdf_la_CFLAGS  = @EWL_CFLAGS@
libewl_test_pdf_la_LIBADD  = 			\
	$(top_builddir)/src/lib/libepdf.la	\
	@EWL_LIBS@
libewl_test_pdf_la_LDFLAGS = 			\
	-L$(top_builddir)/src/lib		\
	-L$(top_builddir)/src/lib/.libs

EXTRA_DIST	= $(libewl_test_pdf_la_SOURCES)
exampledir 	= @EWL_TEST_SOURCE_DIR@
example_DATA 	= $(libewl_test_pdf_la_SOURCES)
endif