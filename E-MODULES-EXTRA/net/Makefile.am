ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing

SUBDIRS = src po

EDJE_FLAGS = -v \
-id $(top_srcdir)/images

filesdir = $(datadir)
files_DATA = module.desktop \
	     e-module-net.edj \
	     net.edj

EXTRA_DIST = $(files_DATA) \
	     net.edc \
	     images/module_icon.png \
	     images/bd.png \
	     images/device.png \
	     images/rx.png \
	     images/tx.png \
	     images/idle.png

%.edj: %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -rf net.edj *~
