AM_CFLAGS = \
  -I@top_srcdir@ \
  @MYSQL_CFLAGS@ \
  $(CC_PIC)


noinst_LTLIBRARIES = libmysac.la
libmysac_la_LIBADD = @MYSQL_LIBS@
libmysac_la_SOURCES = \
mysac.c \
mysac_decode_field.c \
mysac_decode_field.h \
mysac_decode_row.c \
mysac_decode_row.h \
mysac_encode_values.c \
mysac_encode_values.h \
mysac_errors.c \
mysac.h \
mysac_net.c \
mysac_net.h \
mysac_utils.h

MAINTAINERCLEANFILES = Makefile.in
