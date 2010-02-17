MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src/lib @EUPNP_CFLAGS@ @ELM_CFLAGS@
AM_CFLAGS = -I$(top_srcdir)/src/lib @EUPNP_CFLAGS@ @ELM_CFLAGS@

noinst_PROGRAMS = \
	upnp_sniffer \
	light_status_monitor

if USE_MODULE_EUPNP_AV
   noinst_PROGRAMS += media_server_browser
endif


upnp_sniffer_SOURCES = upnp_sniffer.c
upnp_sniffer_LDADD = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la
upnp_sniffer_DEPENDENCIES = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la

if USE_MODULE_EUPNP_AV
media_server_browser_SOURCES = media_server_browser.c
media_server_browser_LDADD = @ELM_LIBS@ $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la $(top_builddir)/src/lib/eupnp_av/libeupnp_av.la
media_server_browser_DEPENDENCIES = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la $(top_builddir)/src/lib/eupnp_av/libeupnp_av.la
endif

light_status_monitor_SOURCES = light_status_monitor.c
light_status_monitor_LDADD = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la
light_status_monitor_DEPENDENCIES = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la
