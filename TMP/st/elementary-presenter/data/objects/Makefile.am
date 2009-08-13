AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id $(top_srcdir)/data/objects -fd $(top_srcdir)/data/objects

filesdir = $(datadir)/elementary-presenter/objects

files_DATA = pres.edj \
bg.png l0.png l1.png l2.png l3.png l4.png

EXTRA_DIST = \
pres.edc \
bg.png l0.png l1.png l2.png l3.png l4.png

pres.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/objects/pres.edc \
	$(top_builddir)/data/objects/pres.edj
			
clean-local:
	rm -f *.edj
