bin_PROGRAMS = ee2

ee2_SOURCES = 		\
	ee2.h		\
	ee2.c

CFLAGS := $(CPPFLAGS) $(CFLAGS) `gtk-config --cflags` 

ee2_LDFLAGS = `gtk-config --libs`

pkgdata_DATA = 		\
	ee2.png		\
	ee2-alpha.png

SPLASH_IMAGE = ee2.png

ee2.h:	ee2.h.in
	sed 's/@SPLASH_IMAGE_FILE@/'`echo $(pkgdatadir) | sed 's/\//\\\\\//g'`'\//' ee2.h.in > ee2.h 
