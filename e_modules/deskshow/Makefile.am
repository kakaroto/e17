MAINTAINERCLEANFILES = Makefile.in mkinstalldirs

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v

# data files for the module
filesdir =  $(datadir)

files_DATA = module_icon.png module.eap deskshow.edj $(wildcard e17*.png)

EXTRA_DIST = $(files_DATA) deskshow.edc VeraBd.ttf

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
module_la_DEPENDENCIES = $(top_builddir)/config.h

deskshow.edj: Makefile deskshow.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 deskshow.edc \
	 deskshow.edj

clean-local:
	 rm -rf *.edj *~
