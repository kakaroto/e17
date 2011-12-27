ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src
EXTRA_DIST = \
  AUTHORS \
  COPYING \
  THANKS

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = email.pc

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h* config.sub \
	configure depcomp install-sh ltmain.sh missing compile m4/l* ylwrap

CLEANFILES = email-*.tar.bz2 email-*.tar.gz

clean-local:
	rm -rf email-1.0.0

maintainer-clean-local:
	rm -rf autom4te.cache
