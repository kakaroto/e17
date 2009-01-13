
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src doc

MAINTAINERCLEANFILES = \
Makefile.in \
aclocal.m4 \
compile \
config.guess \
config.h.in \
config.sub \
configure \
depcomp \
install-sh \
ltmain.sh  \
missing \
$(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz \
$(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.bz2 \
$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-doc.tar.bz2 \
stamp-h.in \
m4/libtool.m4 \
m4/lt~obsolete.m4 \
m4/ltoptions.m4 \
m4/ltsugar.m4 \
m4/ltversion.m4

EXTRA_DIST = \
AUTHORS \
README \
epdf.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = epdf.pc

.PHONY: doc

# Documentation

doc:
	@echo "entering doc/"
	make -C doc doc
