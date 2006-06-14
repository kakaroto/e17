MAINTAINERCLEANFILES = Makefile.in e_modules-mem.spec


SUBDIRS = po

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v

# data files for the module
filesdir = $(datadir)

files_DATA = module_icon.png module.eap mem.edj mem.edc \
             DejaVuSansMono.ttf $(wildcard real*.png) $(wildcard swap*.png)

EXTRA_DIST=e_modules-mem.spec.in e_modules-mem.spec autogen.sh mem.png module_icon.png $(files_DATA)

# the module .so file
INCLUDES = -I. \
	 -I$(top_srcdir) \
	 -I$(includedir) \
	 @e_cflags@

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
if BUILD_NETBSD
module_la_SOURCES	=	e_mod_main.c \
				e_mod_main.h \
				machdep_netbsd.c \
				e_mod_config.c
endif

EXTRA_module_la_SOURCES	=	machdep_linux.c \
				machdep_freebsd.c \
				machdep_netbsd.c

		       
module_la_LIBADD       = @e_libs@ -lm @machdep_libs@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

mem.edj: Makefile mem.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 mem.edc \
	 mem.edj

clean-local:
	 rm -rf *.edj *~

