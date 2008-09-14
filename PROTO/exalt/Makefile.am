## Process this file with automake to produce Makefile.in

MYAPPNAME=exalt-daemon
SUBDIRS = src data

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.h.in~ config.sub configure \
							  install-sh \
                       ltconfig ltmain.sh missing mkinstalldirs depcomp \
                       stamp-h stamp-h.in \
                       $(MYAPPNAME).c


EXTRA_DIST = README \
AUTHORS \
COPYING \
COPYING-PLAIN


pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = exalt_dbus.pc exalt.pc
ACLOCAL_AMFLAGS = -I m4

