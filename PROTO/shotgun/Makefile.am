ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src data
EXTRA_DIST = \
  AUTHORS \
  COPYING \
  README \
  TODO

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h* config.sub \
	configure depcomp install-sh ltmain.sh missing compile m4/l* ylwrap

CLEANFILES = shotgun-*.tar.bz2 shotgun-*.tar.gz

clean-local:
	rm -rf shotgun-1.0.0

maintainer-clean-local:
	rm -rf autom4te.cache

#.PHONY: doc

# Documentation

#doc: all
#	@echo "entering doc/"
#	$(MAKE) -C doc doc
