MAINTAINERCLEANFILES = Makefile.in

INCLUDES               = -I.. \
			 -I. \
                         @EVRY_CFLAGS@

plugindir              = $(datadir)

evry_plugdir = $(plugindir)
evry_plug_LTLIBRARIES = evry_plug.la
evry_plug_la_SOURCES =  evry_plug.c md5.c md5.h
evry_plug_la_LIBADD       = @EVRY_LIBS@
evry_plug_la_LDFLAGS      = -no-undefined -module -avoid-version
evry_plug_la_LIBTOOLFLAGS = --tag=disable-static
