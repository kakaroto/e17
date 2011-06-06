ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs \
		       drawer.pc

SUBDIRS = data src

if HAVE_PO

SUBDIRS += po

endif

# data files for the module
filesdir =  $(datadir)

files_DATA = module.desktop

EXTRA_DIST = module.desktop.in \
	     e_modules-drawer.spec.in \
	     data/images/module_icon.png \
	     drawer.pc.in

clean-local:
	 rm -rf module.desktop e_modules-drawer.spec *~

uninstall:
	 rm -rf $(DESTDIR)$(datadir)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = drawer.pc
