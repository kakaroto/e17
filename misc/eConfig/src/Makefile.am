lib_LTLIBRARIES = libeConfig.la

include_HEADERS = eConfig.h

libeConfig_la_SOURCES = \
    init.c              \
	load.c      	    \
    eConfig.internal.h  \
	path.c

libeConfig_la_LDFLAGS    = -version-info 0:1:0

INCLUDES = -I. -I$(srcdir) -I.. -I$(srcdir)/..
