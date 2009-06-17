MAINTAINERCLEANFILES = Makefile.in

pkgdir = @libdir@

if HAVE_EMOTION


pkg_LTLIBRARIES = libetk_video.la

INCLUDES = -I$(top_srcdir)/src/lib/etk_video \
			  @ETK_CFLAGS@ \
			  @EMOTION_CFLAGS@

installed_headersdir = $(prefix)/include/etk_extra
installed_headers_DATA = etk_video.h

libetk_video_la_SOURCES = etk_video.c \
			  etk_video.h

libetk_video_la_LDFLAGS = -version-info 0:9:0
libetk_video_la_LIBADD = @ETK_LIBS@ @EMOTION_LIBS@

endif
