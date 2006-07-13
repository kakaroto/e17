EDJE_CC = @edje_cc@
EDJE_FLAGS = -v \
	-id $(top_srcdir)/data/themes/blue_eyed \
	-fd $(top_srcdir)/data/themes/blue_eyed

EXTRA_DIST =    ./blue_eyed.edc \
		./readme.edc \
		$(wildcard fonts/*.ttf) \
		$(wildcard edjes/*.ed?) \
		$(wildcard edjes/parts/*.ed?) \
		$(wildcard edjes/programs/*.ed?) \
		$(wildcard edjes/scripts/*.ed?) \
		$(wildcard images/*.png)

filesdir = $(datadir)/themes/

files_DATA = blue_eyed.edj

blue_eyed.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/themes/blue_eyed/blue_eyed.edc \
	$(top_builddir)/data/themes/blue_eyed/blue_eyed.edj

clean-local:
	rm -f ./blue_eyed.edj *~
