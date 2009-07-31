ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs

EDJE_FLAGS = -v -id images # -fd fonts

# data files for the module
filesdir =  $(datadir)

files_DATA = e-module-drawer.edj

EXTRA_DIST = $(files_DATA) \
	     e-module-drawer.edc \
	     images/arrow_down.png \
	     images/arrow_left.png \
	     images/arrow_right.png \
	     images/arrow_up.png \
	     images/bt_glow.png \
	     images/bt_sm_base1.png \
	     images/bt_sm_base2.png \
	     images/bt_sm_hilight.png \
     	     images/bt_sm_shine.png \
	     images/env_bg_bottom.png \
	     images/env_bg_left.png \
	     images/env_bg_right.png \
	     images/env_bg_top.png \
	     images/env_fg_bottom.png \
	     images/env_fg_left.png \
	     images/env_fg_right.png \
	     images/env_fg_top.png \
	     images/gadman_border.png \
	     images/h_line.png \
	     images/logo_white_128.png \
	     images/popup_bg.png \
             images/popup_overlay.png \
	     images/sb_runnerh.png \
	     images/sb_runnerv.png \
	     images/v_line.png

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf e-module-drawer.edj *~
uninstall:
	 rm -rf $(DESTDIR)$(datadir)

