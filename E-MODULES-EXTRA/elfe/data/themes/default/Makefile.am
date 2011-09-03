
EDJE_FLAGS = -v \
	     -id $(top_srcdir)/data/themes/default/images

filesdir = $(datadir)
files_DATA = default.edj

IMGS= \
	images/*.png

EXTRA_DIST = default.edc \
	elm.edc \
	$(IMGS)

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -rf default.edj *~

uninstall:
	rm -rf $(DESTDIR)$(datadir)
