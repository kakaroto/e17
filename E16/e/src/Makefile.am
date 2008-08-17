SUBDIRS =

EXTRA_DIST = gnome.c eglx.c eglx.h glwin.c ecompmgr.c ecompmgr.h magwin.c snprintf.c

bin_PROGRAMS = e16

if ENABLE_GNOME
SRCS_GNOME = gnome.c
endif
if ENABLE_GLX
SRCS_GLX = eglx.c eglx.h glwin.c
endif
if ENABLE_COMPOSITE
SRCS_ECOMPMGR = ecompmgr.c ecompmgr.h magwin.c
endif
if ENABLE_DBUS
SRCS_DBUS = edbus.c edbus.h
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
	comms.c			comms.h \
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
	ipc.c			ipc.h \
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
	piximg.c                piximg.h                \
	progress.c		progress.h		\
	regex.c			\
	screen.c                screen.h                \
	session.c               session.h               \
	settings.c              settings.h              \
	setup.c			\
	size.c			\
	slideout.c		\
	snaps.c                 snaps.h			\
	sound.c			sound.h			\
	stacking.c		\
	startup.c		\
	string.c		\
	systray.c		\
	tclass.c		tclass.h		\
	text.c			\
	theme.c                 \
	timers.c		timers.h		\
	tooltips.c		tooltips.h		\
	user.c			user.h			\
	util.h			\
	warp.c			\
	windowmatch.c   	windowmatch.h		\
	x.c                     xwin.h			\
	xtypes.h                \
	zoom.c			\
	$(MODULE_SRCS)		\
	$(SRCS_GNOME) 		\
	$(SRCS_GLX) 		\
	$(SRCS_ECOMPMGR)	\
	$(SRCS_DBUS)		\
	$(SRCS_SNPRINTF)

e16_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir) \
	$(ECORE_CFLAGS) \
	$(MODULE_CFLAGS) \
	$(IMLIB2_CFLAGS) \
	$(X_CFLAGS) \
	$(DBUS_CFLAGS) \
	-D LOCALEDIR=\"$(datadir)/locale\" \
	-D ENLIGHTENMENT_BIN=\"$(bindir)\" \
	-D ENLIGHTENMENT_LIB=\"$(pkglibdir)\" \
	-D ENLIGHTENMENT_ROOT=\"$(pkgdatadir)\"

LDADD = \
	$(LTLIBINTL)		\
	$(LTLIBICONV)		\
	$(ECORE_LIBS)		\
	$(IMLIB2_LIBS)		\
	$(MODULE_LIBS)		\
	$(GLX_LIBS)		\
	$(X_LIBS)		\
	$(SM_LIBS)		\
	$(XINERAMA_LIBS)	\
	$(X_EXTRA_LIBS)		\
	$(E_X_LIBS)		\
	$(DBUS_LIBS)		\
	-lX11 -lm

if BUILD_MODULES

e16_LDFLAGS = -export-dynamic

libe16dir = $(pkglibdir)

libe16_LTLIBRARIES = $(LIBSND_ESD) $(LIBSND_PA) $(LIBFNT_IFT) $(LIBFNT_XFT) $(LIBFNT_PANGO)

if USE_LIBESD
LIBSND_ESD = libsound_esd.la
libsound_esd_la_SOURCES   = sound_esd.c sound_load.c
libsound_esd_la_CFLAGS    = $(ESD_CFLAGS)
libsound_esd_la_LIBADD    = $(ESD_LIBS)
libsound_esd_la_LDFLAGS   = -module -avoid-version
endif

if USE_LIBPA
LIBSND_PA = libsound_pa.la
libsound_pa_la_SOURCES   = sound_pa.c sound_load.c
libsound_pa_la_CFLAGS    = $(PA_CFLAGS)
libsound_pa_la_LIBADD    = $(PA_LIBS)
libsound_pa_la_LDFLAGS   = -module -avoid-version
endif

LIBFNT_IFT = libfont_ift.la
libfont_ift_la_SOURCES    = ttfont.c
libfont_ift_la_CFLAGS     = $(IMLIB2_CFLAGS)
libfont_ift_la_LIBADD     = $(IMLIB2_LIBS)
libfont_ift_la_LDFLAGS    = -module -avoid-version

if USE_LIBXFT
LIBFNT_XFT = libfont_xft.la
libfont_xft_la_SOURCES    = text_xft.c
libfont_xft_la_CFLAGS     = $(XFT_CFLAGS)
libfont_xft_la_LIBADD     = $(XFT_LIBS)
libfont_xft_la_LDFLAGS    = -module -avoid-version
endif

if USE_LIBPANGO
LIBFNT_PANGO = libfont_pango.la
libfont_pango_la_SOURCES  = text_pango.c
libfont_pango_la_CFLAGS   = $(PANGO_CFLAGS)
libfont_pango_la_LIBADD   = $(PANGO_LIBS) libfont_xft.la
libfont_pango_la_LDFLAGS  = -module -avoid-version
endif

else

MODULE_SRCS = sound_esd.c sound_pa.c sound_load.c ttfont.c text_xft.c text_pango.c
MODULE_LIBS = $(ESD_LIBS) $(PA_LIBS) $(PANGO_LIBS) $(XFT_LIBS)
MODULE_CFLAGS = $(ESD_CFLAGS) $(PA_CFLAGS) $(PANGO_CFLAGS) $(XFT_CFLAGS)

endif

