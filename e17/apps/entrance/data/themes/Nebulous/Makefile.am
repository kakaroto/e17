EET = Nebulous.eet
SCRIPT = build_theme.sh

CLEANFILES = $(EET)
EXTRA_DIST = $(SCRIPT) Nebulous.edc

defaultdir = $(pkgdatadir)/themes
default_DATA = $(EET)

all: $(EET)

$(EET): $(SCRIPT)
		sh $< $(top_srcdir)/data/themes/Nebulous
