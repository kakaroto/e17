AM_CFLAGS = \
  -I@top_srcdir@ \
  @MYSQL_CFLAGS@ \
  $(CC_PIC)


noinst_LTLIBRARIES = libmysac.la
libmysac_la_LIBADD = @MYSQL_LIBS@
libmysac_la_SOURCES = \
mysac.c \
mysac_decode_field.c \
mysac_decode_row.c \
mysac_encode_values.c \
mysac_errors.c \
mysac_net.c

MAINTAINERCLEANFILES = Makefile.in
