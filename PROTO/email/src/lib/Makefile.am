lib_LTLIBRARIES = \
  libemail.la

AM_CFLAGS = \
  $(EMAIL_CFLAGS) \
 -I$(top_srcdir)


libemail_la_LIBADD = \
  $(EMAIL_LIBS)

libemail_la_LDFLAGS = -version-info @version_info@ @release_info@ -no-undefined --as-needed


libemail_la_SOURCES = \
		attachment.c \
		cdecode.c \
		cdecode.h \
		cencode.c \
		cencode.h \
		contact.c \
		email.c \
		Email.h \
		email_private.h \
		email_utils.c \
		Makefile.am \
		md5.c \
		md5.h \
		message.c \
		pop_handlers.c \
		pop_list.c \
		pop_login.c \
		pop_retr.c \
		smtp_handlers.c \
		smtp_login.c

MAINTAINERCLEANFILES = Makefile.in
