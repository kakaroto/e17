SUBDIRS =

EXTRA_DIST = gnome.c ecompmgr.c ecompmgr.h snprintf.c

bin_PROGRAMS = e16

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
	aclass.h		\
	actions.c		\
	alert.c			\
	alert.h			\
	arrange.c		\
	backgrounds.c		\
	backgrounds.h		\
	borders.c		\
	borders.h		\
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
	dialog.h	        \
	dock.c			\
	draw.c			\
	e16-ecore_hints.c	\
	e16-ecore_hints.h	\
	e16-ecore_list.c	\
	e16-ecore_list.h	\
	econfig.c		\
	econfig.h		\
	edge.c			\
	eimage.c		\
	eimage.h		\
	emodule.c		\
	emodule.h		\
	eobj.c			\
	eobj.h			\
	etypes.h		\
	events.c		\
	ewins.c			\
	ewins.h			\
	ewin-ops.c		\
	ewin-ops.h		\
	ewmh.c			\
	extinitwin.c		\
	file.c		        \
	file.h		        \
	finders.c		\
	focus.c 		\
	fonts.c 		\
	fx.c                    \
	grabs.c			\
	groups.c		\
	groups.h		\
	handlers.c		\
	hints.c			\
	hints.h			\
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
	lang.h			\
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
	parse.c                 \
	parse.h                 \
	progress.c		\
	regex.c			\
	screen.c                \
	screen.h                \
	session.c               \
	session.h               \
	settings.c              \
	settings.h              \
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
	timers.h		\
	tooltips.c		\
	tooltips.h		\
	user.c			\
	user.h			\
	util.h			\
	ttfont.c		\
	warp.c			\
	windowmatch.c   	\
	x.c                     \
	xwin.h			\
	zoom.c			\
	$(SRCS_GNOME) 		\
	$(SRCS_ECOMPMGR)	\
	$(SRCS_SNPRINTF)


LDADD = \
	$(LTLIBINTL)		\
	$(LTLIBICONV)		\
	@ecore_libs@		\
	$(ESD_LIBS) 		\
	$(IMLIB_LIBS) 		\
	$(X_PRE_LIBS) 		\
	$(X_LIBS) 		\
	$(X_EXTRA_LIBS) 	\
	$(E_X_LIBS) 		\
	-lX11 -lm

INCLUDES = -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/intl @ecore_cflags@ $(ESD_CFLAGS) $(IMLIB_CFLAGS) $(X_CFLAGS)

install-data-local:
	$(top_srcdir)/mkinstalldirs $(DESTDIR)$(ENLIGHTENMENT_ROOT)/themes

uninstall-local:
	for i in $(bin_PROGRAMS); do \
	  rm -f $(exec_prefix)/bin/$$i; \
	done
