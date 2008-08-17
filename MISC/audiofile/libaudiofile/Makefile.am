## Process this file with automake to produce Makefile.in

INCLUDES = -I.

lib_LTLIBRARIES = libaudiofile.la

libaudiofile_la_SOURCES = \
	audiofile.c error.c write.c bread.c bwrite.c byteorder.c \
	util.c aupv.c \
	aes.c instrument.c loop.c marker.c misc.c track.c query.c compression.c \
	aiff.c aiffwrite.c extended.c aiff.h \
	au.c auwrite.c au.h \
	wave.c wavewrite.c wave.h \
	ulaw.c g711.c g711.h \
	afinternal.h aupvinternal.h byteorder.h extended.h compression.h error.h \
	block.h util.h \
	af_vfs.c

include_HEADERS = audiofile.h aupvlist.h af_vfs.h

CFLAGS = @CFLAGS@ -g
#CFLAGS = @CFLAGS@ -Wall -DNDEBUG
