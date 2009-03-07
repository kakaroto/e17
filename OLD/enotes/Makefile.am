## Process this file with automake to produce Makefile.in

SUBDIRS = src data
bin_SCRIPTS = enewnote

ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in

#### Install DATA 
#install-data-local:
#	@$(NORMAL_INSTALL)
#	if test -d $(srcdir)/data; then \
#	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/data; \
#	  for d in $(srcdir)/data/*; do \
#	      cp -fr $$d $(DESTDIR)$(pkgdatadir)/data; \
#	  done \
#	fi

#### Uninstall DATA - probly not a great idea.
##uninstall-data-local:
##	@$(NORMAL_UNINSTALL)
##	if test -d $(DESTDIR)$(pkgdatadir)/data; \
##		rm -fr $(DESTDIR)$(pkgdatadir)/data; \
##	fi

#### Package DATA when creating a distribution
dist-hook:
	if test -d data; then \
	  mkdir $(distdir)/data; \
	  for d in data/*; do \
	      cp -r $$d $(distdir)/data; \
	  done \
	fi

#### Add these files to the distribution
EXTRA_DIST = README AUTHORS COPYING autogen.sh
