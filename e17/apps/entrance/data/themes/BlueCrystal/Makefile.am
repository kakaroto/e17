THEME = BlueCrystal
SCRIPT = buildtheme.sh

CLEANFILES = $(THEME).db
EXTRA_DIST = $(SCRIPT)

themedir = $(pkgdatadir)/themes/$(THEME)
theme_DATA = $(THEME).db $(THEME).bg.db bedrock.ttf \
			casual.ttf notepad.ttf orbit.ttf pointer.png

all: $(THEME).db

$(THEME).db: $(SCRIPT)
		sh $<

