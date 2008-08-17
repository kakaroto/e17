DB = default.eet
CC = edje_cc
INCLUDES = -v -id ../../images -fd ../../fonts

CLEANFILES = $(DB)

defaultdir = $(pkgdatadir)/themes
default_DATA = $(DB)

all: $(DB)

$(DB): default.edc
	$(CC) $(INCLUDES) default.edc $@
