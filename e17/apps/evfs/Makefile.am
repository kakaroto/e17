## Process this file with automake to produce Makefile.in

SUBDIRS = src

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in debian/changelog


EXTRA_DIST = README AUTHORS COPYING \
	     debian/control \
	     debian/copyright \
	     debian/evfs0-bin.install \
	     debian/evfs0-debug.install \
	     debian/libevfs0-dev.install \
	     debian/libevfs0.install \
	     debian/libevfs0-plugin-bzip2.install \
	     debian/libevfs0-plugin-ftp.install \
	     debian/libevfs0-plugin-gzip.install \
	     debian/libevfs0-plugin-posix.install \
	     debian/libevfs0-plugin-samba.install \
	     debian/libevfs0-plugin-sftp.install \
	     debian/libevfs0-plugin-tar.install \
	     debian/rules \
	     debian/changelog.in \
	     debian/changelog

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = evfs.pc
