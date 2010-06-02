MAINTAINERCLEANFILES = Makefile.in

include_HEADERS = Explicit.h	\
	Explicit_Context.h	\
	Explicit_Object.h

EXTRA_DIST = 			\
	explicit_private.h	\
	explicit_client.h	\
	explicit_server.h

lib_LTLIBRARIES = libexplicit.la libexplicit_server.la

libexplicit_la_CFLAGS = @EXPLICIT_CFLAGS@ @EVAS_CFLAGS@ @EDJE_CFLAGS@
libexplicit_la_LIBADD = @EXPLICIT_LIBS@ @EVAS_LIBS@ @EDJE_LIBS@
libexplicit_la_SOURCES =		\
	explicit_callback.c		\
	explicit_client.c		\
	explicit_object.c		\
	explicit_data_connection.c
libexplicit_la_LDFLAGS = -version-info @version_info@

libexplicit_server_la_CFLAGS = @EXPLICIT_CFLAGS@
libexplicit_server_la_LIBADD = @EXPLICIT_LIBS@
libexplicit_server_la_SOURCES =		\
	explicit_data_connection.c	\
	explicit_data.c
libexplicit_server_la_LDFLAGS = -version-info @version_info@


