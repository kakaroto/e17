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
moon.eap \
moon.edj

EXTRA_DIST = \
moon.edc \
icon.edc

moon.edj: Makefile $(EXTRA_DIST)
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 $(top_srcdir)/data/themes/moon.edc \
	 moon.edj

moon.eap: Makefile $(EXTRA_DIST)
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 $(top_srcdir)/data/themes/icon.edc \
	 moon.eap

clean-local:
	 rm -rf *.edj; rm -rf *.eap

