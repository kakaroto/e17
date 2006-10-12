SUBDIRS = po

MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v \
-id $(top_srcdir) \
-fd $(top_srcdir)

files_DATA = \
module.desktop \
module.edj \
mixer.edj

EXTRA_DIST = \
$(files_DATA) \
mixer.edc \
mixer-volume-high.png \
mixer-volume-medium.png \
mixer-volume-low.png \
mixer-volume-muted.png \
e_modules-mixer.spec.in \
e_modules-mixer.spec

# data files for the module
filesdir = $(datadir)

# the module .so file
INCLUDES = -I. \
	 -I$(top_srcdir) \
	 -I$(includedir) \
	 @SOUND_CFLAGS@ \
	 @e_cflags@

pkgdir                 = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
		       e_mod_main.h \
		       e_mod_types.h \
		       alsa_mixer.h \
		       alsa_mixer.c \
		       oss_mixer.h \
		       oss_mixer.c \
		       e_mod_config.c

module_la_LIBADD       = @e_libs@ @SOUND_LDFLAGS@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

mixer.edj:  mixer.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf mixer.edj *~
uninstall:
	rm -rf $(datadir)
