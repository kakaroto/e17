EDCS =  button.edc \
	checkbox.edc \
	entry.edc \
	misc.edc \
	radio.edc \
	scrollbar.edc \
	window.edc

EETS = $(EDCS:%.edc=%.eet)

EXTRA_DIST = $(EDCS)
CLEANFILES = $(EETS)

themedir = $(pkgdatadir)/themes/default/bits
theme_DATA = $(EETS)

all: $(EETS)

SUFFIXES = .edc .eet
.edc.eet:
	edje_cc -v -id images $< $@
