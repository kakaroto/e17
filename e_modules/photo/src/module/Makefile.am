INCLUDES              = -I. \
			-I$(top_srcdir) \
                        -I$(includedir) \
			@e_cflags@
pkgdir                 = $(datadir)/$(MODULE_ARCH)

pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
                         e_mod_main.h \
			 photo_item.c \
			 photo_item.h \
			 photo_config.c \
                         photo_config.h \
			 photo_config_dialog.c \
                         photo_config_dialog.h \
			 photo_config_dialog_dir.c \
                         photo_config_dialog_dir.h \
			 photo_config_dialog_item.c \
                         photo_config_dialog_item.h \
			 photo_menu.c \
                         photo_menu.h \
			 photo_picture.c \
                         photo_picture.h \
			 photo_picture_histo.c \
                         photo_picture_histo.h \
			 photo_picture_local.c \
                         photo_picture_local.h \
			 photo_picture_net.c \
                         photo_picture_net.h \
			 photo_popup_warn.c \
                         photo_popup_warn.h \
			 photo_popup_info.c \
                         photo_popup_info.h \
			 photo_utils.c \
                         photo_utils.h \
                         Photo.h \
                         Photo_includes.h
module_la_LIBADD       = @e_libs@
module_la_LDFLAGS      = -module -avoid-version -s
module_la_DEPENDENCIES = $(top_builddir)/config.h
