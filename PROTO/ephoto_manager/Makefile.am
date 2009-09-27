ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src

MAINTAINERCLEANFILES = \
Makefile.in \
aclocal.m4 \
compile \
config.guess \
config.h.in  \
config.sub  \
configure  \
depcomp \
install-sh \
ltmain.sh  \
missing  \
eet_docs.tar.gz \
m4/libtool.m4 \
m4/lt~obsolete.m4 \
m4/ltoptions.m4 \
m4/ltsugar.m4 \
m4/ltversion.m4

EXTRA_DIST = \
AUTHORS \
COPYING \
COPYING-PLAIN \
autogen.sh \
eyelight.pc.in \
eyelight.pc \
README.in \
README \
Doxyfile \
doc \
gendoc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ephoto_manager.pc
ACLOCAL_AMFLAGS = -I m4

.PHONY: doc

# Documentation

doc:
	@echo "entering doc/"
	make -C doc doc
