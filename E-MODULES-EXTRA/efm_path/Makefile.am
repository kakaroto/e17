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
	   e-module-efm_path.edj

EXTRA_DIST = $(files_DATA) \
	     images/entry.png \
	     fonts/Vera.ttf

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	    rm -rf e-module-efm_path.edj module.desktop *~
uninstall:
	  rm -rf $(DESTDIR)$(datadir)
