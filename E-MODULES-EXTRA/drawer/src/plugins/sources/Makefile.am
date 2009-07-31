MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I. \
	 -I$(top_srcdir) \
	 -I$(includedir) \
	 @E_CFLAGS@
if BUILD_XPIXMAP
INCLUDES += @XPIXMAP_CFLAGS@
endif

pkgdir = $(datadir)/$(MODULE_ARCH)/plugins/sources
pkg_LTLIBRARIES = launcher.la directory_watcher.la history.la winlist.la

launcher_la_SOURCES = launcher.h launcher.c
launcher_ladir = $(pkgdir)
launcher_la_DATA = launcher.desktop

launcher_la_LIBADD = @E_LIBS@
launcher_la_LDFLAGS = -module -avoid-version
launcher_la_DEPENDENCIES = $(top_builddir)/config.h

directory_watcher_la_SOURCES = directory_watcher.h directory_watcher.c
directory_watcher_ladir = $(pkgdir)
directory_watcher_la_DATA = directory_watcher.desktop

directory_watcher_la_LIBADD = @E_LIBS@
directory_watcher_la_LDFLAGS = -module -avoid-version
directory_watcher_la_DEPENDENCIES = $(top_builddir)/config.h

history_la_SOURCES = history.h history.c
history_ladir = $(pkgdir)
history_la_DATA = history.desktop

history_la_LIBADD = @E_LIBS@
history_la_LDFLAGS = -module -avoid-version
history_la_DEPENDENCIES = $(top_builddir)/config.h

winlist_la_SOURCES = winlist.h winlist.c
winlist_ladir = $(pkgdir)
winlist_la_DATA = winlist.desktop

winlist_la_LIBADD = @E_LIBS@
winlist_la_LDFLAGS = -module -avoid-version
winlist_la_DEPENDENCIES = $(top_builddir)/config.h

if BUILD_XPIXMAP
winlist_la_LIBADD += @XPIXMAP_LIBS@
endif

EXTRA_DIST = $(launcher_la_DATA) $(directory_watcher_la_DATA) $(history_la_DATA) $(winlist_la_DATA)

clean-local:
	rm -rf *~
