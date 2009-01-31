if HAVE_XMLTO

e16docdir = ${datadir}/doc/${PACKAGE}
e16doc_DATA = index.html

EXTRA_DIST = README-0.16.8.xml

CLEANFILES = *.html

index.html: README-0.16.8.xml
	$(XMLTO) html $<

install-data-local:
	mkdir -p $(DESTDIR)$(e16docdir)
	cp *.html $(DESTDIR)$(e16docdir)/

uninstall-local:
	rm -f $(DESTDIR)$(e16docdir)/*.html
	rm -f *.html

endif
