# To create all the document types you need python and docutils.
# To create PDFs you need pdflatex from texlive.

FILE = devwithedje

all: html latex pdf

html:
	rst2html.py --cloak-email-addresses --stylesheet=devwithedje.css  $(FILE).txt $(FILE).html
	cp -f $(FILE).html pre-rendered

latex: 
	rst2latex.py --use-latex-toc --documentclass=book --documentoptions=12pt,a4paper --stylesheet=stylesheet.tex $(FILE).txt $(FILE).tex
	cp -f $(FILE).tex pre-rendered
 
pdf:
	pdflatex $(FILE).tex
	pdflatex $(FILE).tex #the second run is required for internal references
	cp -f $(FILE).pdf pre-rendered

clean:
	@echo "Removing files..."
	rm -f $(FILE).html
	rm -f $(FILE).tex
	rm -f $(FILE).log
	rm -f $(FILE).out
	rm -f $(FILE).pdf
	rm -f $(FILE).aux
	rm -f $(FILE).toc