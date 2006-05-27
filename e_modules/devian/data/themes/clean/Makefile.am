EDJE_CC = @edje_cc@
EDJE_FLAGS = -v \
	-id images \
	-fd fonts

EXTRA_DIST =    ./clean.edc \
		./clean_popup/clean_popup.edc \
		./clean_popup/clean_popup_parts.edc \
		./clean_popup/clean_popup_programs.edc \
		./clean_rss/clean_rss.edc \
		./clean_rss/clean_rss_parts.edc \
		./clean_rss/clean_rss_programs.edc \
		./clean_box/clean_box.edc \
		./clean_box/clean_box_parts.edc \
		./clean_box/clean_box_programs.edc \
		./fonts/VeraBd.ttf \
		./fonts/Vera.ttf \
		$(wildcard ./images/*.png)

filesdir = $(datadir)/themes/

files_DATA = clean.edj

clean.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/clean/clean.edc \
	$(top_builddir)/data/themes/clean/clean.edj

clean:
	rm -f clean.edj
