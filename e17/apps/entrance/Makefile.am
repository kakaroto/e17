## Process this file with automake to produce Makefile.in

SUBDIRS = data config src

EXTRA_DIST = \
	autogen.sh README README.edje AUTHORS COPYING entrance.spec \
	debian/changelog debian/changelog.in debian/copyright \
	debian/control debian/rules debian/dirs debian/docs \
	debian/README.Debian debian/postinst debian/postrm \
	debian/prerm debian/conffiles debian/config

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in debian/changelog

dist-hook:
	if test -d data; then \
	  (cd $(srcdir); tar -cf - data) | (cd $(distdir); tar -xf -) \
	fi
