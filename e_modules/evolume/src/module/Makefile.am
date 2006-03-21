MAINTAINERCLEANFILES=Makefile.in

# the module .so file
INCLUDES               = -I. \
						 -I../lib \
		                 -I$(top_srcdir) \
           				 -I$(includedir) \
			              @e_cflags@

e_modules = @E_MODULES@

filesdir = $(datadir)
pkgdir   = $(e_modules)/$(PACKAGE)/$(MODULE_ARCH)
 
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = \
						 e_mod_cdialog.c \
						 e_mod_cdialog.h \
						 e_mod_cmdialog.c \
						 e_mod_cmdialog.h \
						 e_mod_config.c \
						 e_mod_config.h \
						 e_mod_face.c \
						 e_mod_face.h \
						 e_mod_main.c \
						 e_mod_main.h \
						 e_mod_menu.c \
						 e_mod_menu.h \
						 e_mod_mixer.c \
						 e_mod_mixer.h \
						 e_mod_pack.c \
						 e_mod_pack.h \
						 e_mod_slider.c \
						 e_mod_slider.h \
						 e_mod_volume.c \
						 e_mod_volume.h \
						 e_mod_util.c \
						 e_mod_util.h 

						 

module_la_LIBADD       = @e_libs@ -lm -L../lib/ -lemixer
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h $(top_builddir)/src/lib/libemixer.a
