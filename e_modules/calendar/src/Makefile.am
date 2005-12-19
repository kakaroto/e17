MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = images

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v

# data files for the module
filesdir = $(datadir)
files_DATA = \
NoImage_icon.png \
module_icon.png 

EXTRA_DIST = $(files_DATA)

# the module .so file
INCLUDES               = -I. \
			 -I$(top_srcdir) \
			 -I$(includedir) \
			 @e_cflags@
pkgdir                 = $(datadir)/$(MODULE_ARCH)

pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c e_mod_main.h  \
			 main_editor.h main_editor.c \
			 calendar_color_func.h calendar_color_func.c calendar_text_fun.c \
			 calendar_text_func.h dialog_func.c dialog_func.h add_event_dialog.h\
			 add_event_dialog.c add_todo_dialog.h add_todo_dialog.c calendar_func.h\
			 calendar_func.c cal_face_func.h cal_face_func.c

module_la_LIBADD       = @e_libs@ -lm
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h


clean-local:
	 rm -rf *.edj *~
