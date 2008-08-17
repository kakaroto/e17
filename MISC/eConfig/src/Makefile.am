lib_LTLIBRARIES = libeConfig.la

include_HEADERS = eConfig.h

libeConfig_la_SOURCES = \
    api.c               \
    cache.c             \
    disk.c              \
    eConfig.internal.h  \
    fsck.c              \
    init.c              \
    lock.c              \
    path.c              \
    regex.c             \
    snprintf.c          \
    timestamp.c

libeConfig_la_LDFLAGS    = -version-info 0:1:0

INCLUDES = -I. -I$(srcdir) -I.. -I$(srcdir)/..
