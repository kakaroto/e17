SUBDIRS = themes

EXTRA_DIST = README ChangeLog TODO ewmh.c gnome.c kde.c

if FSSTD
bindir=@bindir@
ENLIGHTENMENT_BIN=@bindir@
ENLIGHTENMENT_ROOT=$(pkgdatadir)
else
ENLIGHTENMENT_BIN=$(prefix)/enlightenment/bin
ENLIGHTENMENT_ROOT=$(prefix)/enlightenment
bindir=$(ENLIGHTENMENT_BIN)
endif


bin_PROGRAMS = enlightenment

if ENABLE_EWMH
SRCS_EWMH = ewmh.c
endif
if ENABLE_GNOME
SRCS_GNOME = gnome.c            
endif
if ENABLE_KDE
SRCS_KDE = kde.c
endif
enlightenment_SOURCES = \
	E.h			\
	timestamp.h             \
	update.h                \
	actions.c		\
	alert.c			\
	areas.c                 \
	arrange.c		\
	arrange.h		\
	atoms.c			\
	borders.c		\
	buttons.c		\
	clone.c                 \
	cmclass.c		\
	comms.c			\
	conf.h			\
	config.c		\
	containers.c		\
	coords.c                \
	cursors.c		\
	desktops.c		\
	dialog.c	        \
	dock.c			\
	draw.c			\
	events.c		\
	evhandlers.c		\
	file.c		        \
	finders.c		\
	focus.c 		\
	fx.c                    \
	globals.c		\
	grabs.c			\
	groups.c		\
	handlers.c		\
	hints.c			\
	icccm.c			\
	iclass.c		\
	iconify.c		\
	init.c			\
	ipc.c			\
	lists.c			\
	main.c			\
	memory.c		\
	menus.c			\
	misc.c			\
	modules.c		\
	mwm.c			\
	network.c		\
	pager.c                 \
	progress.c		\
	regex.c			\
	scursor.c               \
	session.c               \
	settings.c              \
	setup.c			\
	size.c			\
	slideout.c		\
	snaps.c                 \
	snprintf.c		\
	sound.c			\
	startup.c		\
	sticky.c		\
	stripjoints.c   	\
	strips.c		\
	tclass.c		\
	text.c			\
	theme.c                 \
	timers.c		\
	tooltips.c		\
	ttfont.c		\
	update.c		\
	warp.c			\
	windowmatch.c   	\
	x.c                     \
	zoom.c			\
	$(SRCS_EWMH) $(SRCS_GNOME) $(SRCS_KDE)


LDADD = \
	@INTLLIBS@			\
	$(FNLIB_LIBS) 		\
	$(TTF_LIBS) 		\
	$(ESD_LIBS) 		\
	$(IMLIB_LIBS) 		\
	$(XTST_LIBS) 		\
	$(XVM_LIBS) 		\
	$(GHTTP_LIBS)           \
	$(DL_LIBS)              \
	$(XINERAMA_LIBS)              \
	-lm

INCLUDES=-I$(top_srcdir) $(ESD_CFLAGS) $(IMLIB_CFLAGS) -I$(includedir) -I.. -I../intl -I$(top_srcdir)/intl -I/usr/include/freetype1

DEFS=-DENLIGHTENMENT_ROOT=\"@ENLIGHTENMENT_ROOT@\" -DENLIGHTENMENT_BIN=\"@ENLIGHTENMENT_BIN@\" -DLOCALEDIR=\"@LOCALEDIR@\"

#install-exec-local:
#	if [ x@USE_FSSTD@ = "xno" ]; then \
#	  ../mkinstalldirs $(exec_prefix)/bin; \
#	  for i in $(bin_PROGRAMS); do \
#	    rm -f $(exec_prefix)/bin/$$i; \
#	    $(LN_S) $(bindir)/$$i $(exec_prefix)/bin/$$i; \
#	  done; \
#	fi

install-data-local:
	$(top_srcdir)/mkinstalldirs $(DESTDIR)$(ENLIGHTENMENT_ROOT)/themes; \
	rm -f $(DESTDIR)$(ENLIGHTENMENT_ROOT)/themes/DEFAULT; \
        $(LN_S) \
	BrushedMetal-Tigert \
	$(DESTDIR)$(ENLIGHTENMENT_ROOT)/themes/DEFAULT;

uninstall-local:
	for i in $(bin_PROGRAMS); do \
	  rm -f $(exec_prefix)/bin/$$i; \
	done
