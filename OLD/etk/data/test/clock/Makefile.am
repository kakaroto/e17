MAINTAINERCLEANFILES = Makefile.in
	  
EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/test/clock/ \
-fd $(top_srcdir)/data/test/clock/

filesdir = $(pkgdatadir)/test
files_DATA = clock.edj

CLOCK_DEP = \
Vera.ttf \
bg.png \
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
parts/day.edc \
seconds/seconds.cursor.43.png \
seconds/seconds.cursor.39.png \
seconds/seconds.cursor.49.png \
seconds/seconds.cursor.22.png \
seconds/seconds.cursor.28.png \
seconds/seconds.cursor.55.png \
seconds/seconds.cursor.02.png \
seconds/seconds.cursor.29.png \
seconds/seconds.cursor.01.png \
seconds/seconds.cursor.08.png \
seconds/seconds.cursor.34.png \
seconds/seconds.cursor.35.png \
seconds/seconds.cursor.46.png \
seconds/seconds.cursor.31.png \
seconds/seconds.cursor.05.png \
seconds/seconds.cursor.37.png \
seconds/seconds.cursor.23.png \
seconds/seconds.cursor.17.png \
seconds/seconds.cursor.25.png \
seconds/seconds.cursor.15.png \
seconds/seconds.cursor.24.png \
seconds/seconds.cursor.50.png \
seconds/seconds.cursor.19.png \
seconds/seconds.cursor.57.png \
seconds/seconds.cursor.38.png \
seconds/seconds.cursor.36.png \
seconds/seconds.cursor.58.png \
seconds/seconds.cursor.32.png \
seconds/seconds.cursor.41.png \
seconds/seconds.cursor.00.png \
seconds/seconds.cursor.18.png \
seconds/seconds.cursor.42.png \
seconds/seconds.cursor.12.png \
seconds/seconds.cursor.59.png \
seconds/seconds.cursor.33.png \
seconds/seconds.cursor.47.png \
seconds/seconds.cursor.44.png \
seconds/seconds.cursor.54.png \
seconds/seconds.cursor.09.png \
seconds/seconds.cursor.11.png \
seconds/seconds.cursor.48.png \
seconds/seconds.cursor.53.png \
seconds/seconds.cursor.14.png \
seconds/seconds.cursor.45.png \
seconds/seconds.cursor.10.png \
seconds/seconds.cursor.06.png \
seconds/seconds.cursor.16.png \
seconds/seconds.cursor.07.png \
seconds/seconds.cursor.04.png \
seconds/seconds.cursor.30.png \
seconds/seconds.cursor.56.png \
seconds/seconds.cursor.40.png \
seconds/seconds.cursor.20.png \
seconds/seconds.cursor.13.png \
seconds/seconds.cursor.26.png \
seconds/seconds.cursor.21.png \
seconds/seconds.cursor.03.png \
seconds/seconds.cursor.52.png \
seconds/seconds.cursor.27.png \
seconds/seconds.cursor.51.png \
minutes/minutes.cursor.20.png \
minutes/minutes.cursor.07.png \
minutes/minutes.cursor.04.png \
minutes/minutes.cursor.01.png \
minutes/minutes.cursor.05.png \
minutes/minutes.cursor.49.png \
minutes/minutes.cursor.50.png \
minutes/minutes.cursor.54.png \
minutes/minutes.cursor.10.png \
minutes/minutes.cursor.06.png \
minutes/minutes.cursor.43.png \
minutes/minutes.cursor.17.png \
minutes/minutes.cursor.48.png \
minutes/minutes.cursor.59.png \
minutes/minutes.cursor.37.png \
minutes/minutes.cursor.52.png \
minutes/minutes.cursor.09.png \
minutes/minutes.cursor.38.png \
minutes/minutes.cursor.21.png \
minutes/minutes.cursor.30.png \
minutes/minutes.cursor.53.png \
minutes/minutes.cursor.00.png \
minutes/minutes.cursor.28.png \
minutes/minutes.cursor.33.png \
minutes/minutes.cursor.32.png \
minutes/minutes.cursor.29.png \
minutes/minutes.cursor.44.png \
minutes/minutes.cursor.19.png \
minutes/minutes.cursor.57.png \
minutes/minutes.cursor.15.png \
minutes/minutes.cursor.03.png \
minutes/minutes.cursor.45.png \
minutes/minutes.cursor.47.png \
minutes/minutes.cursor.39.png \
minutes/minutes.cursor.31.png \
minutes/minutes.cursor.13.png \
minutes/minutes.cursor.46.png \
minutes/minutes.cursor.35.png \
minutes/minutes.cursor.55.png \
minutes/minutes.cursor.36.png \
minutes/minutes.cursor.56.png \
minutes/minutes.cursor.25.png \
minutes/minutes.cursor.34.png \
minutes/minutes.cursor.18.png \
minutes/minutes.cursor.23.png \
minutes/minutes.cursor.51.png \
minutes/minutes.cursor.22.png \
minutes/minutes.cursor.02.png \
minutes/minutes.cursor.24.png \
minutes/minutes.cursor.12.png \
minutes/minutes.cursor.40.png \
minutes/minutes.cursor.11.png \
minutes/minutes.cursor.16.png \
minutes/minutes.cursor.14.png \
minutes/minutes.cursor.58.png \
minutes/minutes.cursor.41.png \
minutes/minutes.cursor.27.png \
minutes/minutes.cursor.26.png \
minutes/minutes.cursor.42.png \
minutes/minutes.cursor.08.png \
hour/hour.cursor.09.png \
hour/hour.cursor.28.png \
hour/hour.cursor.21.png \
hour/hour.cursor.32.png \
hour/hour.cursor.36.png \
hour/hour.cursor.26.png \
hour/hour.cursor.15.png \
hour/hour.cursor.53.png \
hour/hour.cursor.33.png \
hour/hour.cursor.34.png \
hour/hour.cursor.49.png \
hour/hour.cursor.54.png \
hour/hour.cursor.20.png \
hour/hour.cursor.43.png \
hour/hour.cursor.22.png \
hour/hour.cursor.13.png \
hour/hour.cursor.45.png \
hour/hour.cursor.16.png \
hour/hour.cursor.50.png \
hour/hour.cursor.55.png \
hour/hour.cursor.07.png \
hour/hour.cursor.14.png \
hour/hour.cursor.24.png \
hour/hour.cursor.48.png \
hour/hour.cursor.46.png \
hour/hour.cursor.25.png \
hour/hour.cursor.05.png \
hour/hour.cursor.27.png \
hour/hour.cursor.11.png \
hour/hour.cursor.37.png \
hour/hour.cursor.47.png \
hour/hour.cursor.35.png \
hour/hour.cursor.23.png \
hour/hour.cursor.44.png \
hour/hour.cursor.29.png \
hour/hour.cursor.52.png \
hour/hour.cursor.01.png \
hour/hour.cursor.04.png \
hour/hour.cursor.59.png \
hour/hour.cursor.41.png \
hour/hour.cursor.58.png \
hour/hour.cursor.30.png \
hour/hour.cursor.56.png \
hour/hour.cursor.10.png \
hour/hour.cursor.19.png \
hour/hour.cursor.18.png \
hour/hour.cursor.08.png \
hour/hour.cursor.39.png \
hour/hour.cursor.17.png \
hour/hour.cursor.00.png \
hour/hour.cursor.38.png \
hour/hour.cursor.02.png \
hour/hour.cursor.03.png \
hour/hour.cursor.40.png \
hour/hour.cursor.31.png \
hour/hour.cursor.06.png \
hour/hour.cursor.51.png \
hour/hour.cursor.12.png \
hour/hour.cursor.42.png \
hour/hour.cursor.57.png

EXTRA_DIST = $(CLOCK_DEP)


clock.edj: Makefile $(CLOCK_DEP)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/test/clock/expedition.edc \
	$(top_builddir)/data/test/clock/clock.edj

clean-local:
	rm -f *.edj
	rm -rf autom4te.cache
