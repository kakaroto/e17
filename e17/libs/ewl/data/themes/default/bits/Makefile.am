SUBDIRS = images

EDCS = button-groups.edc button-images.edc button-macro.edc \
       button-programs.edc button.edc \
       checkbox-groups.edc checkbox-images.edc \
       checkbox-programs.edc checkbox.edc \
       entry-groups.edc entry-images.edc entry.edc \
       filedialog-groups.edc filedialog-images.edc filedialog.edc \
       images.edc \
       misc-groups.edc misc-images.edc misc.edc \
       menu-groups.edc menu-macro.edc \
       progressbar-groups.edc progressbar-images.edc progressbar.edc \
       radio-groups.edc radio-images.edc radio-programs.edc radio.edc \
       scrollbar-groups.edc scrollbar-images.edc scrollbar.edc \
       window-groups.edc window-images.edc window.edc

EETS = $(EDCS:%.edc=%.eet)

EXTRA_DIST = $(EDCS)
CLEANFILES = $(EETS)

themedir = $(pkgdatadir)/themes/default/bits
#theme_DATA = $(EETS)

#all: $(EETS)

#SUFFIXES = .edc .eet
#.edc.eet:
#	edje_cc -v -id images $< $@
