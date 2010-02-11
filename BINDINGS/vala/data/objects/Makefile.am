include $(top_srcdir)/Makefile.decl

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id $(top_srcdir)/data/objects -fd $(top_srcdir)/data/objects

filesdir = $(datadir)/libeflvala/objects

files_DATA = test.edj

EXTRA_DIST += \
test.edc over.png under.png sky.jpg clo.png

test.edj: Makefile $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/objects/test.edc \
	$(top_builddir)/data/objects/test.edj
			
clean-local:
	rm -f *.edj

CLEANFILES = Makefile test.edj

MAINTAINERCLEANFILES = Makefile.in
