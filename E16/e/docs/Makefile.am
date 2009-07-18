e16docdir = ${datadir}/doc/${PACKAGE}

EXTRA_DIST = README README.xml e16-docs.xml e16-docs-src.xml xml2dox.xsl \
             e16.1 README.html e16-docs.html

CLEANFILES = MAIN
MAINTAINERCLEANFILES = e16-docs.xml e16.1 README.html e16-docs.html

e16-docs.xml: e16-docs-src.xml
	grep -v -e "^<font" -e "^<img" -e "more on next page" $< > $@

e16.1: e16-docs.xml
	$(XMLTO) man $<

README.html: README.xml
	$(XMLTO) html-nochunks $<

e16-docs.html: e16-docs.xml
	$(XMLTO) html-nochunks $<

if INSTALL_MANS

dist_man_MANS = e16.1

endif

if INSTALL_DOCS

dist_e16doc_DATA = README.html e16-docs.html

endif

MAIN: e16-docs-src.xml xml2dox.xsl
	xsltproc $(srcdir)/xml2dox.xsl $< > $@
