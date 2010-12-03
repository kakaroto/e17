AUTOMAKE_OPTIONS = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I. 					\
	-I$(top_srcdir)/src/include 		\
	-DPACKAGE_MODULE_DIR=\"@PACKAGE_MODULE_DIR@\" @elixir_cflags@ @EET_CFLAGS@

if ELIXIR_STATIC
noinst_LTLIBRARIES = libelixir.la
else
lib_LTLIBRARIES = libelixir.la
endif

libelixir_la_SOURCES = 	\
elixir_debug.c 		\
elixir_file.c 		\
elixir_manip.c 		\
elixir_module.c 	\
elixir_params.c 	\
elixir_run.c 		\
elixir_tracker.c 	\
elixir_void.c 		\
elixir_object.c 	\
elixir_class.c 		\
elixir_id.c 		\
elixir_template.c 	\
elixir_context.c	\
elixir_eet.c		\
$(sources)

libelixir_la_LDFLAGS = -version-info 0:1:0
libelixir_la_DEPENDENCIES = $(top_builddir)/config.h
libelixir_la_LIBADD = @elixir_libs@ @EET_LIBS@

if EINA_STATIC_BUILD_DIR
libelixir_la_SOURCES += $(top_srcdir)/src/modules/bindings/dir/dir_module.c
endif

if EINA_STATIC_BUILD_ECORE
libelixir_la_SOURCES += $(top_srcdir)/src/modules/bindings/ecore/ecore_module.c
libelixir_la_LIBADD += @ECORE_LIBS@
INCLUDES += @ECORE_CFLAGS@
endif

if EINA_STATIC_BUILD_ECORE_CON
libelixir_la_SOURCES += $(top_srcdir)/src/modules/bindings/ecore_con/ecore_con.c
libelixir_la_LIBADD += @ECORE_CON_LIBS@
INCLUDES += @ECORE_CON_CFLAGS@
endif

if EINA_STATIC_BUILD_ECORE_DOWNLOAD
libelixir_la_SOURCES += $(top_srcdir)/src/modules/bindings/ecore_download/ecore_download.c
libelixir_la_LIBADD += @ECORE_DOWNLOAD_LIBS@
INCLUDES += @ECORE_DOWNLOAD_CFLAGS@
endif

if EINA_STATIC_BUILD_ECORE_EVAS
libelixir_la_SOURCES += $(top_srcdir)/src/modules/bindings/ecore_evas/ecore_evas_module.c
libelixir_la_LIBADD += @ECORE_EVAS_LIBS@
INCLUDES += @ECORE_EVAS_CFLAGS@
endif

if EINA_STATIC_BUILD_EDJE
libelixir_la_SOURCES += $(top_srcdir)/src/modules/bindings/edje/edje_module.c
libelixir_la_SOURCES += $(top_srcdir)/src/modules/bindings/edje/edje_layout_module.c
libelixir_la_SOURCES += $(top_srcdir)/src/modules/loaders/edje/edje_loader.c
libelixir_la_LIBADD += @EDJE_LIBS@
INCLUDES += @EDJE_CFLAGS@
endif

if EINA_STATIC_BUILD_ELIXIR
libelixir_la_SOURCES += $(top_srcdir)/src/modules/bindings/elixir/main_module.c
endif

if EINA_STATIC_BUILD_EVAS
libelixir_la_SOURCES += $(top_srcdir)/src/modules/bindings/evas/evas-bindings.c $(top_srcdir)/src/modules/bindings/evas/evas-const-binding.c $(top_srcdir)/src/modules/bindings/evas/evas-events-binding.c \
	$(top_srcdir)/src/modules/bindings/evas/evas-int-params.c $(top_srcdir)/src/modules/bindings/evas/evas-object-2-params.c $(top_srcdir)/src/modules/bindings/evas/evas-object-binding.c \
	$(top_srcdir)/src/modules/bindings/evas/evas-object-boolean-params.c $(top_srcdir)/src/modules/bindings/evas/evas-object-color-params.c \
	$(top_srcdir)/src/modules/bindings/evas/evas-object-image-binding.c $(top_srcdir)/src/modules/bindings/evas/evas-object-int-2-params.c \
	$(top_srcdir)/src/modules/bindings/evas/evas-object-int-params.c $(top_srcdir)/src/modules/bindings/evas/evas-object-line-binding.c \
	$(top_srcdir)/src/modules/bindings/evas/evas-object-params.c $(top_srcdir)/src/modules/bindings/evas/evas-object-string-params.c \
	$(top_srcdir)/src/modules/bindings/evas/evas-object-textblock.c $(top_srcdir)/src/modules/bindings/evas/evas-object-text.c $(top_srcdir)/src/modules/bindings/evas/evas-params.c \
	$(top_srcdir)/src/modules/bindings/evas/evas-string-params.c $(top_srcdir)/src/modules/bindings/evas/void-params.c $(top_srcdir)/src/modules/bindings/evas/evas-smart.c \
	$(top_srcdir)/src/modules/bindings/evas/evas-object-int-3-params.c $(top_srcdir)/src/modules/bindings/evas/evas-object-double-2-params.c \
	$(top_srcdir)/src/modules/bindings/evas/evas-object-int-4-params.c $(top_srcdir)/src/modules/bindings/evas/evas-object-double-params.c

libelixir_la_LIBADD += @EVAS_LIBS@
INCLUDES += @EVAS_CFLAGS@
endif

if EINA_STATIC_BUILD_FILE
libelixir_la_SOURCES += $(top_srcdir)/src/modules/bindings/file/file_module.c
endif

if EINA_STATIC_BUILD_MIX
libelixir_la_SOURCES += $(top_srcdir)/src/modules/bindings/mix/mixer_module.c
libelixir_la_LIBADD += @sdl_libs@ -lSDL_mixer
INCLUDES += @sdl_cflags@
endif

if EINA_STATIC_BUILD_SQLITE3
libelixir_la_SOURCES += $(top_srcdir)/src/modules/bindings/sqlite/sqlite_const.c $(top_srcdir)/src/modules/bindings/sqlite/sqlite_module.c
libelixir_la_LIBADD += @SQLITE_LIBS@
INCLUDES += @SQLITE_CFLAGS@
endif

if EINA_STATIC_BUILD_EMOTION
libelixir_la_SOURCES += $(top_srcdir)/src/modules/bindings/emotion/emotion_module.c
libelixir_la_LIBADD += @EMOTION_LIBS@
INCLUDES += @EMOTION_CFLAGS@
endif

if EINA_STATIC_BUILD_TEXT
libelixir_la_SOURCES += $(top_srcdir)/src/modules/loaders/text/text_loader.c
endif

if EINA_STATIC_BUILD_EET
libelixir_la_SOURCES += $(top_srcdir)/src/modules/loaders/eet/eet_loader.c $(top_srcdir)/src/modules/bindings/eet/eet_module.c
libelixir_la_LIBADD += @EET_LIBS@
INCLUDES += @EET_CFLAGS@
endif

if EINA_STATIC_BUILD_EIO
libelixir_la_SOURCES += $(top_srcdir)/src/modules/bindings/eio/eio_module.c
libelixir_la_LIBADD += @EIO_LIBS@
INCLUDES += @EIO_CFLAGS@
endif

if EINA_STATIC_BUILD_CMP
libelixir_la_SOURCES += $(top_srcdir)/src/modules/security/cmp/cmp_security.c
endif

if EINA_STATIC_BUILD_REMOTE
libelixir_la_SOURCES += $(top_srcdir)/src/modules/security/remote/remote_security.c
endif

if EINA_STATIC_BUILD_LET
libelixir_la_SOURCES += $(top_srcdir)/src/modules/security/let/let_security.c
endif
