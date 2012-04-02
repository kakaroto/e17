ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src
EXTRA_DIST = \
  AUTHORS \
  COPYING

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h* config.sub \
	configure depcomp install-sh ltmain.sh missing compile m4/l* ylwrap \
$(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz \
$(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.bz2

CLEANFILES = efx-*.tar.bz2 efx-*.tar.gz

maintainer-clean-local:
	rm -rf autom4te.cache
