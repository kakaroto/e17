## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = INSTALL Makefile.in aclocal.m4 config.guess \
		config.h.in config.sub configure install-sh loaderpath.h \
		ltconfig ltmain.sh missing mkinstalldirs stamp-h.in

SUBDIRS		= libltdl

# CFLAGS	= -O -g -mpentium -mcpu=pentium -march=pentium
LDFLAGS		= -L/usr/X11R6/lib
INCLUDES	= -I/usr/X11R6/include -I$(top_srcdir)/libltdl

pkgdir		= $(libdir)/loaders/image
pkg_LTLIBRARIES	= png.la jpeg.la

png_la_SOURCES	= loader_png.c
png_la_LDFLAGS	= -no-undefined -module -avoid-version
png_la_LIBADD	= -lpng -lz -lX11 -lXext

jpeg_la_SOURCES	= loader_jpeg.c
jpeg_la_LDFLAGS	= -no-undefined -module -avoid-version
jpeg_la_LIBADD	= -ljpeg

bin_PROGRAMS	= imlib2
imlib2_SOURCES	= rend.c ximage.c scale.c main.c rgba.c image.c \
		  color.c grab.c blend.c file.c rgbadraw.c api.c draw.c \
		  context.c updates.c colormod.c font.c format.c \
		  api.h  image.h scale.h blend.h context.h updates.h \
		  color.h draw.h rend.h ximage.h colormod.h file.h rgba.h \
		  common.h grab.h rgbadraw.h font.h format.h
				 
imlib2_LDADD	= @DLLDFLAGS@ $(top_builddir)/libltdl/libltdlc.la -lX11 -lXext

SYS_LOADERS_PATH = @pkglibdir@

image.o: loaderpath.h
loaderpath.h: Makefile
	@echo "#define SYS_LOADERS_PATH \"${SYS_LOADERS_PATH}\"" > $@
	@echo "#define USER_LOADERS_PATH \".loaders\"" >> $@

