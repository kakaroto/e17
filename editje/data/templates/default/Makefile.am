MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = 

filesdir = $(datadir)/editje/data/templates
files_DATA = \
default.edj

EXTRA_DIST = \
default.edc

default.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/templates/default/default.edc \
	$(top_builddir)/data/templates/default/default.edj

clean-local:
	rm -f *.edj
