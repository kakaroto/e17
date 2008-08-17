AUTOMAKE_OPTIONS       = 1.7 foreign

MAINTAINERCLEANFILES = Makefile.in

if BUILD_FRONTEND_ETK
etk_fe_dir = etk_fe
etk_fe_a = $(etk_fe_dir)/libetk_fe.a
endif

SUBDIRS = $(etk_fe_dir)

INCLUDES             = -I. -I$(top_srcdir) -I$(top_srcdir)/src/include

bin_PROGRAMS = extrackt

extrackt_INCLUDES = \
Extrackt.h \
cddb.h \
cddev.h \
command.h \
main.h \
protocol.h \
http.h \
util.h \
conf.h

extrackt_SOURCES = \
main.c \
cddev.c \
cddb.c \
command.c \
protocol.c \
http.c \
util.c \
conf.c \
$(extrackt_INCLUDES)
	
extrackt_CFLAGS  = @my_includes@
extrackt_LDFLAGS = -export-dynamic
extrackt_LDADD	= $(INTLLIBS) \
		$(etk_fe_a) \
		@ETK_LIBS@ \
		@my_libs@
		
