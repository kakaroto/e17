MAINTAINERCLEANFILES = Makefile.in
SUBDIRS = images 

EDJE_FLAGS = \
-v \
-id $(top_srcdir)/data/themes/images


# data files for the module
filesdir = $(datadir)
files_DATA = \
module.desktop \
e-module-moon.edj \
moon.edj

EXTRA_DIST = \
module.desktop \
moon.edc \
icon.edc

moon.edj: Makefile $(EXTRA_DIST)
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 $(top_srcdir)/data/themes/moon.edc \
	 moon.edj

e-module-moon.edj: Makefile $(EXTRA_DIST)
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 $(top_srcdir)/data/themes/icon.edc \
	 e-module-moon.edj

clean-local:
	 rm -rf *.edj
