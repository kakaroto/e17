MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = images

EDJE_CC = @edje_cc@
EDJE_FLAGS_VERBOSE_ =
EDJE_FLAGS_VERBOSE_0 =
EDJE_FLAGS_VERBOSE_1 = -v
EDJE_FLAGS = $(EDJE_FLAGS_$(V)) -id $(top_srcdir)/data/simple/images

EDJ = theme.edj

EXTRA_DIST = theme.edc

theme.edj:  theme.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/simple/theme.edc \
	$(top_builddir)/data/simple/theme.edj

icondir = $(pkgdatadir)/simple
icon_DATA =  $(EDJ)

clean-local:
	rm -rf *.edj *~
