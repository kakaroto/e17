MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = expedition.edc 
	  
EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/test/clock/ \
-fd $(top_srcdir)/data/test/clock/

filesdir = $(pkgdatadir)/test
files_DATA = clock.edj

CLOCK_DEP = \
expedition.edc \
fonts/fonts.edc \
programs/programs.edc \
programs/seconds.edc \
programs/hour.edc \
programs/minutes.edc \
macros/macros.edc \
images/images.edc \
embryo/global.edc \
parts/bg.edc \
parts/hands.edc \
parts/day.edc

clock.edj: Makefile $(CLOCK_DEP)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/test/clock/expedition.edc \
	$(top_builddir)/data/test/clock/clock.edj

clean-local:
	rm -f *.edj
	rm -rf autom4te.cache
