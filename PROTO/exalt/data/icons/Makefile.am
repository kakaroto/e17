icondir = $(pkgdatadir)/icons


EDJE_FLAGS = -v

# data files for the module
filesdir =  $(datadir)

icon_DATA =  theme.edj

EXTRA_DIST = $(files_DATA) \
	     theme.edc

e-module-exalt.edj: $(EXTRA_DIST)

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf *.edj *~
uninstall:
	 rm -rf $(DESTDIR)$(datadir)

