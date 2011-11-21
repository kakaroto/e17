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

pkgconfigdir = $(libdir)/pkgconfig

if BUILD_EHAL
EHAL_PC = ehal.pc
endif

if BUILD_ENOTIFY
ENOTIFY_PC = enotify.pc
endif

if BUILD_ECONNMAN0_7X
ECONNMAN_PC = econnman-0.7x.pc
endif

if BUILD_EBLUEZ
EBLUEZ_PC = ebluez.pc
endif

if BUILD_EOFONO
EOFONO_PC = eofono.pc
endif

if BUILD_EUKIT
EUKIT_PC = eukit.pc
endif

pkgconfig_DATA = edbus.pc $(EBLUEZ_PC) $(ECONNMAN_PC) $(EHAL_PC) $(ENOTIFY_PC) $(EOFONO_PC) $(EUKIT_PC)

EXTRA_DIST = \
AUTHORS \
COPYING \
README \
$(pkgconfig_DATA) \
autogen.sh \
ebluez.pc.in \
econnman-0.7x.pc.in \
edbus.pc.in \
ehal.pc.in \
enotify.pc.in \
eofono.pc.in \
eukit.pc.in \
e_dbus.spec \
e_dbus.spec.in \
m4/ac_attribute.m4 \
m4/efl_binary.m4 \
m4/efl_compiler_flag.m4 \
m4/efl_doxygen.m4

.PHONY: doc

# Documentation

doc:
	@echo "entering doc/"
	make -C doc doc
