## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS     = 1.7 foreign

LDFLAGS              = -rdynamic -shared
INCLUDES             = -I. -I$(top_srcdir) -I$(top_srcdir)/src/include

pkgdir =  $(libdir)/evfs/plugins/meta

if HAVE_TAGLIB
AUDIO_TAGGER_L = audio_tagger.la
else
AUDIO_TAGGER_L = 
endif

if HAVE_EXTRACTOR
EXTRACTOR_TAGGER_L = extractor_tagger.la
else
EXTRACTOR_TAGGER_L = 
endif

pkg_LTLIBRARIES         = $(AUDIO_TAGGER_L) $(EXTRACTOR_TAGGER_L)

audio_tagger_la_SOURCES = evfs_meta_audio.c \
			$(top_srcdir)/src/common/evfs_debug.c \
			$(top_srcdir)/src/common/evfs_event_helper.c \
			$(top_srcdir)/src/common/evfs_misc.c

extractor_tagger_la_SOURCES = evfs_meta_extractor.c \
                        $(top_srcdir)/src/common/evfs_debug.c \
                        $(top_srcdir)/src/common/evfs_event_helper.c \
                        $(top_srcdir)/src/common/evfs_misc.c

			
audio_tagger_la_CFLAGS =  @ecore_cflags@ @taglib_cflags@
audio_tagger_la_LDFLAGS = -module -avoid-version
audio_tagger_la_LIBADD =  @ecore_libs@ @taglib_libs@ -L$(top_builddir)/src/lib -levfs

extractor_tagger_la_CFLAGS =  @ecore_cflags@ @extractor_cflags@
extractor_tagger_la_LDFLAGS = -module -avoid-version
extractor_tagger_la_LIBADD =  @ecore_libs@ @extractor_libs@ -L$(top_builddir)/src/lib -levfs


