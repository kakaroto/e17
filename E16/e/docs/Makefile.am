if HAVE_XMLTO

doc_DATA = index.html

EXTRA_DIST = README-0.16.8.xml

index.html: README-0.16.8.xml
	$(XMLTO) html $<

install-data-local:
	mkdir -p $(DESTDIR)$(docdir)
	cp *.html $(DESTDIR)$(docdir)/

uninstall-local:
	rm -f $(DESTDIR)$(docdir)/*.html
	rm -f *.html

endif
