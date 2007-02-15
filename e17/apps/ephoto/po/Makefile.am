MAINTAINERCLEANFILES = Makefile.in $(PACKAGE).pot

MSGFMT = msgfmt

localedir = $(datadir)/locale
locale_DATA = $(addsuffix .mo,$(ALL_LINGUAS))

EXTRA_DIST = $(PACKAGE).pot $(addsuffix .po,$(ALL_LINGUAS))

%.mo : %.po
	$(MSGFMT) -f -o $@ $<

install-localeDATA:
uninstall-localeDATA:
install-data-local:
	for L in $(ALL_LINGUAS); do \
	  $(mkinstalldirs) $(DESTDIR)$(localedir)/$$L/LC_MESSAGES; \
	  $(INSTALL_DATA) \
	  $$L.mo $(DESTDIR)$(localedir)/$$L/LC_MESSAGES/$(PACKAGE).mo; \
	done

uninstall:
	for L in $(ALL_LINGUAS); do \
	  rm -f  $(DESTDIR)$(localedir)/$$L/LC_MESSAGES/$(PACKAGE).mo; \
	done

clean-local:
	rm -f *.mo
