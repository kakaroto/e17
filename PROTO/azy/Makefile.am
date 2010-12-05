ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src doc
EXTRA_DIST = \
  AUTHORS \
  COPYING \
  README \
  TODO \
  azy.spec

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = azy.pc

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h* config.sub \
	configure depcomp install-sh ltmain.sh missing compile m4/l* ylwrap

CLEANFILES = azy-*.tar.bz2 azy-*.tar.gz

clean-local:
	rm -rf azy-1.0.0

maintainer-clean-local:
	rm -rf autom4te.cache

.PHONY: doc

# Documentation

doc: all
	@echo "entering doc/"
	$(MAKE) -C doc doc
