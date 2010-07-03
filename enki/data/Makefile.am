MAINTAINERCLEANFILES = Makefile.in theme.edj

EDJ = theme.edj

EDJE_FLAGS = -v -id $(top_srcdir)/data -fd $(top_srcdir)/data

theme.edj:  theme.edc
	$(EDJE_CC) $(EDJE_FLAGS) $(top_srcdir)/data/theme.edc

icondir = $(pkgdatadir)
icon_DATA =  $(EDJ)

clean-local:
	rm -rf *.edj *~

uninstall:
	rm -rf $(DESTDIR)$(pkgdatadir)

