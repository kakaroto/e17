EXTRA_DIST= taillights.edc
SUBDIRS = parts programs fonts images
DB = taillights.eet
CC = edje_cc
INCLUDES = -v -id ./images -fd ./fonts

CLEANFILES = $(DB)

taillightsdir = $(pkgdatadir)/themes
taillights_DATA = $(DB)

all: $(DB)

$(DB): taillights.edc
	(cd $(top_srcdir)/data/themes/taillights ; $(CC) $(INCLUDES) taillights.edc $(top_builddir)/data/themes/taillights/$@)
