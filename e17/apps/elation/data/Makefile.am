EDJE_CC = edje_cc $(EDJE_FLAGS)
EDJE_FLAGS = -v

AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

filesdir = $(datadir)/elation/data
files_DATA = \
theme.edj \
Vera.ttf \
VeraBd.ttf \
bg.png \
disk.png

EXTRA_DIST = \
Vera.ttf \
VeraBd.ttf \
bg.png \
disk.png \
theme.edc

theme.edj: Makefile
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/theme.edc \
	$(top_builddir)/data/theme.edj

clean-local:
	rm -f *.edj
