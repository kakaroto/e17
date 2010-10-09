MAINTAINERCLEANFILES = Makefile.in theme.edj

SUBDIRS = images

EDJ = theme.edj

EXTRA_DIST = theme.edc

EDJE_FLAGS = -v \
	     -id $(top_srcdir)/data/default/images

theme.edj:  theme.edc
	$(EDJE_CC) $(EDJE_FLAGS) $(top_srcdir)/data/default/theme.edc

icondir = $(pkgdatadir)/default
icon_DATA =  $(EDJ)

clean-local:
	rm -rf *.edj *~

uninstall:
	rm -rf $(DESTDIR)$(pkgdatadir)²
