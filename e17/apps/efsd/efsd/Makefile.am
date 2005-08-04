## Process this file with automake to produce Makefile.in

bin_PROGRAMS = efsd

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in Makefile

DEBUGFLAGS = -DDEBUG -DDEBUG_NEST -W -Wall

INCLUDES = @edb_cflags@ @xml2_cflags@ $(DEBUGFLAGS) @ecore_cflags@

if USE_THREADS_FILES
efsd_threadstuff = efsd_lock.c
endif


include_HEADERS = \
	efsd.h \
	libefsd.h

efsd_SOURCES = 					\
	efsd_commands.h efsd_commands.c 	\
	efsd_debug.h efsd_debug.c		\
	efsd_dynarray.h efsd_dynarray.c 	\
	efsd_event_queue.h efsd_event_queue.c 	\
	efsd_fam_r.c efsd_fam_r.h 		\
	efsd_filetype.h efsd_filetype.c 	\
	efsd_fs.h efsd_fs.c 			\
	efsd_globals.h 				\
	efsd_hash.h efsd_hash.c 		\
	efsd_io.h efsd_io.c 			\
	efsd_main.h efsd_main.c 		\
	efsd_macros.h 				\
	efsd_meta.c efsd_meta.h 		\
	efsd_meta_monitor.c efsd_meta_monitor.h \
	efsd_monitor.h efsd_monitor.c 		\
	efsd_queue.h efsd_queue.c 		\
	efsd_list.h efsd_list.c 		\
	efsd_lock.h 				\
	efsd_misc.h efsd_misc.c 		\
	efsd_options.h efsd_options.c 		\
	efsd_stack.h efsd_stack.c 		\
	efsd_statcache.h efsd_statcache.c 	\
	efsd_types.h efsd_types.c 		\
	$(efsd_threadstuff)

efsd_LDADD = @edb_libs@ @xml2_libs@ @ecore_libs@

lib_LTLIBRARIES = libefsd.la

libefsd_la_SOURCES   = 				\
	efsd.h 					\
	efsd_macros.h 				\
	efsd_debug.h efsd_debug.c 		\
	efsd_io.h efsd_io.c 			\
	efsd_options.h efsd_options.c 		\
	efsd_types.h efsd_types.c 		\
	efsd_queue.h libefsd_queue.c 		\
	efsd_list.h efsd_list.c 		\
	efsd_hash.h efsd_hash.c 		\
	libefsd_misc.h libefsd_misc.c 		\
	libefsd.c

libefsd_la_CFLAGS = -DLIB $(AM_CFLAGS)

EXTRA_DIST = gdb.scr

install-exec-hook:
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	test ! -z "$(GDB_CMD_FILE)" && $(INSTALL_DATA) gdb.scr $(DESTDIR)$(pkgdatadir)/ || :
