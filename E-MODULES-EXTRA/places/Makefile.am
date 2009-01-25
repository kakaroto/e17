MAINTAINERCLEANFILES = Makefile.in aclocal.m4 ABOUT-NLS config.h.in \
   config.guess config.rpath config.sub configure depcomp install-sh \
   ltmain.sh missing module.desktop mkinstalldirs config.h.in

SUBDIRS = src po

EDJE_FLAGS = -v \
	   -id $(top_srcdir)/images \
	   -fd $(top_srcdir)/fonts

# data files for the module
filesdir =  $(datadir)

files_DATA = module.desktop \
	     e-module-places.edj

EXTRA_DIST = $(files_DATA) \
	     images/eject.png \
	     images/eject_bg.png \
	     images/icon_dvd.png \
	     images/icon_hdd.png \
	     images/icon_ipod.png \
	     images/icon_ms.png \
	     images/icon_sdmmc.png \
	     images/icon_psp.png \
	     images/tag_ext3.png \
	     images/tag_fat.png \
	     images/tag_hfs.png

ACLOCAL_AMFLAGS = -I m4

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf e-module-places.edj module.desktop *~
uninstall:
	 rm -rf $(DESTDIR)$(datadir)
