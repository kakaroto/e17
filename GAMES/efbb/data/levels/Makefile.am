MAINTAINERCLEANFILES = Makefile.in

LEVEL_CREATOR = $(top_srcdir)/tools/levels_create.py
LEVELS = $(top_srcdir)/data/levels

filesdir = $(datadir)/efbb/

files_DATA = levels.eet

EXTRA_DIST = \
	levels.list \
	level00.cfg \
	level01.cfg \
	level02.cfg \
	level03.cfg \
	level04.cfg \
	level05.cfg
	$(LEVEL_CREATOR)

levels.eet: Makefile $(EXTRA_DIST)
	$(LEVEL_CREATOR) $(LEVELS) $(top_srcdir)/data/levels/levels.eet

clean-local:
	rm -f *.eet
