MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
@EKEKO_CFLAGS@

lib_LTLIBRARIES = libekeko.la

libekeko_la_SOURCES = \
ekeko_value.c \
ekeko_main.c \
ekeko_input.c \
ekeko_object.c \
ekeko_property.c \
ekeko_event.c \
ekeko_type.c \
ekeko_renderable.c \
ekeko_canvas.c

libekeko_la_LIBADD = \
@EKEKO_LIBS@

libekeko_la_DEPENDENCIES =

libekeko_la_LDFLAGS = @create_shared_lib@ -version-info @version_info@
