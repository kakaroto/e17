EXTRA_DIST= default.edc images.edc macros/slider.edc embryo/globals.edc parts/status.edc programs/status.edc programs/enticepreview.edc
SUBDIRS = parts programs
DB = default.eet
CC = edje_cc
INCLUDES = -v -id ../../images -fd ../../fonts

CLEANFILES = $(DB)

defaultdir = $(pkgdatadir)/themes
default_DATA = $(DB)

all: $(DB)

$(DB): default.edc
	$(CC) $(INCLUDES) default.edc $@
