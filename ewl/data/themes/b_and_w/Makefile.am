MAINTAINERCLEANFILES = Makefile.in
SUBDIRS = groups \
	  fonts \
	  images

EDCS =  b_and_w.edc
EDJS = $(EDCS:%.edc=%.edj)

b_and_w_DEPS = \
	groups/blank.edc \
        groups/border.edc \
        groups/button.edc \
        groups/combo.edc \
	groups/cursor.edc \
	groups/entry.edc \
	groups/image.edc \
        groups/label.edc \
        groups/menu.edc \
        groups/notebook.edc \
	groups/paned.edc \
        groups/progressbar.edc \
	groups/rectangle.edc \
        groups/scroll.edc \
	groups/scrollpane.edc \
        groups/shadow.edc \
        groups/spacer.edc \
        groups/tree.edc \
        groups/window.edc

EXTRA_DIST = $(EDCS) 

CLEANFILES = $(EDJS)

themedir = $(pkgdatadir)/themes/
theme_DATA = $(EDJS)

all: $(EDJS)

b_and_w.edj: ${b_and_w_DEPS}

SUFFIXES = .edc .edj
.edc.edj:
	edje_cc -v -id $(top_srcdir)/data/themes/$(*F)/images -fd $(top_srcdir)/data/themes/$(*F) $(top_srcdir)/data/themes/$(*F)/$< $(top_builddir)/data/themes/$(*F)/$(@F)

