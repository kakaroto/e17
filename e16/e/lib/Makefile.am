MAINTAINERCLEANFILES	= Makefile.in

if BUILD_ROOTHACKLIB

lib_LTLIBRARIES		= libe16_hack.la

libe16_hack_la_SOURCES  = e16_hack.c

libe16_hack_la_LIBADD   = $(DLOPEN_LIBS)
#libe16_hack_la_LDFLAGS  = -version-info 0:0:0
libe16_hack_la_LDFLAGS  = -avoid-version

endif
