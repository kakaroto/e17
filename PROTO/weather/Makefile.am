ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs

SUBDIRS = src

filesdir = $(datadir)
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = eweather.pc

clean-local:
	rm -rf *~

uninstall:
	rm -rf $(DESTDIR)$(datadir)
