MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src/lib @EINA_CFLAGS@ @ECORE_CFLAGS@
AM_CFLAGS = -I$(top_srcdir)/src/lib @EINA_CFLAGS@ @ECORE_CFLAGS@

noinst_PROGRAMS = \
	upnp_sniffer \
	media_server_browser \
	light_status_monitor


upnp_sniffer_SOURCES = upnp_sniffer.c
upnp_sniffer_LDADD = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la
upnp_sniffer_DEPENDENCIES = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la

media_server_browser_SOURCES = media_server_browser.c
media_server_browser_LDADD = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la
media_server_browser_DEPENDENCIES = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la

light_status_monitor_SOURCES = light_status_monitor.c
light_status_monitor_LDADD = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la
light_status_monitor_DEPENDENCIES = $(top_builddir)/src/lib/libeupnp.la $(top_builddir)/src/lib/eupnp_ecore/libeupnp_ecore.la
