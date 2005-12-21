MAINTAINERCLEANFILES = Makefile.in
SUBDIRS = fonts

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id images -fd fonts

filesdir = $(datadir)
files_DATA = \
rss.edj \
rss.edc

EXTRA_DIST = $(files_DATA)

rss.edj: Makefile rss.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 rss.edc \
	 rss.edj

clean-local:
	rm -f *.edj
