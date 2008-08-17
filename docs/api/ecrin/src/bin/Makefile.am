
CLEANFILES = Makefile.in

bin_PROGRAMS = ecrin

ecrin_SOURCES = \
define.c \
define.h \
description.c \
description.h \
ecrin.c \
ecrin.h \
enum.c \
enum.h \
function.c \
function.h \
gui_ewl.c \
gui_ewl.h \
hash.c \
hash.h \
parser.c \
parser.h \
util.c \
util.h

ecrin_CPPFLAGS = @ECORE_CFLAGS@ @EWL_CFLAGS@ @LIBXML_CFLAGS@
ecrin_CFLAGS = 
ecrin_LDADD = @ECORE_LIBS@ @EWL_LIBS@ @LIBXML_LIBS@
