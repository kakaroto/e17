SUBDIRS = images fonts

THEME = trakvision
EDC = $(THEME).edc
EET = $(EDC:%.edc=%.eet)
SUFFIXES = .edc .eet

CLEANFILES = $(EET)
EXTRA_DIST = $(EDC) trakvision_images.edc trakvision_parts.edc \
             trakvision_progs.edc trakvision_fonts.edc

themedir = $(pkgdatadir)/themes
theme_DATA = $(EET)

all: $(EDC)

.edc.eet:
	edje_cc -v -id images -fd fonts $< $@

preview: $(EET)
	edje $(EET) envision
