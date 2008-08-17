## Process this file with automake to produce Makefile.in

SUBDIRS = data config src

EXTRA_DIST = \
	autogen.sh README README.edje AUTHORS COPYING entrance.spec \
	debian/changelog debian/changelog.in debian/copyright \
	debian/control debian/rules debian/docs \
	debian/README.Debian debian/entrance.postinst \
	debian/entrance.postrm debian/entrance.prerm \
	debian/entrance.config debian/etc/pam debian/etc/init \
	debian/etc/Xsession m4/ac_expand_dir.m4 \
	debian/entrance.templates entrance.spec.in

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs compile depcomp \
		       stamp-h.in debian/changelog config.h.in config.h.in~

dist-hook:
	if test -d data; then \
	  (cd $(srcdir); tar -cf - data) | (cd $(distdir); tar -xf -) \
	fi
