THEME = matrix0
SCRIPT = buildtheme.sh

CLEANFILES = $(THEME).db
EXTRA_DIST = $(SCRIPT)

themedir = $(pkgdatadir)/themes/$(THEME)
theme_DATA = $(THEME).db $(THEME).bg.db pointer.png \
			abadi.ttf extrafrosty.ttf elegante.ttf frosty.ttf miltown.ttf swissbc.ttf

all: $(THEME).db

$(THEME).db: $(SCRIPT)
		sh $<

