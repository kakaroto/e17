## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in loaderpath.h

LDFLAGS		     = -L/usr/X11R6/lib
INCLUDES	     = -I/usr/X11R6/include -I$(top_srcdir)/libltdl \
                       $(X_CFLAGS) -I$(prefix)/include -I$(includedir) \
		       -I. -I$(top_srcdir) -I$(top_srcdir)/src \
		       -I$(top_srcdir)/loaders

lib_LTLIBRARIES      = libImlib2.la
include_HEADERS      = Imlib2.h
libImlib2_la_SOURCES = rend.c ximage.c scale.c rgba.c image.c color.c grab.c \
                       blend.c file.c rgbadraw.c api.c draw.c context.c \
		       updates.c colormod.c font.c format.c grad.c rotate.c \
                       Imlib2.h image.h scale.h blend.h context.h updates.h \
		       color.h draw.h rend.h ximage.h colormod.h file.h \
		       rgba.h common.h grab.h rgbadraw.h font.h format.h \
		       rotate.h grad.h filter.h \
		       asm_blend.S filter.c asm_rgba.S asm_scale.S asm_rotate.S
libImlib2_la_LIBADD   = @DLLDFLAGS@ $(top_builddir)/libltdl/libltdlc.la \
                       -lX11 -lXext -lttf $(LDFLAGS)
libImlib2_la_DEPENDENCIES = $(top_builddir)/config.h
libImlib2_la_LDFLAGS      = -version-info 0:3:0

SYS_LOADERS_PATH = @pkglibdir@
image.lo: loaderpath.h
loaderpath.h: Makefile
	@echo "#define SYS_LOADERS_PATH \"${SYS_LOADERS_PATH}\"" > $@
	@echo "#define USER_LOADERS_PATH \".loaders\"" >> $@
		
