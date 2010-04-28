ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.log config.sub configure depcomp \
		       install-sh ltmain.sh missing compile

pkgconfigdir = $(libdir)/pkgconfig

if BUILD_EHAL
EHAL_PC = ehal.pc
endif
if BUILD_ENOTIFY
ENOTIFY_PC = enotify.pc
endif
if BUILD_ECONNMAN
ECONNMAN_PC = econnman.pc
endif
if BUILD_EBLUEZ
EBLUEZ_PC = ebluez.pc
endif
if BUILD_EOFONO
EOFONO_PC = eofono.pc
endif
#if BUILD_EUDEV
#EUDEV_PC = eudev.pc
#endif
if BUILD_EUKIT
EUKIT_PC = eukit.pc
endif
pkgconfig_DATA = edbus.pc $(EHAL_PC) $(ENOTIFY_PC) $(ECONNMAN_PC) $(EBLUEZ_PC) $(EOFONO_PC) $(EUKIT_PC)
#pkgconfig_DATA = edbus.pc $(EHAL_PC) $(ENOTIFY_PC) $(ECONNMAN_PC) $(EBLUEZ_PC) $(EOFONO_PC) $(EUDEV_PC) $(EUKIT_PC)

EXTRA_DIST = e_dbus.spec
