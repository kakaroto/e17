# $Id: Makefile.am,v 1.6 2001/09/22 16:25:29 mej Exp $

EXTRA_HEADERS = types.h

install-exec-hook:
	$(mkinstalldirs) $(DESTDIR)$(includedir)/$(PACKAGE)
	@for i in $(EXTRA_HEADERS) ; do \
	  $(INSTALL_DATA) $(srcdir)/$$i $(DESTDIR)$(includedir)/$(PACKAGE)/ ; \
	done

EXTRA_DIST = types.h.in
