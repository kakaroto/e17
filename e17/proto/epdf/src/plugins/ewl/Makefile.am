
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EPDF_EWL
EPDF_EWL_PLUGIN = libewl_pdf.la
EPDF_EWL_PLUGIN_SOURCE = ewl_pdf.c
else
EPDF_EWL_PLUGIN =
EPDF_EWL_PLUGIN_SOURCE =
endif

pkgdir                = @EWL_TEST_LIBS_DIR@
pkg_LTLIBRARIES       = $(EPDF_EWL_PLUGIN)

libewl_pdf_la_SOURCES = $(EPDF_EWL_PLUGIN_SOURCE)
libewl_pdf_la_CFLAGS  = @EWL_CFLAGS@ -I$(top_builddir)/src/lib
libewl_pdf_la_LIBADD  = 			\
	$(top_builddir)/src/lib/libepdf.la	\
	@EWL_LIBS@
libewl_pdf_la_LDFLAGS = 			   \
	@create_shared_lib@ -module -avoid-version \
	-L$(top_builddir)/src/lib		   \
	-L$(top_builddir)/src/lib/.libs

EXTRA_DIST	= $(libewl_pdf_la_SOURCES)
exampledir 	= @EWL_TEST_SOURCE_DIR@
example_DATA 	= $(libewl_pdf_la_SOURCES)
