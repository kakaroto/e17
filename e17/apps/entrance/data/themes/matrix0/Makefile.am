THEME = matrix0
SCRIPT = buildtheme.sh

CLEANFILES = $(THEME).db
EXTRA_DIST = $(SCRIPT)

themedir = $(pkgdatadir)/themes/$(THEME)
theme_DATA = $(THEME).db $(THEME).bg.db pointer.png \
			extrafrosty.ttf miltown.ttf ocran.ttf

all: $(THEME).db

$(THEME).db: $(SCRIPT)
		sh $<

