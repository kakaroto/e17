SUBDIRS = images fonts

THEME = trakvision
EDC = $(THEME).edc
EDJ = $(EDC:%.edc=%.edj)
SUFFIXES = .edc .edj

CLEANFILES = $(EDJ)
EXTRA_DIST = $(EDC) trakvision_images.edc trakvision_parts.edc \
             trakvision_progs.edc trakvision_fonts.edc

themedir = $(pkgdatadir)/themes
theme_DATA = $(EDJ)

all: $(EDC)

.edc.edj:
	edje_cc -v -id images -fd fonts $< $@

preview: $(EDJ)
	edje $(EDJ) envision
