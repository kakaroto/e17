EXTRA_DIST= artifakt.edc images.edc fonts.edc
SUBDIRS = parts programs images fonts
DB = artifakt.eet
CC = edje_cc
INCLUDES = -v -id ./images -fd ./fonts

CLEANFILES = $(DB)

artifaktdir = $(pkgdatadir)/themes
artifakt_DATA = $(DB)

all: $(DB)

$(DB): artifakt.edc
	$(CC) $(INCLUDES) artifakt.edc $@
