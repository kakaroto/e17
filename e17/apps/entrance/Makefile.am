## Process this file with automake to produce Makefile.in

SUBDIRS = src

EXTRA_DIST = \
	autogen.sh README AUTHORS COPYING

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in
install-data-local:
	@$(NORMAL_INSTALL)
	if test -d $(srcdir)/data; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir); \
	  (cd $(srcdir); tar -cf - data) | (cd $(DESTDIR)$(pkgdatadir); tar -xf -); \
	  (cd $(srcdir)/config/init.d; tar -cf - entrance) | (cd $(DESTDIR)$(pkgdatadir); tar -xf -) \
	fi
	$(mkinstalldirs) $(DESTDIR)$(PAM_PREFIX); \
	(cd $(srcdir)/config/pam.d; tar -cf - entrance) | (cd $(DESTDIR)$(PAM_PREFIX); tar -xf -)

dist-hook:
	if test -d data; then \
	  (cd $(srcdir); tar -cf - data) | (cd $(distdir); tar -xf -) \
	fi
