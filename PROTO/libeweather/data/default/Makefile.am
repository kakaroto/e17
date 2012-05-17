MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = images

EDJE_CC = @edje_cc@
EDJE_FLAGS_VERBOSE_ =
EDJE_FLAGS_VERBOSE_0 =
EDJE_FLAGS_VERBOSE_1 = -v
EDJE_FLAGS = $(EDJE_FLAGS_$(V)) -id $(top_srcdir)/data/default/images

EDJ = theme.edj

EXTRA_DIST = theme.edc

theme.edj:  theme.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/default/theme.edc \
	$(top_builddir)/data/default/theme.edj

icondir = $(pkgdatadir)/default
icon_DATA =  $(EDJ)

clean-local:
	rm -rf *.edj *~
