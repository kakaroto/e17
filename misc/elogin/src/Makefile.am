bin_PROGRAMS = elogin

elogin_SOURCES = mem.c str.c db.c x.c im_engine.c image.c\
				events.c main.c events.h im_engine.h db.h\
				image.h main.h  mem.h  str.h  util.h  x.h
IMAGES =

ELOGIN_BIN=$(prefix)/elogin/bin
ELOGIN_ROOT=$(prefix)/elogin
bindir=$(ELOGIN_BIN)

SUBDIRS =

EXTRA_DIST =

LDADD = \
    $(TTF_LIBS)         \
    $(X_LIBS)       \
    $(DL_LIBS)              \
    $(XINERAMA_LIBS)

INCLUDES=-I$(top_srcdir) $(IMLIB2_CFLAGS) -I$(includedir)






