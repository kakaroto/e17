DB = default.eet
CC = edje_cc
INCLUDES = -v -id ../../images

CLEANFILES = $(DB)

defaultdir = $(pkgdatadir)/themes
default_DATA = $(DB)

all: $(DB)

$(DB): default.edc
	$(CC) $(INCLUDES) $< $@
