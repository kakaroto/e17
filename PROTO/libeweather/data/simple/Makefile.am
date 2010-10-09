MAINTAINERCLEANFILES = Makefile.in theme.edj

SUBDIRS = images

EDJ = theme.edj

EXTRA_DIST = theme.edc

EDJE_FLAGS = -v \
	     -id $(top_srcdir)/data/simple/images

theme.edj:  theme.edc
	$(EDJE_CC) $(EDJE_FLAGS) $(top_srcdir)/data/simple/theme.edc

icondir = $(pkgdatadir)/simple
icon_DATA =  $(EDJ)

clean-local:
	rm -rf *.edj *~

uninstall:
	rm -rf $(DESTDIR)$(pkgdatadir)²
