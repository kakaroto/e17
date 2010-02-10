.PHONY: clean all
.PRECIOUS: %.pdf

PDFS = efl_big_picture.pdf
FIGURE_DIR = images
FIGURES = rel1rel2simple.pdf rel1rel2-a.pdf rel1rel2-b.pdf align.pdf

all: $(PDFS)

clean:
	$(RM) $(PDFS) $(FIGURES) *.aux *.dvi *.idx *.out *.toc *.log *.eps *~

INKSCAPE_EXPORT_OPTS = --export-dpi=300 --export-area-drawing --export-area-snap

%.eps: $(FIGURE_DIR)/%.svg Makefile
	inkscape $< --export-eps=$@ $(INKSCAPE_EXPORT_OPTS)

%.pdf: %.eps Makefile
	epstopdf $< --outfile=$@

%.pdf: %.tex profusion.cls logo-profusion.pdf $(FIGURES) Makefile
	xelatex -output-driver "xdvipdfmx -V5 -z9 -E" $<
	xelatex -output-driver "xdvipdfmx -V5 -z9 -E" $< # references...
