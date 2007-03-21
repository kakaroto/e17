INCLUDES =  \
@ECORE_CFLAGS@ \
@DBUS_CFLAGS@ \
-I ../ \
-g -Werror

lib_LTLIBRARIES = libenm.la

include_HEADERS = E_Nm.h

libenm_la_SOURCES = \
E_Nm.h \
e_nm_private.h \
e_nm.c \
e_nm_manager.c \
e_nm_device.c \
e_nm_util.c 


libenm_la_LIBADD = @ECORE_LIBS@ @DBUS_LIBS@ -lm -ledbus
