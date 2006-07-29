# DocBook XML Document Transformation Makefile
# Required tools:
# - xmlto (Available as Debian package)
# - fop (Available as Debian package)
# - Jimi Image library which is available from Sun at http://xmlgraphics.apache.org/fop/0.20.5/graphics.html#packages
#
# You are also advised to install all xml-related Debian packages
# The book "Docbook the Definitive GUIDE" is also available as a Debian package
#
# Many thanks to Ben Rockwood for his help.
#
# Kostis Kapelonis
# Summer 2006


### File without suffix
FILE = concepts

FOP = fop
XMLTO = xmlto

all: pdf html html_single

fo: 
	$(XMLTO) fo $(FILE).xml

pdf: fo
	$(FOP) $(FILE).fo -pdf $(FILE).pdf
	rm -f $(FILE).fo 

txt:
	$(XMLTO) txt $(FILE).xml

html:
	$(XMLTO) xhtml $(FILE).xml

html_single:
	$(XMLTO) xhtml-nochunks $(FILE).xml

clean:
	@echo "REMOVING EVERYTHING BUT MAKEFILE AND XML SOURCE!!!!!"
	rm -f *.html
	rm -f $(FILE).pdf $(FILE).fo
	rm -f $(FILE).txt

check: 
	xmllint --valid --noout $(FILE).xml

