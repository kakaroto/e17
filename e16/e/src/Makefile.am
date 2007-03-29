SUBDIRS =

EXTRA_DIST = gnome.c eglx.c eglx.h ecompmgr.c ecompmgr.h snprintf.c

bin_PROGRAMS = e16

if ENABLE_GNOME
SRCS_GNOME = gnome.c
endif
if HAVE_GLX
SRCS_GLX = eglx.c eglx.h
endif
if HAVE_COMPOSITE
SRCS_ECOMPMGR = ecompmgr.c ecompmgr.h
endif
if USE_ESNPRINTF
SRCS_SNPRINTF = snprintf.c
endif

e16_SOURCES = \
	E.h			\
	about.c			\
	aclass.c		aclass.h		\
	actions.c		\
	alert.c			alert.h			\
	arrange.c		\
	backgrounds.c		backgrounds.h		\
	borders.c		borders.h		\
	buttons.c		buttons.h		\
	cmclass.c		\
	comms.c			\
	conf.h			\
	config.c		\
	container.c		container.h		\
	coords.c                \
	cursors.c		cursors.h		\
	desktops.c		desktops.h		\
	dialog.c	        dialog.h	        \
	dock.c			\
	draw.c			\
	e16-ecore_hints.c	e16-ecore_hints.h	\
	e16-ecore_list.c	e16-ecore_list.h	\
	econfig.c		econfig.h		\
	edebug.h		\
	edge.c			\
	eimage.c		eimage.h		\
	emodule.c		emodule.h		\
	eobj.c			eobj.h			\
	etypes.h		\
	events.c		events.h		\
	ewins.c			ewins.h			\
	ewin-ops.c		ewin-ops.h		\
	ewmh.c			\
	extinitwin.c		\
	file.c		        file.h		        \
	finders.c		\
	focus.c 		focus.h			\
	fonts.c 		\
	fx.c                    \
	grabs.c			grabs.h			\
	groups.c		groups.h		\
	handlers.c		\
	hints.c			hints.h			\
	hiwin.c			hiwin.h			\
	icccm.c			\
	iclass.c		iclass.h		\
	iconify.c		\
	icons.c			icons.h			\
	ipc.c			\
	lang.c			lang.h			\
	main.c			\
	memory.c		\
	menus.c			menus.h			\
	menus-misc.c		\
	misc.c			\
	mod-misc.c		\
	mod-trans.c		\
	moveresize.c		\
	mwm.c			\
	pager.c                 \
	parse.c                 parse.h                 \
	progress.c		progress.h		\
	regex.c			\
	screen.c                screen.h                \
	session.c               session.h               \
	settings.c              settings.h              \
	setup.c			\
	size.c			\
	slideout.c		\
	snaps.c                 snaps.h			\
	sound.c			\
	stacking.c		\
	startup.c		\
	systray.c		\
	tclass.c		tclass.h		\
	text.c			\
	theme.c                 \
	timers.c		timers.h		\
	tooltips.c		tooltips.h		\
	user.c			user.h			\
	util.h			\
	ttfont.c		\
	warp.c			\
	windowmatch.c   	windowmatch.h		\
	x.c                     xwin.h			\
	zoom.c			\
	$(SRCS_GNOME) 		\
	$(SRCS_GLX) 		\
	$(SRCS_ECOMPMGR)	\
	$(SRCS_SNPRINTF)


LDADD = \
	$(LTLIBINTL)		\
	$(LTLIBICONV)		\
	$(ECORE_LIBS)		\
	$(ESD_LIBS) 		\
	$(IMLIB2_LIBS) 		\
	$(GLX_LIBS) 		\
	$(XFT_LIBS) 		\
	$(X_PRE_LIBS) 		\
	$(X_LIBS) 		\
	$(XINERAMA_LIBS) 	\
	$(X_EXTRA_LIBS) 	\
	$(E_X_LIBS) 		\
	-lX11 -lm

INCLUDES = -I$(top_builddir) -I$(top_srcdir) $(ECORE_CFLAGS) $(ESD_CFLAGS) $(IMLIB2_CFLAGS) $(XFT_CFLAGS) $(X_CFLAGS)

install-data-local:
	$(top_srcdir)/mkinstalldirs $(DESTDIR)$(ENLIGHTENMENT_ROOT)/themes

uninstall-local:
	for i in $(bin_PROGRAMS); do \
	  rm -f $(exec_prefix)/bin/$$i; \
	done
