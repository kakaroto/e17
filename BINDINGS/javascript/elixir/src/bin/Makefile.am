INCLUDES = -Wall -I$(top_srcdir)/src/include @EINA_CFLAGS@ @EET_CFLAGS@ @elixir_cflags@

bin_PROGRAMS = elixir
if BUILD_MODULE_EET
bin_PROGRAMS += elixir-compile
endif

elixir_SOURCES = elixir_main.c

elixir_LDADD = $(top_builddir)/src/lib/libelixir.la @EINA_LIBS@
elixir_LDFLAGS =
elixir_DEPENDENCIES = $(top_builddir)/src/include/Elixir.h

if BUILD_MODULE_EET
elixir_compile_SOURCES = elixir_compile_main.c

elixir_compile_LDADD = $(top_builddir)/src/lib/libelixir.la @EINA_LIBS@ @EET_LIBS@
elixir_compile_LDFLAGS =
elixir_compile_DEPENDENCIES = $(top_builddir)/src/include/Elixir.h

if ELIXIR_STATIC
elixir_compile_LDADD += $(top_builddir)/src/lib/libelixir.la

if EINA_STATIC_BUILD_ECORE
elixir_compile_LDADD += @ECORE_LIBS@
endif

if EINA_STATIC_BUILD_ECORE_CON
elixir_compile_LDADD += @ECORE_CON_LIBS@
endif

if EINA_STATIC_BUILD_ECORE_DOWNLOAD
elixir_compile_LDADD += @ECORE_DOWNLOAD_LIBS@
endif

if EINA_STATIC_BUILD_ECORE_EVAS
elixir_compile_LDADD += @ECORE_EVAS_LIBS@
endif

if EINA_STATIC_BUILD_EDJE
elixir_compile_LDADD += @EDJE_LIBS@
endif

if EINA_STATIC_BUILD_EVAS
elixir_compile_LDADD += @EVAS_LIBS@
endif

if EINA_STATIC_BUILD_MIX
elixir_compile_LDADD += @sdl_libs@ -lSDL_mixer
endif

if EINA_STATIC_BUILD_EET
elixir_compile_LDADD += @EET_LIBS@
endif

if EINA_STATIC_BUILD_SQLITE3
elixir_compile_LDADD += @SQLITE_LIBS@
endif

if EINA_STATIC_BUILD_EMOTION
elixir_compile_LDADD += @EMOTION_LIBS@
endif

if EINA_STATIC_BUILD_EIO
elixir_compile_LDADD += @EIO_LIBS@
endif

endif

endif

if ELIXIR_STATIC
elixir_LDADD += $(top_builddir)/src/lib/libelixir.la

if EINA_STATIC_BUILD_ECORE
elixir_LDADD += @ECORE_LIBS@
endif

if EINA_STATIC_BUILD_ECORE_CON
elixir_LDADD += @ECORE_CON_LIBS@
endif

if EINA_STATIC_BUILD_ECORE_DOWNLOAD
elixir_LDADD += @ECORE_DOWNLOAD_LIBS@
endif

if EINA_STATIC_BUILD_ECORE_EVAS
elixir_LDADD += @ECORE_EVAS_LIBS@
endif

if EINA_STATIC_BUILD_EDJE
elixir_LDADD += @EDJE_LIBS@
endif

if EINA_STATIC_BUILD_EVAS
elixir_LDADD += @EVAS_LIBS@
endif

if EINA_STATIC_BUILD_MIX
elixir_LDADD += @sdl_libs@ -lSDL_mixer
endif

if EINA_STATIC_BUILD_EET
elixir_LDADD += @EET_LIBS@
endif

if EINA_STATIC_BUILD_SQLITE3
elixir_LDADD += @SQLITE_LIBS@
endif

if EINA_STATIC_BUILD_EMOTION
elixir_LDADD += @EMOTION_LIBS@
endif

if EINA_STATIC_BUILD_EIO
elixir_LDADD += @EIO_LIBS@
endif

endif
