INCLUDES              = -I. \
			-I$(top_srcdir) \
                        -I$(includedir) \
			@ENLIGHTENMENT_CFLAGS@
pkgdir                 = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
			 e_mod_main.h \
			 e_mod_devian.c \
			 e_mod_devian.h \
			 e_mod_config.c \
			 e_mod_config.h \
			 e_mod_config_dialog.c \
			 e_mod_config_dialog.h \
			 e_mod_config_dialog_devian.c \
			 e_mod_config_dialog_devian.h \
			 e_mod_config_dialog_rss.c \
			 e_mod_config_dialog_rss.h \
			 e_mod_config_dialog_theme.c \
			 e_mod_config_dialog_theme.h \
			 e_mod_menu.c \
			 e_mod_menu.h \
			 e_mod_popup_warn.c \
			 e_mod_popup_warn.h \
			 e_mod_container.c \
			 e_mod_container.h \
			 e_mod_container_box.c \
			 e_mod_container_box.h \
			 e_mod_source.c \
			 e_mod_source.h \
			 e_mod_source_picture.c \
			 e_mod_source_picture.h \
			 e_mod_source_rss.c \
			 e_mod_source_rss.h \
			 e_mod_source_file.c \
			 e_mod_source_file.h \
			 e_mod_data_picture.c \
			 e_mod_data_picture.h \
			 e_mod_data_rss.c \
			 e_mod_data_rss.h \
			 e_mod_data_rss_parse.c \
			 e_mod_data_rss_parse.h \
			 e_mod_data_file.c \
			 e_mod_data_file.h \
                         dEvian.h dEvian_includes.h
module_la_LIBADD       = @ENLIGHTENMENT_LIBS@
module_la_LDFLAGS      = -module -avoid-version -s
module_la_DEPENDENCIES = $(top_builddir)/config.h
