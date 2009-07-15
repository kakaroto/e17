
lib_LTLIBRARIES = \
	libesmartxx_container.la

libesmartxx_container_la_SOURCES = \
	EvasSmartContainer.cpp

libesmartxx_container_la_LIBADD = \
	$(EFL_LIBS)                \
	$(ESMART_CONTAINER_LIBS)

EXTRA_DIST = \
	$(esmartxx_container_HEADERS)\
	$(libesmartxx_container_HEADERS)

libesmartxx_container_HEADERS = \
	EvasSmartContainer.h

libesmartxx_containerdir = \
	$(pkgincludedir)

AM_CPPFLAGS = \
	$(EFL_CFLAGS)\
	$(ESMART_CONTAINER_CFLAGS)

## File created by the gnome-build tools

