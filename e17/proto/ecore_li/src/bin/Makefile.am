MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -Wall -g \
-I$(top_srcdir)/src/lib \
@EDATA_CFLAGS@ \
@ECORE_CFLAGS@

bin_PROGRAMS = ecore_li_kbd

ecore_li_kbd_SOURCES = \
ecore_li_kbd.c

ecore_li_kbd_LDADD = \
$(top_builddir)/src/lib/libecore_li.la \
@EDATA_LIBS@ \
@ECORE_LIBS@

ecore_li_kbd_DEPENDENCIES = \
$(top_builddir)/src/lib/libecore_li.la
