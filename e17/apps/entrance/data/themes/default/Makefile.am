EET = default.eet
SCRIPT = build_theme.sh

CLEANFILES = $(EET)
EXTRA_DIST = $(SCRIPT)

defaultdir = $(pkgdatadir)/themes
default_DATA = $(EET)

all: $(EET)

$(EET): $(SCRIPT)
		sh $<
