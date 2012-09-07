AM_CFLAGS = \
  @SHOTGUN_CFLAGS@ \
  @AZY_CFLAGS@ \
  @GUI_CFLAGS@ \
  @edbus_CFLAGS@ \
  @enotify_CFLAGS@ \
 -DDATA_DIR=\"$(datadir)\" \
 -DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
 -DPACKAGE_LIB_DIR=\"$(libdir)\" \
 -DPACKAGE_SRC_DIR=\"$(top_srcdir)\" \
 -I@top_builddir@ \
 -I@top_srcdir@/src/include

AM_CXXFLAGS = $(AM_CFLAGS)

bin_PROGRAMS = shotgun

shotgun_LDADD = \
  @SHOTGUN_LIBS@ \
  @AZY_LIBS@ \
  @GUI_LIBS@ \
  @edbus_LIBS@ \
  @enotify_LIBS@ \
  -lm \
  @top_builddir@/src/lib/libshotgun.la

shotgun_SOURCES = \
azy.c \
chat.c \
chat_image.c \
contact.c \
contact_list.c \
dbus.c \
eet.c \
events.c \
getpass_x.c \
login.c \
logging.c \
main.c \
settings.c \
ui.c \
ui.h \
util.c

MAINTAINERCLEANFILES = Makefile.in
