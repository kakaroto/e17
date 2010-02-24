ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs

SUBDIRS = src

EDJE_FLAGS = -v -id $(top_srcdir)

filesdir = $(datadir)
files_DATA = module.desktop eweather.edj

EXTRA_DIST = module.desktop.in \
	     eweather.edc icon-weather.png

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -rf eweather.edj module.desktop *~

uninstall:
	rm -rf $(DESTDIR)$(datadir)
