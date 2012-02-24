MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = common
EXOTIC_LIBADD =
EXOTIC_DEPS =

if EXOTIC_USE_COYOTE
SUBDIRS += coyote
EXOTIC_LIBADD +=
EXOTIC_DEPS += coyote/libexotic_coyote.la
endif

if EXOTIC_USE_PS3
SUBDIRS += ps3
EXOTIC_LIBADD +=
EXOTIC_DEPS += ps3/libexotic_ps3.la
endif
# if EXOTIC_USE_MINGW32CE
# SUBDIRS += windows
# endif
# if EXOTIC_USE_MINGW
# SUBDIRS += windows
# endif
# if EXOTIC_USE_DARWIN
# SUBDIRS += darwin
# endif

lib_LTLIBRARIES = libexotic.la

libexotic_la_SOURCES =
libexotic_la_LIBADD = $(EXOTIC_LIBADD) $(EXOTIC_DEPS) common/libexotic_common.la
libexotic_la_DEPENDENCIES = $(EXOTIC_DEPS) common/libexotic_common.la

DIST_SUBDIRS = common coyote # ps3 windows darwin
