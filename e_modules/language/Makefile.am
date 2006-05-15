MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = src data

# data files for the module
filesdir = $(datadir)
files_DATA = \
module.eap \
module_icon.png

EXTRA_DIST = $(files_DATA)

pkgdir		       = $(datadir)/$(MODULE_ARCH)

clean-local:
	rm -rf *~
