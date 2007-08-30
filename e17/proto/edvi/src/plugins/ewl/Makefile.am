
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EDVI_EWL
EDVI_EWL_PLUGIN = libewl_dvi.la
else
EDVI_EWL_PLUGIN = 
endif

pkgdir                = @EWL_TEST_LIBS_DIR@
pkg_LTLIBRARIES       = $(EDVI_EWL_PLUGIN)

libewl_dvi_la_SOURCES = ewl_dvi.c
libewl_dvi_la_CFLAGS  = @EWL_CFLAGS@ -I$(top_builddir)/src/lib
libewl_dvi_la_LIBADD  = 			\
	$(top_builddir)/src/lib/libedvi.la	\
	@EWL_LIBS@
libewl_dvi_la_LDFLAGS = 			\
	-L$(top_builddir)/src/lib		\
	-L$(top_builddir)/src/lib/.libs

EXTRA_DIST	= $(libewl_dvi_la_SOURCES)
exampledir 	= @EWL_TEST_SOURCE_DIR@
example_DATA 	= $(libewl_dvi_la_SOURCES)
