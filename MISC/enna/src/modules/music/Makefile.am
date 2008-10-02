MAINTAINERCLEANFILES = Makefile.in

MODULE = music

# data files for the module
filesdir = $(libdir)/enna/modules/

# the module .so file
INCLUDES = \
-I. \
-I$(top_srcdir) \
-I$(top_srcdir)/src/modules/$(MODULE) \
-I$(top_srcdir)/src/bin \
-I$(top_srcdir)/src/modules \
@ENNA_CFLAGS@

pkgdir = $(libdir)/enna/modules

pkg_LTLIBRARIES = enna_module_music.la

enna_module_music_la_SOURCES= \
enna_module_music.c \
enna_module_music.h \
smart_player.c\
smart_player.h

enna_module_music_la_LIBADD = @ENNA_LIBS@
enna_module_music_la_LDFLAGS = -module -avoid-version
enna_module_music_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/modules/enna_module_$(MODULE).*
