ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs

SUBDIRS = src data

EXTRA_DIST = e_modules-elfe.spec



clean-local:
	 rm -rf module.desktop e_modules-elfe.spec *~

uninstall:
	 rm -rf $(DESTDIR)$(datadir)

