## Process this file with automake to produce Makefile.in

SUBDIRS = ttfonts

INCLUDES = \
-I. -I$(top_srcdir) \
@GTK_CFLAGS@ -DPREFIX=\""$(prefix)"\" @GIBLIB_CFLAGS@

bin_PROGRAMS = stalk

stalk_SOURCES = \
	main.c \
	utils.c utils.h \
    debug.h \
    stalk.h \
    structs.h \
    options.c options.h \
    stalk_interface.c stalk_interface.h \
    stalk_file.c stalk_file.h \
    stalk_line.c stalk_line.h \
    stalk_window.c stalk_window.h \
    imlib.c

stalk_LDADD = @GTK_LIBS@ @GIBLIB_LIBS@
