MAINTAINERCLEANFILES = Makefile.in theme.edj

EDJ = theme.edj


EDJE_FLAGS = -v

theme.edj:  theme.edc
	$(EDJE_CC) $(EDJE_FLAGS) theme.edc

icondir = $(pkgdatadir)
icon_DATA =  $(EDJ)

clean-local:
	rm -rf *.edj *~

uninstall:
	rm -rf $(DESTDIR)$(pkgdatadir)

EXTRA_DIST = theme.edc $(wildcard images/*.png)
