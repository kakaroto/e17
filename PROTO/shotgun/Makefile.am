ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src

if GUI
SUBDIRS += data
endif

EXTRA_DIST = \
  AUTHORS \
  COPYING \
  README \
  TODO

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = shotgun.pc

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h* config.sub \
	configure depcomp install-sh ltmain.sh missing compile m4/l* ylwrap

CLEANFILES = shotgun-*.tar.bz2 shotgun-*.tar.gz shotgun.pc

maintainer-clean-local:
	rm -rf autom4te.cache

#.PHONY: doc

# Documentation

#doc: all
#	@echo "entering doc/"
#	$(MAKE) -C doc doc
