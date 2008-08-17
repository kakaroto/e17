## Process this file with automake to produce Makefile.in

.o.so:  
	$(CC) $(INCLUDES) $(LDFLAGS) $< -o $@

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
	@EVAS_CFLAGS@ \
	@ECORE_CFLAGS@ 

CLEANFILES = \
	libengrave_la-engrave.h \
	libengrave_la-engrave.c \
	engrave.c

lib_LTLIBRARIES = libengrave.la 

ENGRAVEHEADERS = \
	Engrave.h \
	engrave_data.h \
	engrave_enums.h \
	engrave_file.h \
	engrave_font.h \
	engrave_spectrum.h \
	engrave_color_class.h \
	engrave_style.h \
	engrave_group.h \
	engrave_image.h \
	engrave_load.h \
	engrave_macros.h \
	engrave_out.h \
	engrave_parse.h \
	engrave_part.h \
	engrave_part_state.h \
	engrave_program.h \
	engrave_canvas.h

ENGRAVEFILES = \
	engrave.y \
	engrave.l \
	engrave_out.c \
	engrave_data.c \
	engrave_file.c \
	engrave_font.c \
	engrave_spectrum.c \
	engrave_color_class.c \
	engrave_style.c \
	engrave_group.c \
	engrave_image.c \
	engrave_parse.c \
	engrave_part.c \
	engrave_part_state.c \
	engrave_program.c \
	engrave_load.c \
	engrave_canvas.c \
	engrave_private.h \
	$(ENGRAVEHEADERS)

libengrave_la_SOURCES = $(ENGRAVEFILES)

installed_headersdir = $(prefix)/include/engrave
installed_headers_DATA = $(ENGRAVEHEADERS)

libengrave_la_YFLAGS		= -d
libengrave_la_LIBADD		= @EVAS_LIBS@ @ECORE_LIBS@ @LEXLIB@ -lm
libengrave_la_DEPENDENCIES	= $(top_builddir)/src/config.h
libengrave_la_LDFLAGS		= -version-info @version_info@
libengrave_la_CFLAGS		= -Wall

