lib_LTLIBRARIES = \
  libshotgun.la

AM_CFLAGS = \
  $(ELM_CFLAGS) \
 -I@top_builddir@ \
 -I@top_srcdir@/src/include
AM_CXXFLAGS = $(AM_CFLAGS)

libshotgun_la_LIBADD = \
  @ELM_LIBS@ \
  -lm

libshotgun_la_LDFLAGS = -version-info @version_info@ @release_info@ -no-undefined

libshotgun_la_SOURCES = \
cdecode.c \
cdecode.h \
cencode.c \
cencode.h \
iq.c \
login.c \
md5.c \
md5.h \
messaging.c \
presence.c \
pugiconfig.hpp \
pugixml.cpp \
pugixml.hpp \
sha1.c \
sha1.h \
sha1wrap.c \
shotgun.c \
shotgun_private.h \
shotgun_utils.c \
xml.cpp \
xml.h

MAINTAINERCLEANFILES = Makefile.in
