MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I. \
	 -I$(top_srcdir) \
	 -I$(includedir) \
	 @e_cflags@ @SOUND_CFLAGS@

pkgdir = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES = module.la
module_la_SOURCES = e_mod_main.h \
		    e_mod_main.c \
                    e_mod_config.c \
                    e_mod_system.h \
                    e_mod_system.c

if BUILD_ALSA
module_la_SOURCES += e_mod_alsa.h \
                     e_mod_alsa.c
endif

module_la_LIBADD = @e_libs@ @SOUND_LDFLAGS@
module_la_LDFLAGS = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

clean-local:
	rm -rf *~
