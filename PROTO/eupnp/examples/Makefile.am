MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src/lib @EUPNP_CFLAGS@
AM_CFLAGS = -I$(top_srcdir)/src/lib @EUPNP_CFLAGS@

if HAVE_ELM
   AM_CPPFLAGS += @ELM_CFLAGS@
   AM_CFLAGS += @ELM_CFLAGS@
endif

if HAVE_EMOTION
   AM_CPPFLAGS += @EMOTION_CFLAGS@
   AM_CFLAGS += @EMOTION_CFLAGS@
endif

bin_PROGRAMS = \
	eupnp_sniffer \
	eupnp_light_status_monitor

if EUPNP_BUILD_EXAMPLE_BROWSER
   bin_PROGRAMS += eupnp_media_server_browser
endif


eupnp_sniffer_SOURCES = upnp_sniffer.c
eupnp_sniffer_LDADD = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la
eupnp_sniffer_DEPENDENCIES = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la

eupnp_media_server_browser_SOURCES = media_server_browser.c
eupnp_media_server_browser_LDADD = @EMOTION_LIBS@ @ELM_LIBS@ $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la $(top_builddir)/src/lib/eupnp_av/libeupnp_av.la
eupnp_media_server_browser_DEPENDENCIES = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la $(top_builddir)/src/lib/eupnp_av/libeupnp_av.la

eupnp_light_status_monitor_SOURCES = light_status_monitor.c
eupnp_light_status_monitor_LDADD = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la
eupnp_light_status_monitor_DEPENDENCIES = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la
