
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EDVI_EWL
EDVI_EWL_TEST = edvi_ewl_test
endif

if BUILD_EDVI_ETK
EDVI_ETK_TEST = edvi_etk_test
endif

INCLUDES = 		\
-I$(top_srcdir)/src/lib	\
@EDVI_CFLAGS@

bin_PROGRAMS = edvi_evas_test edvi_esmart_test $(EDVI_EWL_TEST) $(EDVI_ETK_TEST)

edvi_evas_test_SOURCES =			\
       edvi_evas_test.c

edvi_evas_test_LDADD =				\
	$(top_builddir)/src/lib/libedvi.la	\
	$(EDVI_LIBS)

edvi_esmart_test_SOURCES =			\
       edvi_esmart_test.c

edvi_esmart_test_LDADD =			\
	$(top_builddir)/src/lib/libedvi.la	\
	$(EDVI_LIBS)

edvi_ewl_test_SOURCES =				\
       edvi_ewl_test.c

edvi_ewl_test_CFLAGS = @EWL_CFLAGS@

edvi_ewl_test_LDADD =				\
	$(top_builddir)/src/lib/libedvi.la	\
	$(EDVI_LIBS)				\
	$(EWL_LIBS)

edvi_etk_test_SOURCES =				\
       edvi_etk_test.c

edvi_etk_test_CFLAGS = @ETK_CFLAGS@

edvi_etk_test_LDADD =				\
	$(top_builddir)/src/lib/libedvi.la	\
	$(EDVI_LIBS)
