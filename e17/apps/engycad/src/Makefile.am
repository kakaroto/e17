## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I../lib \
	@ENGY_CFLAGS@

bin_PROGRAMS = engycad

engycad_SOURCES = \
arc.c \
arc.h \
circle.c \
circle.h \
cl.c \
cl.h \
common.c \
common.h \
dim.c \
dim.h \
dimstyle.c \
dimstyle.h \
drawing.c \
drawing.h \
earc.c \
earc.h \
ellipse.c \
ellipse.h \
engy.h \
engytypes.h \
globals.h \
graserv.c \
graserv.h \
guiserv.c \
guiserv.h \
hatch.c \
hatch.h \
hatchstyle.c \
hatchstyle.h \
image.c \
image.h \
layer.c \
layer.h \
line.c \
line.h \
linestyle.c \
linestyle.h \
log.c \
log.h \
magnet.c \
magnet.h \
menu.c \
menu.h \
misc.c \
misc.h \
mouse.c \
mouse.h \
panel.c \
panel.h \
point.c \
point.h \
rectangle.c \
rectangle.h \
sel.c \
sel.h \
serv.c \
serv.h \
shell.c \
shell.h \
text.c \
text.h \
textstyle.c \
textstyle.h \
trans.c \
trans.h \
undo.c \
undo.h

engycad_LDADD = @ENGY_LIBS@ @pthread_libs@ -lm
