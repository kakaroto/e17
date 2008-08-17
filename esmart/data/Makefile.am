
MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = images

EDJE_CC = @edje_cc@
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/images \
-fd $(top_srcdir)/data/fonts \
@EDJE_DEF@

filesdir = $(pkgdatadir)
files_DATA = esmart_text_entry_test.edj

EXTRA_DIST = esmart_text_entry_test.edc

esmart_text_entry_test.edj: Makefile esmart_text_entry_test.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/esmart_text_entry_test.edc \
	$(top_builddir)/data/esmart_text_entry_test.edj

clean-local:
	rm -f *.edj

