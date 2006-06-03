MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = images

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v

# data files for the module
filesdir = $(datadir)
files_DATA = \
NoImage_icon.png \
module_icon.png \
../module.eap

EXTRA_DIST = $(files_DATA)

# the module .so file
INCLUDES               = -I. \
			 -I$(top_srcdir) \
			 -I$(includedir) \
			 @e_cflags@
pkgdir                 = $(datadir)/$(MODULE_ARCH)

pkg_LTLIBRARIES        = module.la

module_la_SOURCES      = add_event_dialog.c add_event_dialog.h add_todo_dialog.c add_todo_dialog.h \
                         cal_face_func.c cal_face_func.h calendar_color_func.c calendar_color_func.h \
                         calendar_func.c calendar_func.h calendar_text_fun.c calendar_text_fun.h \
                         dialog_func.c dialog_func.h e_mod_main.c e_mod_main.h main_editor.c \
                         main_editor.h otherfonts_editor.c otherfonts_editor.h today_editor.c \
                         today_editor.h weekday_editor.c weekday_editor.h weekend_editor.c \
                         weekend_editor.h 

module_la_LIBADD       = @e_libs@ -lm
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h


clean-local:
	 rm -rf *.edj *~
