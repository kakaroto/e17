######################################################################
# Makefile for eplay
# User Options
######################################################################

# Directory where qiv will be installed under.

PREFIX = /usr/local

# Should image be centered on screen by default? 1=yes, 0=no.

CENTER = 1

# /usr/X11R6/include/X11/cursorfont.h for more choices.

CURSOR = 84

# The first line of this list should reflect the libraries which are
# compiled into imlib on your system. If your system's C library does
# not declare random/srandom or getopt_long, remove RANDOM and/or
# GETOPT_LONG in the second line.

HAVE_LIBS = GIF TIFF XPM XBM PNG PPM PNM PGM PCX BMP EIM JPEG TGA \
            RANDOM GETOPT_LONG

# The following only apply to 'make install-xscreensaver':
# Delay in minutes to start xscreensaver

SS_TIMEOUT = 5

# Delay in minutes to cycle between xscreensaver programs

SS_CYCLE = 5

# Delay in seconds to wait between images

SS_DELAY = 5

# Image files to display

SS_IMAGES = ~/pictures/*.jpg

# Comment out this line to have pictures be dislayed sequentially

SS_RANDOMIZE = -r

######################################################################
# Variables and Rules
# Do not edit below here!
######################################################################

CC        = gcc
CFLAGS    = -O2 -Wall -s -fomit-frame-pointer -finline-functions \
	    -fcaller-saves -ffast-math -fno-strength-reduce \
	    -fthread-jumps

INCLUDES  = `imlib-config --cflags-gdk`
LIBS      = `imlib-config --libs-gdk`

PROGRAM   = eplay
OBJS      = image.o filter.o options.o utils.o main.o
HEADERS   = eplay.h
DEFINES   = $(patsubst %,-DHAVE_%, $(HAVE_LIBS)) \
            -DCENTER=$(CENTER) \
	    -DSTATUSBAR_FONT=$(STATUSBAR_FONT) \
	    -DCURSOR=$(CURSOR) \
	    -DWM_HANDLE=$(WM_HANDLE)

ifeq (,$(findstring GETOPT_LONG,$(HAVE_LIBS)))
OBJS     += getopt.o getopt1.o
endif

PROGRAM_G = qiv-g
OBJS_G    = $(OBJS:.o=.g)
DEFINES_G = $(DEFINES) -DDEBUG

SS_PROG   = $(PREFIX)/ss-qiv

######################################################################

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) $(DEFINES) $(LIBS) $(OBJS) -o $(PROGRAM)

$(OBJS): %.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(DEFINES) $(INCLUDES) $< -o $@

######################################################################

debug: $(PROGRAM_G)

$(PROGRAM_G): $(OBJS_G)
	$(CC) -g $(CFLAGS) $(DEFINES_G) $(LIBS) $(OBJS_G) -o $(PROGRAM_G)

$(OBJS_G): %.g: %.c $(HEADERS)
	$(CC) -c -g $(CFLAGS) $(DEFINES_G) $(INCLUDES) $< -o $@

######################################################################

clean :  
	rm -f $(PROGRAM) $(PROGRAM_G) $(OBJS) $(OBJS_G) getopt.o getopt1.o

install: $(PROGRAM)
	install -s -m 0755 $(PROGRAM) $(PREFIX)/bin
	install -m 0644 $(PROGRAM).1 $(PREFIX)/man/man1
	gzip $(PREFIX)/man/man1/$(PROGRAM).1
	@if ./qiv -o white -f ./intro.jpg ; \
	then echo "-- Test Passed --" ; \
	else echo "-- Test Failed --" ; \
	fi

install-xscreensaver: install
	@echo "#!/bin/sh" > $(SS_PROG)
	@echo "xhost +`hostname` 1> /dev/null" >> $(SS_PROG)
	@echo "xrdb < ~/.qivrc" >> $(SS_PROG)
	@echo "xscreensaver 1> /dev/null &" >>  $(SS_PROG)
	@echo "xscreensaver.timeout: $(SS_TIMEOUT)" > ~/.qivrc
	@echo "xscreensaver.cycle: $(SS_CYCLE)" >> ~/.qivrc
	@echo "xscreensaver.programs: qiv -iftsd $(SS_DELAY) $(SS_RANDOMIZE) $(SS_IMAGES)" >> ~/.qivrc
	echo "Start screensaver with ss-qiv"


