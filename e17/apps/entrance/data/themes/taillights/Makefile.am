EXTRA_DIST= taillights.edc images.edc
SUBDIRS = parts programs fonts images
DB = taillights.eet
CC = edje_cc
INCLUDES = -v -id ./images -fd ./fonts

CLEANFILES = $(DB)

taillightsdir = $(pkgdatadir)/themes
taillights_DATA = $(DB)

all: $(DB)

$(DB): taillights.edc
	$(CC) $(INCLUDES) taillights.edc $@
