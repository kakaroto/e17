ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = src data po

# data files for the module
filesdir = $(datadir)
files_DATA = e-module-language.edj module.desktop

EXTRA_DIST = $(files_DATA)

pkgdir		       = $(datadir)/$(MODULE_ARCH)

clean-local:
	rm -rf *~
