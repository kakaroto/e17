MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = @ETAM_CFLAGS@ \
@EFL_ETAM_BUILD@

lib_LTLIBRARIES = libetam.la

includes_HEADERS = Etam.h
includesdir = $(includedir)/etam-@VMAJ@

libetam_la_SOURCES = \
etam_packet_long.c \
etam_packet_string.c \
etam_packet_bool.c \
etam_descriptor.c \
etam_collection.c \
etam_request.c \
etam_main.c \
etam_data.c \
etam_local.c \
etam_packets.c

libetam_la_LIBADD = @ETAM_LIBS@
libetam_la_LDFLAGS  = -no-undefined @lt_enable_auto_import@ -version-info @version_info@ @release_info@

EXTRA_DIST = etam_private.h etam_bool.h etam_packets_op.h
