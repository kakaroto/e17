lib_LTLIBRARIES = libeConfig.la

include_HEADERS = eConfig.h

libeConfig_la_SOURCES = \
	main.c		\
	load.c

libeConfig_la_LDFLAGS    = -version-info 0:1:0
libeConfig_la_LIBADD     = $(SUPPORT_LIBS)

INCLUDES = -I. -I$(srcdir) -I.. -I$(srcdir)/..
