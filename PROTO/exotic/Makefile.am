ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src doc

MAINTAINERCLEANFILES = \
Makefile.in \
aclocal.m4 \
config.guess \
config.h.in \
config.h.in~ \
config.sub \
configure \
depcomp \
install-sh \
ltmain.sh \
missing \
$(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz \
$(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.bz2 \
$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-doc.tar.bz2 \
m4/libtool.m4 \
m4/lt~obsolete.m4 \
m4/ltoptions.m4 \
m4/ltsugar.m4 \
m4/ltversion.m4

bin_SCRIPTS =

EXTRA_DIST = \
AUTHORS \
COPYING \
autogen.sh \
exotic.pc.in \
exotic.spec.in \
exotic.spec \
m4/efl_attribute.m4 \
m4/efl_doxygen.m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = exotic.pc

.PHONY: doc

doc:
	@echo "entering doc/"
	$(MAKE) -C doc doc

