
## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = INSTALL Makefile.in aclocal.m4 config.guess \
		       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in build-stamp configure-stamp depcomp \
		       README \
		       edb.pc \
                       edb.spec \
                       debian/changelog

SUBDIRS		     = src test tools

bin_SCRIPTS = 

EXTRA_DIST = AUTHORS COPYING COPYING-PLAIN \
             README.in README \
	     edb.pc.in \
	     edb.spec.in edb.spec \
	     debian/changelog \
	     debian/changelog.in \
	     debian/control \
	     debian/copyright \
	     debian/libedb1-dev.install \
	     debian/libedb1.install \
	     debian/rules \
	     debian/edb-tools.install

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = edb.pc
