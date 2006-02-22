MAINTAINERCLEANFILES = Makefile.in

EDCS =  e17.edc
#EDCS = default.edc skeleton.edc zero.edc e17.edc

EDJS = $(EDCS:%.edc=%.edj)

e17_DEPS = \
	e17/bits/border-groups.edc \
	e17/bits/box-groups.edc \
	e17/bits/button-groups.edc \
	e17/bits/debug-groups.edc \
	e17/bits/entry-groups.edc \
	e17/bits/label-groups.edc \
	e17/bits/menu-groups.edc \
	e17/bits/menu-item.edc \
	e17/bits/paned-groups.edc \
	e17/bits/progressbar-groups.edc \
	e17/bits/rowtext-groups.edc \
	e17/bits/scrollbar-groups.edc \
	e17/bits/selection-groups.edc \
	e17/bits/separator-groups.edc \
	e17/bits/spacer-groups.edc \
	e17/bits/spinner-groups.edc \
	e17/bits/stock-groups.edc \
	e17/bits/tree-groups.edc \
	e17/bits/treebg-groups.edc \
	e17/bits/treehead-groups.edc \
	e17/bits/window-groups.edc

EXTRA_DIST = $(EDCS) $(basename $(EDCS))
CLEANFILES = $(EDJS)

themedir = $(pkgdatadir)/themes/
theme_DATA = $(EDJS)

all: $(EDJS)

e17.edj: ${e17_DEPS}

SUFFIXES = .edc .edj
.edc.edj:
	edje_cc -v -id $(top_srcdir)/data/themes/$(notdir $(<:%.edc=%))/bits/images  -fd $(top_srcdir)/data/themes $(top_srcdir)/data/themes/$< $(top_builddir)/data/themes/$(notdir $@)
