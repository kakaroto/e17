# $Id: Makefile.am,v 1.6 2001/09/22 16:25:29 mej Exp $

EXTRA_HEADERS = \
    array.h avl_tree.h dlinked_list.h linked_list.h list_if.h obj.h regexp.h \
    socket.h str.h tok.h types.h url.h vector_if.h

install-exec-hook:
	$(mkinstalldirs) $(DESTDIR)$(includedir)/$(PACKAGE)
	for i in $(EXTRA_HEADERS) ; do \
	  $(INSTALL_DATA) $(srcdir)/$$i $(DESTDIR)$(includedir)/$(PACKAGE)/ ; \
	done

EXTRA_DIST = $(EXTRA_HEADERS) types.h.in

all: types.h

types.h: types.h.in
	(cd $(top_srcdir) && ./config.status)
