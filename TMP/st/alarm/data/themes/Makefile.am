AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = edje_cc
EDJE_FLAGS = -v -id $(top_srcdir)/data -fd $(top_srcdir)/data

filesdir = $(datadir)/alarm/themes
files_DATA = default.edj

EXTRA_DIST = \
default.edc \
arrow_down.png \
arrow_left.png \
arrow_right.png \
arrow_up.png \
bt_base1.png \
bt_base2.png \
bt_bases.png \
bt_basew.png \
bt_hilight.png \
bt_hilightw.png \
bt_shine.png \
bt_sm_base1.png \
bt_sm_base2.png \
bt_sm_hilight.png \
bt_sm_shine.png \
dia_botshad.png \
dia_grad.png \
dia_topshad.png \
sb_runnerh.png \
sb_runnerv.png \
shelf_inset.png \
tog_base.png

default.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default.edc \
	$(top_builddir)/data/themes/default.edj

clean-local:
	rm -f *.edj
