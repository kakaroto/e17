MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = dbus

if BUILD_EBLUEZ
SUBDIRS += bluez
endif

if BUILD_ECONNMAN
SUBDIRS += connman
endif

if BUILD_EHAL
SUBDIRS += hal
endif

if BUILD_ENOTIFY
SUBDIRS += notification
endif

if BUILD_EOFONO
SUBDIRS += ofono
endif

if BUILD_EUKIT
SUBDIRS += ukit
endif
