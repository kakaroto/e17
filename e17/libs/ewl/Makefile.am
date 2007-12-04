
## Process this file with automake to produce Makefile.in

SUBDIRS = src data tools

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
		   config.h.in config.sub configure install-sh \
		   ltconfig ltmain.sh missing mkinstalldirs \
		   stamp-h.in debian/changelog

bin_SCRIPTS = 

EXTRA_DIST = README AUTHORS COPYING ewl.spec ewl.pc.in \
	     debian/rules \
	     debian/control \
	     debian/changelog \
	     debian/copyright \
	     debian/changelog.in \
	     debian/ewl0-bin.install \
	     debian/libewl0-dev.install \
	     debian/libewl0-engine-evas-buffer.install \
	     debian/libewl0-engine-evas-fb.install \
	     debian/libewl0-engine-evas-gl-x11.install \
	     debian/libewl0-engine-evas.install \
	     debian/libewl0-engine-evas-software-x11.install \
	     debian/libewl0-engine-evas-xrender.install \
	     debian/libewl0-engines-all.install \
	     debian/libewl0-engine-x11.install \
	     debian/libewl0.install

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ewl.pc
