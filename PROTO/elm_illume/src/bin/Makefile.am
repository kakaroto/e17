MAINTAINERCLEANFILES = Makefile.in
INCLUDES = -I$(top_srcdir) \
         -I$(top_srcdir)/src/bin \
         @ELM_ILLUME_CFLAGS@

bin_PROGRAMS = elm_illume

ELMILLUMEHEADERS = \
              elm_illume.h \
              elm_ill_container.h

elm_illume_SOURCES = \
            $(ELMILLUMEHEADERS) \
            main.c \
            elm_ill_container.c

elm_illume_LDFLAGS = -export-dynamic
elm_illume_LDADD = @ELM_ILLUME_LIBS@
