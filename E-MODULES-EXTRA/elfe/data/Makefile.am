SUBDIRS = themes

MAINTAINERCLEANFILES = Makefile.in module.desktop

filesdir = $(datadir)
files_DATA = module.desktop

clean-local:
	 rm -rf module.desktop *~

uninstall:
	 rm -rf $(DESTDIR)$(datadir)