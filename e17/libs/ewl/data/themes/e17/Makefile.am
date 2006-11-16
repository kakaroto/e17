MAINTAINERCLEANFILES = Makefile.in

EDCS =  e17.edc
EDJS = $(EDCS:%.edc=%.edj)

e17_DEPS = \
	groups/border-groups.edc \
	groups/button-groups.edc \
	groups/debug-groups.edc \
	groups/entry-groups.edc \
	groups/label-groups.edc \
	groups/menu-groups.edc \
	groups/menu-item.edc \
	groups/paned-groups.edc \
	groups/progressbar-groups.edc \
	groups/rowtext-groups.edc \
	groups/scrollbar-groups.edc \
	groups/scrollpane-groups.edc \
	groups/selection-groups.edc \
	groups/separator-groups.edc \
	groups/shadow-groups.edc \
	groups/spacer-groups.edc \
	groups/spinner-groups.edc \
	groups/stock-groups.edc \
	groups/tree-groups.edc \
	groups/treebg-groups.edc \
	groups/treehead-groups.edc \
	groups/window-groups.edc

EXTRA_DIST = $(EDCS) $(basename $(EDCS))
CLEANFILES = $(EDJS)

themedir = $(pkgdatadir)/themes/
theme_DATA = $(EDJS)

all: $(EDJS)

e17.edj: ${e17_DEPS}

SUFFIXES = .edc .edj
.edc.edj:
	edje_cc -v -id $(top_srcdir)/data/themes/$(notdir $(<:%.edc=%))/images -fd $(top_srcdir)/data/themes/$(notdir $(<:%.edc=%)) $(top_srcdir)/data/themes/$(notdir $(<:%.edc=%))/$< $(top_builddir)/data/themes/$(notdir $(<:%.edc=%))/$(notdir $@)

