4AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = edje_cc
EDJE_FLAGS = -v -id $(top_srcdir)/data -fd $(top_srcdir)/data

filesdir = $(datadir)/rage

files_DATA = \
default.edj

EXTRA_DIST = \
default.edc \
Vera.ttf \
VeraBd.ttf \
bg.png \
bg_over.png \
busy-1.png \
busy-2.png \
busy-3.png \
busy-4.png \
busy-5.png \
busy-6.png \
busy-7.png \
busy-8.png \
busy-9.png \
videos.png \
audios.png \
photos.png \
config.png \
video.png \
dvd.png \
audio.png \
photo.png \
mitem.png \
mitem2.png \
miniover.png \
update.png \
book.png \
favorites.png \
resume.png \
bug.png \
genre.png

default.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/default.edc \
	$(top_builddir)/data/default.edj

clean-local:
	rm -f *.edj
