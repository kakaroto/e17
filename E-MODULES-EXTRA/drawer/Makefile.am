ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs

SUBDIRS = data src po

# data files for the module
filesdir =  $(datadir)

files_DATA = module.desktop

EXTRA_DIST = $(files_DATA) \
	     data/images/module_icon.png

clean-local:
	 rm -rf module.desktop *~
uninstall:
	 rm -rf $(DESTDIR)$(datadir)

