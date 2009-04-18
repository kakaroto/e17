e16docdir = ${datadir}/doc/${PACKAGE}

EXTRA_DIST = README $(DOCS_DIST)

if HAVE_XMLTO

e16doc_DATA = README.html

DOCS_DIST = README.xml e16-docs.xml xml2dox.xsl # e16.1

CLEANFILES = *.html MAIN e16-docs.1.xml e16.1

README.html: README.xml
	$(XMLTO) html $<
	sed -i -e 's/index.html/README.html/' index.html ar*.html
	mv index.html $@

man_MANS = # e16.1

e16.1: e16-docs.1.xml
	$(XMLTO) man $<

e16-docs.html: e16-docs.1.xml
	$(XMLTO) html $<
	sed -i -e 's/index.html/e16-docs.html/' index.html
	mv index.html $@

e16-docs.1.xml: e16-docs.xml
	grep -v -e "^<font" -e "^<img" -e "more on next page" $< > $@

MAIN: e16-docs.xml xml2dox.xsl
	xsltproc $(srcdir)/xml2dox.xsl $< > $@

install-data-local:
	mkdir -p $(DESTDIR)$(e16docdir)
	cp *.html $(DESTDIR)$(e16docdir)/

uninstall-local:
	rm -f $(DESTDIR)$(e16docdir)/*.html
	rm -f *.html

endif
