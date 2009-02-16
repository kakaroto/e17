MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = edje_cc
EDJE_FLAGS = -v -id $(top_srcdir)/data/theme/b_and_w/images -fd $(top_srcdir)/data

filesdir = $(datadir)/$(PACKAGE_NAME)/data
files_DATA = b_and_w.edj

EXTRA_DIST = \
b_and_w.edc \
images/back.png \
images/control.png \
images/favorite.png \
images/forward.png \
images/go.png \
images/inset_sunk.png \
images/reload.png \
images/sb_hilight.png \
images/sb_knob.png \
images/sb_runner_h.png \
images/sb_runner_v.png \
images/sb_shine.png \
images/tb_base.png \
images/tb_shine.png

b_and_w.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/theme/b_and_w/b_and_w.edc \
	$(top_builddir)/data/theme/b_and_w/b_and_w.edj

clean-local:
	rm -f *.edj
