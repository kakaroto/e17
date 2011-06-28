MAINTAINERCLEANFILES = *.o config.h.in config.h Makefile.in Makefile stamp-h1

INCLUDES = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/src/bin \
	-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
	-DGETTEXT_PACKAGE=\"$(PACKAGE)\" \
	-DLOCALEDIR=\"$(localedir)\" \
	@ELEMENTARY_CFLAGS@ @EFREET_MIME_CFLAGS@ @EIO_CFLAGS@ @EXIF_CFLAGS@ @EET_CFLAGS@

bin_PROGRAMS = ephoto
if BUILD_QUICKLAUNCH
bin_PROGRAMS += ephoto_ql
endif

_sources = \
	ephoto.c \
	ephoto_main.c \
	ephoto_config.c \
        ephoto_directory_thumb.c \
        ephoto_thumb_browser.c \
        ephoto_single_browser.c \
        ephoto_slideshow.c


_libs =	@ELEMENTARY_LIBS@ @EFREET_MIME_LIBS@ @EIO_LIBS@ @EXIF_LIBS@ @EET_LIBS@

ephoto_SOURCES = $(_sources)
ephoto_LDADD = $(_libs)

noinst_HEADERS = gettext.h ephoto.h
EXTRA_DIST = gettext.h ephoto.h

if BUILD_QUICKLAUNCH
############################################################################
## Build quick launch binary, needs elementary_quicklaunch to be enabled. ##
##                                                                        ##
## It is composed of a library with actual code and a binary that talks   ##
## to server that will then fork() + dlopen() such library.               ##
############################################################################
ephoto_qldir = $(quicklauncher_libdir)
ephoto_ql_LTLIBRARIES = ephoto_ql.la
ephoto_ql_la_SOURCES = $(_sources)
ephoto_ql_la_LIBADD = $(_libs)
ephoto_ql_la_CFLAGS =
ephoto_ql_la_LDFLAGS = -module -avoid-version -no-undefined
ephoto_ql_SOURCES = ephoto.c
ephoto_ql_LDADD = @ELEMENTARY_LIBS@
ephoto_ql_CFLAGS = -DELM_LIB_QUICKLAUNCH=1
ephoto_ql_LDFLAGS =
endif
