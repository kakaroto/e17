MAINTAINERCLEANFILES = Makefile.in theme.edj

SUBDIRS = images

EDJ = theme.edj


EDJE_FLAGS = -v \
	     -id $(top_srcdir)/data/images

theme.edj:  theme.edc
	$(EDJE_CC) $(EDJE_FLAGS) theme.edc

icondir = $(pkgdatadir)
icon_DATA =  $(EDJ)

clean-local:
	rm -rf *.edj *~

uninstall:
	rm -rf $(DESTDIR)$(pkgdatadir)²
