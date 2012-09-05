MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = images fonts

EDJE_CC = @edje_cc@
EDJE_FLAGS_VERBOSE_ =
EDJE_FLAGS_VERBOSE_0 =
EDJE_FLAGS_VERBOSE_1 = -v
EDJE_FLAGS = $(EDJE_FLAGS_$(V)) -id $(top_srcdir)/data/theme/images -fd $(top_srcdir)/data/theme/fonts

filesdir = $(datadir)/efbb/

files_DATA = efbb.edj

EXTRA_DIST = \
	ammo.edc \
	bar.edc \
	boom.edc \
	button.edc \
	cannon.edc \
	gengrid.edc \
	lvlsel_win.edc \
	main.edc \
	main_win.edc \
	objects.edc \
	options.edc \
	retry_win.edc \
	score.edc


efbb.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/theme/main.edc \
	$(top_builddir)/data/theme/efbb.edj

clean-local:
	rm -f *.edj
