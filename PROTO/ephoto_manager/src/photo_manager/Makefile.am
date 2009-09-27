
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
   -Wall -g @EPHOTO_MANAGER_CFLAGS@

lib_LTLIBRARIES      = libephoto_manager.la
EPHOTOMANAGERHEADERS      = EPhoto_Manager.h
libephoto_manager_la_SOURCES  = \
photo_manager.c \
Photo_Manager.h \
photo_manager_private.h \
root.c \
album.c \
photo.c \
sync.c \
load.c \
thumb.c \
file_manager.c

installed_headersdir = $(prefix)/include/ephoto_manager
installed_headers_DATA = $(EPHOTOMANAGERHEADERS)

libephoto_manager_la_LIBADD  = @EPHOTO_MANAGER_LIBS@

libephoto_manager_la_DEPENDENCIES =

libephoto_manager_la_LDFLAGS = -version-info 1:0:0

