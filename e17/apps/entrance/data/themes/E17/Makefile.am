EET = E17.eet
SCRIPT = build_theme.sh

CLEANFILES = $(EET)
EXTRA_DIST = $(SCRIPT)

E17dir = $(pkgdatadir)/themes
E17_DATA = $(EET)

all: $(EET)

$(EET): $(SCRIPT)
		sh $<
