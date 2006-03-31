MAINTAINERCLEANFILES	= Makefile.in
INCLUDES		= -I$(includedir) \
				  -I$(top_srcdir)/src/bin 


AM_CFLAGS 		= @ECORE_CFLAGS@  -DMODULE_ARCH="\"@MODULE_ARCH@\""




#lib_LTLIBRARIES				= libemixer.la
noinst_LTLIBRARIES 			= 	libemixer.la
libemixer_la_SOURCES			=	e_mixer.c \
								e_mixer.h

libemixer_la_DEPENDENCIES	= $(top_builddir)/config.h


pkgdir 						= @E_MODULES@/$(PACKAGE)/$(MODULE_ARCH)/systems/
pkg_LTLIBRARIES       		= module_alsa.la

module_alsa_la_LDFLAGS      = -module -avoid-version
module_alsa_la_SOURCES		= alsa_mixer.c \
								alsa_mixer.h

module_alsa_la_LIBADD		= @ECORE_LIBS@ @EVAS_LIBS@ @alsa_libs@
module_alsa_la_DEPENDENCIES	= $(top_builddir)/config.h
module_alsa_la_CFLAGS		= @alsa_cflags@ @ECORE_CFLAGS@

