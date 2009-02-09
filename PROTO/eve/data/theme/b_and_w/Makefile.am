MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = edje_cc
EDJE_FLAGS = -v -id $(top_srcdir)/data -fd $(top_srcdir)/data

filesdir = $(datadir)/$(PACKAGE_NAME)/data
files_DATA = b_and_w.edj

EXTRA_DIST = \
b_and_w.edc \
back.png \
control.png \
favorite.png \
forward.png \
go.png \
inset_sunk.png \
reload.png \
sb_hilight.png \
sb_knob.png \
sb_runner_h.png \
sb_runner_v.png \
sb_shine.png \
tb_base.png \
tb_shine.png

b_and_w.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/theme/b_and_w/b_and_w.edc \
	$(top_builddir)/data/theme/b_and_w/b_and_w.edj

clean-local:
	rm -f *.edj
