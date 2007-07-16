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
	entropy_config.c \
	entropy_gui.c \
	entropy_alert.c \
	entropy_gui_event_handlers.c 

entropy_CC	= cc
entropy_CFLAGS  = @EWL_CFLAGS@ @EVFS_CFLAGS@ \
		  @IMLIB2_CFLAGS@ @EPSILON_CFLAGS@ @ECORE_CFLAGS@
entropy_LDFLAGS	= -rdynamic @IMLIB2_CFLAGS@
entropy_LDADD	= $(INTLLIBS) \
		@EWL_LIBS@ \
		@IMLIB2_LIBS@ \
		@EVFS_LIBS@ @EPSILON_LIBS@ @ECORE_LIBS@
