ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop

SUBDIRS = src po

EDJE_FLAGS = -v \
	   -id $(top_srcdir)/images \
	   -fd $(top_srcdir)/fonts

filesdir = $(datadir)
files_DATA = \
	   module.desktop \
	   e-module-efm_nav.edj

EXTRA_DIST = $(files_DATA) \
	     images/home.png \
	     images/bt_base1.png \
	     images/bt_base2.png \
	     images/bt_hilight.png \
	     images/bt_shine.png \
	     images/bt_dis_base.png \
	     images/bt_dis_shine.png \
	     images/bt_dis_hilight.png \
	     images/back.png \
	     images/forward.png \
	     images/up.png \
	     images/favorites.png \
	     images/refresh.png \
	     fonts/Vera.ttf

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	    rm -rf e-module-efm_nav.edj module.desktop *~
uninstall:
	  rm -rf $(DESTDIR)$(datadir)
