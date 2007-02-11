ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = src

EDJE_FLAGS = -v \
-id $(top_srcdir)/images \
-fd $(top_srcdir)/fonts

filesdir = $(datadir)
files_DATA = module.desktop \
	     module.edj \
	     net.edj
	     
EXTRA_DIST = $(files_DATA) \
	     module.desktop.in \
	     module.edc \
	     net.edc \
	     fonts/Vera.ttf \
	     fonts/VeraBd.ttf \
	     images/module_icon.png \
	     images/bd.png \
	     images/device.png \
	     images/rx.png \
	     images/tx.png \
	     images/idle.png	     

%.edj: %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@
	
module.edj: module.edc
	$(EDJE_CC) $(EDJE_FLAGS) module.edc module.edj

clean-local:
	rm -rf module.edj net.edj *~
	
uninstall:
	rm -rf $(datadir)
