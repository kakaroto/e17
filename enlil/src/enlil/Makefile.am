
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/enlil \
-I$(top_builddir)/src/enlil \
-I$(top_srcdir)/src/enlil/exif \
-I$(top_builddir)/src/enlil/exif \
-I$(top_srcdir)/src/enlil/exif/exif \
-I$(top_builddir)/src/enlil/exif/exif \
@ENLIL_CFLAGS@ \
@EFL_ENLIL_BUILD@

lib_LTLIBRARIES = libenlil.la

PWG_SOURCES = \
pwg_categories.azy_client.c \
pwg_categories.azy_client.h \
pwg_categories.azy_server.c \
pwg_categories.azy_server.h \
pwg_Common_Azy.c \
pwg_Common_Azy.h \
pwg_Common.c \
pwg_Common.h \
pwg_images.azy_client.c \
pwg_images.azy_client.h \
pwg_images.azy_server.c \
pwg_images.azy_server.h \
pwg_session.azy_client.c \
pwg_session.azy_client.h \
pwg_session.azy_server.c \
pwg_session.azy_server.h

if HAVE_AZY
BUILT_SOURCES = $(PWG_SOURCES)
CLEANFILES = $(PWG_SOURCES)
endif

libenlil_la_SOURCES  = \
enlil.c \
Enlil.h \
enlil_private.h \
library.c \
collection.c \
tag.c \
album.c \
photo.c \
sync.c \
load.c \
thumb.c \
file_manager.c \
transformations.c \
iptc.c \
exif.c \
exif/jpeg-data.c \
exif/jpeg-marker.c \
exif/exif-i18n.c \
geocaching.c \
net_sync.c \
download.c

if HAVE_AZY
libenlil_la_SOURCES += $(PWG_SOURCES)
endif

$(PWG_SOURCES): ws_piwigo/services.azy
	@echo  "  GEN    $@"
	@azy_parser -H -p -o ./ ws_piwigo/services.azy

installed_headersdir = $(includedir)/enlil
installed_headers_DATA = Enlil.h
libenlil_la_CFLAGS =  @EFL_PTHREAD_CFLAGS@
libenlil_la_LIBADD  = @ENLIL_LIBS@
libenlil_la_LDFLAGS = -no-undefined -version-info @version_info@ @EFL_PTHREAD_LIBS@

EXTRA_DIST = exif/

if HAVE_AZY
EXTRA_DIST += ws_piwigo/services.azy
endif
