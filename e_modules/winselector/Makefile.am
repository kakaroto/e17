MAINTAINERCLEANFILES = Makefile.in mkinstalldirs

EDJE_FLAGS = -v \
-id $(top_srcdir) \
-fd $(top_srcdir)

# data files for the module
filesdir =  $(datadir)

files_DATA = module.desktop \
	     e-module-winselector.edj \
	     winselector.edj

EXTRA_DIST = $(files_DATA) \
	     module_icon.png \
	     winselector.edc \
	     e17_titlebar.png \
	     e17_titlebar_glint1.png \
	     e17_titlebar_glint2.png \
	     e17_titlebar_glint3.png \
	     e17_titlebar_shadow1.png \
	     e17_titlebar_shadow2.png \
	     VeraBd.ttf

# the module .so file
INCLUDES = -I. \
			  -I$(top_srcdir) \
			  -I$(includedir) \
			   @e_cflags@

pkgdir                 =  $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
		         e_mod_main.h
			 
module_la_LIBADD       = @e_libs@ -lm
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h $(top_builddir)/winselector.edj

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf winselector.edj *~
