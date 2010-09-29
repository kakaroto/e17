MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id $(top_srcdir)/data/themes -fd $(top_srcdir)/data/themes -id $(top_srcdir)/data/themes/images

filesdir = $(datadir)/$(PACKAGE)
files_DATA = default.edj

images = \
images/back-button.png \
images/back_bg.png \
images/bg.png \
images/bottom_bg.png \
images/scrollbar-bg.png \
images/scrollbar-knob.png \
images/top_bg.png \
images/bt-back.png \
images/bt-back2.png \
images/bt-next.png \
images/bt-next2.png \
images/bt-play.png \
images/bt-pause.png

fonts =

EXTRA_DIST = default.edc $(images) $(fonts)

default.edj: Makefile $(images) $(fonts) default.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default.edc \
	$(top_builddir)/data/themes/default.edj

clean-local:
	rm -f default.edj
