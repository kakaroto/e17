## $Id$

SUBDIRS = fonts images parts programs

THEME = default
EDC = $(THEME).edc embrace.edc mailbox.edc
EET = $(THEME).eet

CLEANFILES = $(EET)
EXTRA_DIST = $(EDC)

themedir = $(pkgdatadir)/themes
theme_DATA = $(EET)

all: $(EET)

$(EET): $(EDC)
	edje_cc -v -fd fonts -id images $< $(EET)

preview: $(EET)
	edje $(EET) embrace
