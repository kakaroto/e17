MAINTAINERCLEANFILES = Makefile.in

INCLUDES               = -I. \
			             -I$(includedir) \
                         @E_CFLAGS@ \
                         @EVRY_CFLAGS@

plugindir              = $(datadir)

plugin_pidgindir = $(plugindir)
plugin_LTLIBRARIES = evry_plug_pidgin.la 

#evry_plug_pidgin_act.la

evry_plug_pidgin_la_SOURCES =  evry_plug_pidgin.c
evry_plug_pidgin_la_LIBADD       = @EVRY_LIBS@ @EDBUS_LIBS@ @EINA_LIBS@
evry_plug_pidgin_la_LDFLAGS      = -no-undefined -module -avoid-version
evry_plug_pidgin_la_LIBTOOLFLAGS = --tag=disable-static

#evry_plug_pidgin_act_la_SOURCES =  evry_plug_pidgin_act.c
#evry_plug_pidgin_act_la_LIBADD       = @EVRY_LIBS@ @EDBUS_LIBS@ @EINA_LIBS@
#evry_plug_pidgin_act_la_LDFLAGS      = -no-undefined -module -avoid-version
#evry_plug_pidgin_act_la_LIBTOOLFLAGS = --tag=disable-static