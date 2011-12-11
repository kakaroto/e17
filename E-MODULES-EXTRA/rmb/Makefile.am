ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs

SUBDIRS = src

filesdir = $(datadir)
files_DATA = module.desktop right.png

EXTRA_DIST = e_modules-rmb.spec \
	     right.png

clean-local:
	 rm -rf module.desktop e_modules-scale.spec *~

uninstall:
	 rm -rf $(DESTDIR)$(datadir)

