ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs

SUBDIRS = src po

EDJE_FLAGS = -v \
	   -id $(top_srcdir)/images \
	   -fd $(top_srcdir)/fonts

# data files for the module
filesdir =  $(datadir)

files_DATA = module.desktop \
	     e-module-exalt.edj

EXTRA_DIST = $(files_DATA) \
	     images/module_icon.png \
	     e-module-exalt.edc \
	     fonts/VeraBd.ttf

e-module-exalt.edj: $(EXTRA_DIST)

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf *.edj module.desktop *~
uninstall:
	 rm -rf $(DESTDIR)$(datadir)

