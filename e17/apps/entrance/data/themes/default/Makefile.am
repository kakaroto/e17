DB = default.eet
SCRIPT = build_theme.sh

CLEANFILES = $(DB)
EXTRA_DIST = $(SCRIPT)

defaultdir = $(pkgdatadir)/themes
default_DATA = $(DB)

all: $(DB)

$(DB): $(SCRIPT)
		sh $<
