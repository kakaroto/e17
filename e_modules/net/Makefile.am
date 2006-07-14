MAINTAINERCLEANFILES = Makefile.in e_modules-net.spec

SUBDIRS = po

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v \
-id $(top_srcdir) \
-fd $(top_srcdir)

# data files for the module
filesdir = $(datadir)

files_DATA = module_icon.png \
			module.eap \
			net.edj

EXTRA_DIST=$(files_DATA) \
			e_modules-net.spec \
			net.edc \
			rx_00.png \
			rx_01.png \
			rx_02.png \
			rx_03.png \
			rx_04.png \
			rx_05.png \
			rx_06.png \
			rx_07.png \
			rx_08.png \
			rx_09.png \
			rx_10.png \
			tx_00.png \
			tx_01.png \
			tx_02.png \
			tx_03.png \
			tx_04.png \
			tx_05.png \
			tx_06.png \
			tx_07.png \
			tx_08.png \
			tx_09.png \
			tx_10.png \
			VeraMono.ttf

# the module .so file
INCLUDES = -I. \
	 -I$(top_srcdir) \
	 -I$(includedir) \
	 @e_cflags@

pkgdir                 = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
		       e_mod_main.h \
		       e_mod_config.c
		       
module_la_LIBADD       = @e_libs@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h $(top_builddir)/net.edj

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf *.edj *~

