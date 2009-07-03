e16docdir = ${datadir}/doc/${PACKAGE}

EXTRA_DIST = README README.xml e16-docs.xml xml2dox.xsl

dist_man_MANS = e16.1

e16-docs.1.xml: e16-docs.xml
	grep -v -e "^<font" -e "^<img" -e "more on next page" $< > $@

e16.1: e16-docs.xml | e16-docs.1.xml
	$(XMLTO) man e16-docs.1.xml

MAINTAINERCLEANFILES = e16-docs.1.xml e16.1

if BUILD_DOCS

DISTCLEANFILES = $(e16doc_DATA) MAIN

e16doc_DATA = README.html e16-docs.html

README.html: README.xml
	$(XMLTO) html $<
	sed -i -e 's/index.html/README.html/' index.html ar*.html
	mv index.html $@

e16-docs.html: e16-docs.1.xml
	$(XMLTO) html $<
	sed -i -e 's/index.html/e16-docs.html/' index.html
	mv index.html $@

MAIN: e16-docs.xml xml2dox.xsl
	xsltproc $(srcdir)/xml2dox.xsl $< > $@

install-data-local:
	mkdir -p $(DESTDIR)$(e16docdir)
	cp *.html $(DESTDIR)$(e16docdir)/

uninstall-local:
	rm -f $(DESTDIR)$(e16docdir)/*.html
	rm -f *.html

endif
