MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v \
	-id images \
	-fd fonts

EXTRA_DIST = \
module.edc \
module_feed.edc \
module_feed_parts.edc \
module_feed_programs.edc \
module_feedone.edc \
module_feedone_parts.edc \
module_feedone_programs.edc \
module_icon.edc \
module_item.edc \
module_item_parts.edc \
module_item_programs.edc \
module_popw.edc \
module_popw_parts.edc \
module_popw_programs.edc \
module_viewer.edc \
module_viewer_parts.edc \
module_viewer_programs.edc

filesdir = $(datadir)
files_DATA = e-module-news.edj

e-module-news.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/theme/module.edc \
	$(top_builddir)/data/theme/e-module-news.edj

clean-local:
	rm -f e-module-news.edj
