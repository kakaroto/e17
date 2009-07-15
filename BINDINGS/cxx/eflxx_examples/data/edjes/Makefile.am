MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = \
	-v \
	-id $(top_srcdir)/data/images \
	-fd $(top_srcdir)/data/fonts \
	@EDJE_DEF@

filesdir = $(pkgdatadir)/edjes
files_DATA = simple.edj angstrom-bootmanager.edj angstrom-calibrate.edj angstrom-installer.edj esmart_text_entry_test.edj

EXTRA_DIST = angstrom-background.edc \
	     angstrom-bootmanager.edc \
	     angstrom-calibrate.edc \
	     angstrom-installer.edc \
	     e17-default-clock.edc \
	     entrance-buttons.edc \
	     simple.edc \
	     esmart_text_entry_test.edc

simple.edj: Makefile simple.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/edjes/simple.edc

angstrom-bootmanager.edj: Makefile angstrom-bootmanager.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/edjes/angstrom-bootmanager.edc

angstrom-calibrate.edj: Makefile angstrom-calibrate.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/edjes/angstrom-calibrate.edc

angstrom-installer.edj: Makefile angstrom-installer.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/edjes/angstrom-installer.edc

esmart_text_entry_test.edj: Makefile esmart_text_entry_test.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/edjes/esmart_text_entry_test.edc

clean-local:
	rm -f *.edj

