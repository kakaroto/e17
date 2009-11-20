MAINTAINERCLEANFILES = Makefile.in
INCLUDES = -I$(top_srcdir) \
         -I$(top_srcdir)/src/bin \
         @ELM_KBD_CFLAGS@

bin_PROGRAMS = elm_kbd

ELMKBDHEADERS = \
              elm_kbd.h

elm_kbd_SOURCES = \
            $(EXPANDHEADERS) \
            main.c

elm_kbd_LDFLAGS = -export-dynamic
elm_kbd_LDADD = @ELM_KBD_LIBS@
