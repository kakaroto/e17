MAINTAINERCLEANFILES = Makefile.in

SUBDIRS =

EDJE_FLAGS = -v \
-id $(top_srcdir) \
-fd $(top_srcdir)

filesdir = $(datadir)
files_DATA = e-module-mpdule.edj \
	     module.desktop \
	     mpdule.edj

EXTRA_DIST = $(files_DATA) \
			e_modules-mpdule.spec \
			mpdule.edc \
			VeraMono.ttf

CONFIG_CLEAN_FILES = e_modules-mpdule.spec mpdule.edj

# the module .so file
INCLUDES               = -I. \
			 -I$(top_srcdir) \
			 -I$(includedir) \
			 @E_CFLAGS@ @MPD_CFLAGS@

pkgdir                 = $(datadir)/$(MODULE_ARCH)

pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
		         e_mod_main.h \
			 e_mod_config.c
module_la_LIBADD       = @E_LIBS@ @MPD_LIBS@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h $(top_builddir)/mpdule.edj

mpdule.edj:  mpdule.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf mpdule.edj *~
