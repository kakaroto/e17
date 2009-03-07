ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in e_modules-mem.spec

SUBDIRS = po

EDJE_FLAGS = -v \
-id $(top_srcdir) \
-fd $(top_srcdir)

# data files for the module
filesdir = $(datadir)

files_DATA = module.desktop \
			e-module-mem.edj \
			mem.edj

EXTRA_DIST= e_modules-mem.spec \
			$(files_DATA) \
			mem.edc \
			module_icon.png \
			real_00.png \
			real_01.png \
			real_02.png \
			real_03.png \
			real_04.png \
			real_05.png \
			real_06.png \
			real_07.png \
			real_08.png \
			real_09.png \
			real_10.png \
			swap_00.png \
			swap_01.png \
			swap_02.png \
			swap_03.png \
			swap_04.png \
			swap_05.png \
			swap_06.png \
			swap_07.png \
			swap_08.png \
			swap_09.png \
			swap_10.png \
			DejaVuSansMono.ttf

# the module .so file
INCLUDES = -I. \
	 -I$(top_srcdir) \
	 -I$(includedir) \
	 @E_CFLAGS@

pkgdir                 = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
if BUILD_FREEBSD
module_la_SOURCES	=	e_mod_main.c \
				e_mod_main.h \
				machdep_freebsd.c \
				e_mod_config.c
endif
if BUILD_LINUX
module_la_SOURCES	=	e_mod_main.c \
				e_mod_main.h \
				machdep_linux.c \
				e_mod_config.c
endif

EXTRA_module_la_SOURCES	=	machdep_linux.c \
				machdep_freebsd.c


module_la_LIBADD       = @E_LIBS@ @machdep_libs@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h $(top_builddir)/mem.edj

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf mem.edj *~
