MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = images.edc

EDJE_CC = edje_cc
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/images

imagedir = $(pkgdatadir)/images
image_DATA = images.edj

images.edj: Makefile images.edc
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/images.edc \
	$(top_builddir)/data/images.edj
			
clean-local:
	rm -f *.edj
