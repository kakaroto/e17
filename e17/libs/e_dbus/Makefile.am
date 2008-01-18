SUBDIRS = src

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.log config.sub configure depcomp \
		       install-sh ltmain.sh missing compile

pkgconfigdir = $(libdir)/pkgconfig

if BUILD_EHAL
EHAL_PC = ehal.pc
endif
if BUILD_ENM
ENM_PC = enm.pc
endif
if BUILD_ENOTIFY
ENOTIFY_PC = enotify.pc
endif
pkgconfig_DATA = edbus.pc $(EHAL_PC) $(ENM_PC) $(ENOTIFY_PC)
