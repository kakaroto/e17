AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id $(top_srcdir)/data/themes -fd $(top_srcdir)/data/themes

filesdir = $(datadir)/elmdentica/themes

files_DATA = default.edj

EXTRA_DIST = \
	default.edc			\
	bubble_left.png			\
	bubble_3.png			\
	pitr_green_menu_icon_set_1.png	\
	pitr_green_menu_icon_set_1.svg	\
	pitr_red_menu_icon_set_3.png	\
	pitr_red_menu_icon_set_3.svg	\
	pitr_green_menu_icon_set_3.png	\
	pitr_green_menu_icon_set_3.svg	\
	pitr_red_menu_icon_set_2.png	\
	pitr_red_menu_icon_set_2.svg	\
	edit-undo.png			\
	edit-undo.svg			\
	Envelope_closed.png		\
	Envelope_closed.svg

default.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/default.edc \
	$(top_builddir)/data/themes/default.edj
                        
clean-local:
	rm -f *.edj
