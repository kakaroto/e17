if HAVE_XMLTO

#htmldir = $(pkgdatadir)/html
html_DATA = index.html

EXTRA_DIST = README-0.16.8.xml

index.html: README-0.16.8.xml
	$(XMLTO) html $<

install-data-local:
	mkdir -p $(DESTDIR)$(htmldir)
	cp *.html $(DESTDIR)$(htmldir)/

uninstall-local:
	rm -f $(DESTDIR)$(htmldir)/*.html
	rm -f *.html

endif
