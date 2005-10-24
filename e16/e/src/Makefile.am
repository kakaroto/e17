SUBDIRS =

EXTRA_DIST = ewmh.c gnome.c ecompmgr.c ecompmgr.h snprintf.c

bin_PROGRAMS = e16

if ENABLE_EWMH
SRCS_EWMH = ewmh.c
endif
if ENABLE_GNOME
SRCS_GNOME = gnome.c
endif
if USE_ESNPRINTF
SRCS_SNPRINTF = snprintf.c
endif
if HAS_COMPOSITE
SRCS_ECOMPMGR = ecompmgr.c ecompmgr.h
endif
e16_SOURCES = \
	E.h			\
	aclass.c		\
	actions.c		\
	alert.c			\
	areas.c                 \
	arrange.c		\
	backgrounds.c		\
	backgrounds.h		\
	borders.c		\
	buttons.c		\
	buttons.h		\
	cmclass.c		\
	comms.c			\
	conf.h			\
	config.c		\
	coords.c                \
	cursors.c		\
	desktops.c		\
	desktops.h		\
	dialog.c	        \
	dock.c			\
	draw.c			\
	econfig.c		\
	econfig.h		\
	ecore-e16.c		\
	ecore-e16.h		\
	edge.c			\
	emodule.c		\
	emodule.h		\
	eobj.c			\
	eobj.h			\
	events.c		\
	ewins.c			\
	ewins.h			\
	ewin-ops.c		\
	ewin-ops.h		\
	extinitwin.c		\
	file.c		        \
	finders.c		\
	focus.c 		\
	fonts.c 		\
	fx.c                    \
	grabs.c			\
	groups.c		\
	handlers.c		\
	hints.c			\
	hiwin.c			\
	hiwin.h			\
	icccm.c			\
	iclass.c		\
	iclass.h		\
	iconify.c		\
	icons.c			\
	icons.h			\
	ipc.c			\
	lang.c			\
	lists.c			\
	main.c			\
	memory.c		\
	menus.c			\
	menus.h			\
	menus-misc.c		\
	misc.c			\
	mod-misc.c		\
	mod-trans.c		\
	moveresize.c		\
	mwm.c			\
	pager.c                 \
	progress.c		\
	regex.c			\
	screen.c                \
	session.c               \
	settings.c              \
	setup.c			\
	size.c			\
	slideout.c		\
	snaps.c                 \
	snaps.h			\
	sound.c			\
	stacking.c		\
	startup.c		\
	tclass.c		\
	tclass.h		\
	text.c			\
	theme.c                 \
	timers.c		\
	timestamp.h		\
	tooltips.c		\
	tooltips.h		\
	ttfont.c		\
	warp.c			\
	windowmatch.c   	\
	x.c                     \
	xwin.h			\
	zoom.c			\
	$(SRCS_EWMH) $(SRCS_GNOME) \
	$(SRCS_ECOMPMGR)	\
	$(SRCS_SNPRINTF)


LDADD = \
	@LIBINTL@		\
	@ecore_libs@		\
	$(ESD_LIBS) 		\
	$(IMLIB_LIBS) 		\
	$(X_PRE_LIBS) 		\
	$(X_LIBS) 		\
	$(X_EXTRA_LIBS) 	\
	$(XINERAMA_LIBS)	\
	$(E_X_LIBS) 		\
	-lX11 -lm

INCLUDES = -I$(top_builddir) -I$(top_srcdir)/intl @ecore_cflags@ $(ESD_CFLAGS) $(IMLIB_CFLAGS) $(X_CFLAGS)

install-data-local:
	$(top_srcdir)/mkinstalldirs $(DESTDIR)$(ENLIGHTENMENT_ROOT)/themes

uninstall-local:
	for i in $(bin_PROGRAMS); do \
	  rm -f $(exec_prefix)/bin/$$i; \
	done
