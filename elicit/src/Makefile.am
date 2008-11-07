## Process this file with automake to produce Makefile.in

INCLUDES = @ELICIT_CFLAGS@

bin_PROGRAMS = elicit 

elicit_SOURCES = \
        elicit.c Elicit.h \
        callbacks.c callbacks.h \
        util.c util.h \
        conf.c conf.h \
        ruler.c ruler.h \
        swatches.c swatches.h \
        shots.c shots.h \
        themes.c themes.h \
	spectrum.c spectrum.h \
	zoom.c zoom.h

elicit_LDADD = @ELICIT_LIBS@

