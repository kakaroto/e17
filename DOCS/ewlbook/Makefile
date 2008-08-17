### cuddletech.com - DocBook XML Document Transformation Makefile
### Created: August 11th, 2003
### Requires: 	FOP (http://xml.apache.org/fop/)
###		 or XMLmind (http://www.xmlmind.com/foconverter/)
###		XSLTPROC (http://xmlsoft.org/XSLT/xsltproc2.html)
###		DocBook XML DTD (http://www.oasis-open.org/docbook/xml/)	
###		DocBook XSL StyleSheets (http://docbook.sourceforge.net/projects/xsl/)
###		LDP XSL StyleSheets (http://my.core.com/9A/2F/dhorton/docbook/tldp-xsl/)

###	Help at: http://tldp.org/LDP/LDP-Author-Guide/
###		 http://docbook.org/tdg/en/html/		<-- The Definative Guide (Published by ORA)
###		 http://www.sagehill.net/docbookxsl/		<-- Excellent Guide!
###		 http://docbook.org/tdg/en/html/part2.html  	<-- DocBook Element Reference


### File without suffix
FILE = ewlbook

#XSLTPROC = /usr/local/bin/xsltproc
XSLTPROC = /usr/bin/xsltproc		# Gentoo

#FOP = /opt/fop-0.20.5/fop.sh
FOP = /usr/bin/fop			# Gentoo

PREFIX = /usr/share/sgml/docbook

###FO_XSL = $(PREFIX)/docbook-xsl-1.61.3/fo/docbook.xsl		## DB
#FO_XSL = $(PREFIX)/docbook-xsl-1.61.3/fo/tldp-print.xsl	## LDP
FO_XSL = $(PREFIX)/xsl-stylesheets-1.66.1/fo/docbook.xsl	# Gentoo DB FO
#FO_XSL = $(PREFIX)/docbook-xsl-1.64.1/fo/docbook.xsl	# Official DB FO

### XSL Arguments: the fop argeument will make FOP play nice, and the 9x7 format is for tech page size.
#FO_ARGS = --stringparam page.height 9in --stringparam page.width 7in --stringparam fop.extensions 1 --stringparam draft.mode no
FO_ARGS = --stringparam fop.extensions 1 --stringparam draft.mode no

#HTML_XSL = $(PREFIX)/docbook-xsl-1.61.3/html/tldp-html.xsl
#HTML_XSL = $(PREFIX)/docbook-xsl-1.64.1/html/docbook.xsl	# Gentoo DB HTML
HTML_XSL = $(PREFIX)/xsl-stylesheets-1.66.1/html/docbook.xsl

all: pdf html

fo:
	$(XSLTPROC) $(FO_ARGS) $(FO_XSL) $(FILE).xml > $(FILE).fo

pdf: fo
	$(FOP) $(FILE).fo -pdf $(FILE).pdf

	## Use PassiveTeX instead!: (BROKEN)
	# mv $(FILE).fo passivetex.fo
	# pdflatex "&pdfxmltex" passivetex.fo
	# mv passivetex.pdf $(FILE).pdf

	rm -f $(FILE).fo 

txt:
	$(XSLTPROC) $(FO_XSL) $(FILE).xml > $(FILE).fo
		$(FOP) $(FILE).fo -txt $(FILE).txt
	rm -f $(FILE).fo 

html:
	$(XSLTPROC) -o $(FILE).html $(HTML_XSL) $(FILE).xml

clean:
	echo "REMOVING EVERYTHING BUT MAKEFILE AND XML SOURCE!!!!!"
	rm -f $(FILE).html
	rm -f $(FILE).pdf $(FILE).fo
	rm -f $(FILE).txt
	rm -f passive*

check: 
	xmllint --valid --noout $(FILE).xml

