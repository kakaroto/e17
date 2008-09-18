AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = edje_cc
EDJE_FLAGS = -v -id $(top_srcdir)/data/objects -fd $(top_srcdir)/data/objects

filesdir = $(datadir)/alarm/objects
files_DATA = clock.edj

EXTRA_DIST = \
clock.edc \
clock_base.png \
tacho_fg.png \
ch-00.png ch-01.png ch-02.png ch-03.png ch-04.png ch-05.png ch-06.png \
ch-07.png ch-08.png ch-09.png ch-10.png ch-11.png ch-12.png ch-13.png \
ch-14.png ch-15.png ch-16.png ch-17.png ch-18.png ch-19.png ch-20.png \
ch-21.png ch-22.png ch-23.png ch-24.png ch-25.png ch-26.png ch-27.png \
ch-28.png ch-29.png ch-30.png ch-31.png ch-32.png ch-33.png ch-34.png \
ch-35.png ch-36.png ch-37.png ch-38.png ch-39.png ch-40.png ch-41.png \
ch-42.png ch-43.png ch-44.png ch-45.png ch-46.png ch-47.png ch-48.png \
ch-49.png ch-50.png ch-51.png ch-52.png ch-53.png ch-54.png ch-55.png \
ch-56.png ch-57.png ch-58.png ch-59.png cm-00.png \
cm-00.png cm-01.png cm-02.png cm-03.png cm-04.png cm-05.png cm-06.png \
cm-07.png cm-08.png cm-09.png cm-10.png cm-11.png cm-12.png cm-13.png \
cm-14.png cm-15.png cm-16.png cm-17.png cm-18.png cm-19.png cm-20.png \
cm-21.png cm-22.png cm-23.png cm-24.png cm-25.png cm-26.png cm-27.png \
cm-28.png cm-29.png cm-30.png cm-31.png cm-32.png cm-33.png cm-34.png \
cm-35.png cm-36.png cm-37.png cm-38.png cm-39.png cm-40.png cm-41.png \
cm-42.png cm-43.png cm-44.png cm-45.png cm-46.png cm-47.png cm-48.png \
cm-49.png cm-50.png cm-51.png cm-52.png cm-53.png cm-54.png cm-55.png \
cm-56.png cm-57.png cm-58.png cm-59.png \
cs-00.png cs-01.png cs-02.png cs-03.png cs-04.png cs-05.png cs-06.png \
cs-07.png cs-08.png cs-09.png cs-10.png cs-11.png cs-12.png cs-13.png \
cs-14.png cs-15.png cs-16.png cs-17.png cs-18.png cs-19.png cs-20.png \
cs-21.png cs-22.png cs-23.png cs-24.png cs-25.png cs-26.png cs-27.png \
cs-28.png cs-29.png cs-30.png cs-31.png cs-32.png cs-33.png cs-34.png \
cs-35.png cs-36.png cs-37.png cs-38.png cs-39.png cs-40.png cs-41.png \
cs-42.png cs-43.png cs-44.png cs-45.png cs-46.png cs-47.png cs-48.png \
cs-49.png cs-50.png cs-51.png cs-52.png cs-53.png cs-54.png cs-55.png \
cs-56.png cs-57.png cs-58.png cs-59.png

clock.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/objects/clock.edc \
	$(top_builddir)/data/objects/clock.edj

clean-local:
	rm -f *.edj
