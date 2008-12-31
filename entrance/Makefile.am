## Process this file with automake to produce Makefile.in

SUBDIRS = data config src

EXTRA_DIST = \
	autogen.sh README README.edje AUTHORS COPYING entrance.spec \
	m4/ac_expand_dir.m4 entrance.spec.in

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs compile depcomp \
		       stamp-h.in config.h.in config.h.in~

dist-hook:
	if test -d data; then \
	  (cd $(srcdir); tar -cf - data) | (cd $(distdir); tar -xf -) \
	fi
