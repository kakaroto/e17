AUTOMAKE_OPTIONS       = 1.7 foreign

SUBDIRS = plugins

INCLUDES             = -I. -I$(top_srcdir) -I$(top_srcdir)/src/include

bin_PROGRAMS = entropy

entropy_SOURCES = \
	entropy_mime_engine.c \
	entropy_thumbnailer_engine.c \
	plugin_helper.c \
	notification_engine.c \
	entropy_core.c \
	main.c \
	plugins/md5.c \
	plugins/thumbnail_generic.c \
	entropy_file.c \
	entropy_debug.c \
	entropy_config.c


entropy_CC	= @PTHREAD_CC@
entropy_CFLAGS  = @PTHREAD_CFLAGS@ \
		  @EWL_CFLAGS@ @EVFS_CFLAGS@ \
		  @IMLIB2_CFLAGS@
entropy_LDFLAGS	= -rdynamic @PTHREAD_CFLAGS@ @IMLIB2_CFLAGS@
entropy_LDADD	= $(INTLLIBS) \
		@PTHREAD_LIBS@ \
		@EWL_LIBS@ \
		@IMLIB2_LIBS@ \
		@EVFS_LIBS@
