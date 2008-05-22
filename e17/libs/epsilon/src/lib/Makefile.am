
MAINTAINERCLEANFILES = Makefile.in

SUBDIRS              = exiftags

AM_CPPFLAGS = \
-I$(top_srcdir)/src/include \
-DPACKAGE_BIN_DIR=\""$(bindir)"\" \
-DPACKAGE_LIB_DIR=\""$(libdir)"\" \
@EVAS_CFLAGS@ \
@ECORE_CFLAGS@ \
@EDJE_CFLAGS@ \
@PNG_CFLAGS@

lib_LTLIBRARIES      = libepsilon.la
include_HEADERS      = Epsilon.h Epsilon_Plugin.h
libepsilon_la_SOURCES = \
Epsilon.c \
Epsilon_Exif.c \
md5.c md5.h \
epsilon_thumb.c \
$(top_srcdir)/src/common/epsilon_thumb_common.c

libepsilon_la_LIBADD	= exiftags/libepsilon_exiftags.la @PNG_LIBS@ @EDJE_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@
libepsilon_la_DEPENDENCIES = ../config.h
libepsilon_la_LDFLAGS      = -version-info @version_info@
