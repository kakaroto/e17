## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in
SUBDIRS              = exiftags

INCLUDES	     = @IMLIB2_CFLAGS@ @EPEG_CFLAGS@ @PNG_CFLAGS@ @ECORE_CFLAGS@ @EVAS_CFLAGS@ @EDJE_CFLAGS@ \
			-I$(top_srcdir)/src/include \
			-DPACKAGE_BIN_DIR=\""$(bindir)"\" \
			-DPACKAGE_LIB_DIR=\""$(libdir)"\"

lib_LTLIBRARIES      = libepsilon.la
include_HEADERS      = Epsilon.h 
libepsilon_la_SOURCES = \
Epsilon.c \
Epsilon_Exif.c \
md5.c md5.h \
epsilon_thumb.c \
$(top_srcdir)/src/common/epsilon_thumb_common.c

libepsilon_la_LIBADD	= exiftags/libepsilon_exiftags.la @IMLIB2_LIBS@ @EPEG_LIBS@ @PNG_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ @EDJE_LIBS@ -lemotion -lxine
libepsilon_la_DEPENDENCIES = ../config.h 
libepsilon_la_LDFLAGS      = $(LDFLAGS) -version-info 0:1:0
