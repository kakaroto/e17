MAINTAINERCLEANFILES = Makefile.in
SUBDIRS = images 

moduledir = @MODULEDIR@

EDJE_CC = @edje_cc@
EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/themes/images


# data files for the module
filesdir = $(moduledir)/$(PACKAGE)
files_DATA = \
module.desktop \
module.edj \
moon.edj

EXTRA_DIST = \
moon.edc \
icon.edc

moon.edj: Makefile $(EXTRA_DIST)
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 $(top_srcdir)/data/themes/moon.edc \
	 moon.edj

module.edj: Makefile $(EXTRA_DIST)
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 $(top_srcdir)/data/themes/icon.edc \
	 module.edj

clean-local:
	 rm -rf moon.edj; rm -rf *.desktop

