MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = @ELICIT_CFLAGS@ -Wall -Werror -DDATADIR=\"$(datadir)\"

bin_PROGRAMS = elicit

elicit_SOURCES = \
main.c \
binreloc.c binreloc.h \
elicit.c elicit.h \
config.c config.h \
color.c color.h \
palette.c palette.h \
palette_view.c palette_view.h \
grab.c grab.h \
shot.c shot.h \
band.c band.h \
cslider.c cslider.h \
pan.c pan.h \
scrollframe.c scrollframe.h \
types.h

elicit_LDADD = @ELICIT_LIBS@

