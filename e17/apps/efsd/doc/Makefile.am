## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES = Makefile.in 

EXTRA_DIST = efsd-manual.sgml

MANUAL = $(top_srcdir)/efsd/libefsd.h

if HAVE_DOCGEN
efsd-manual.sgml: efsd-manual.raw $(MANUAL)
	TOPDIR=@LINUX_SRC@ @DOCGEN@ $(MANUAL) <efsd-manual.raw >efsd-manual.sgml
endif

if HAVE_JADE

html = efsd-manual-$(VERSION)

html-manual: efsd-manual.sgml html-customizations.dsl
	$(mkinstalldirs) ./$(html)/figures
	cd $(html) && @JADE@ -t sgml -d ../html-customizations.dsl ../efsd-manual.sgml
	cd figures && for file in *.gif; do \
	  cp -f $$file ../$(html)/figures/$$file; \
	done
	cp -f stylesheet.css $(html)/stylesheet.css
	tar cfvz efsd-manual-$(VERSION).tar.gz $(html)

html-filetypes: filetypes.xsl
	$(mkinstalldirs) ./$(html)/filetypes
	cp -fr filetypes-templates/* ./$(html)/filetypes
	./generate-trees.sh ./$(html)/filetypes

endif

