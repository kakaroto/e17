/*

Copyright (C) 2000, 2001 Christian Kreibich <cK@whoop.org>.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#ifndef libefsd_h
#define libefsd_h

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <efsd.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Efsd connection: */
typedef struct efsd_connection EfsdConnection;
typedef struct efsd_options EfsdOptions;

/**
 * efsd_open - Creates and returns an efsd connection. 
 * 
 * Returns pointer to a newly allocated and initialized
 * Efsd connection object. You need this object for all
 * other calls in order to identify the connection to
 * libefsd.
 */
EfsdConnection *efsd_open(void);


/**
 * efsd_close - Closes a connection to Efsd.
 * @ec: The Efsd connection
 *
 * Use this to close an efsd connection.
 * Frees the allocated EfsdConnection object.
 * Returns value < 0 if the the final
 * command could not be sent to Efsd.
 */
int            efsd_close(EfsdConnection *ec);


/**
 * efsd_get_connection_fd - Returns file descriptor of an Efsd connection.
 * @ec: The Efsd connection
 *
 * If you need to know the file descriptor of the connection
 * (such as when select()ing it etc), use this accessor function. 
 */
int            efsd_get_connection_fd(EfsdConnection *ec);


/**
 * efsd_events_pending - predicate that tells whether more events have arrived.
 * @ec: The Efsd connection
 *
 * When there are Efsd events waiting to be read, returns value
 * other than zero, and zero when there are none.
 */
int            efsd_events_pending(EfsdConnection *ec);


/**
 * efsd_next_event - reads and returns next Efsd event, if possible.
 * @ec: The Efsd connection
 * @ev: Pointer to an allocated EfsdEvent.
 *
 * If available, reads an event that the Efsd daemon sent. It does
 * not block. You want to use this if you select() Efsd's file
 * descriptor. Returns -1 when called on closed connection or no
 * data was available, >= 0 otherwise.
*/
int            efsd_next_event(EfsdConnection *ec, EfsdEvent *ev);


/** 
 * efsd_wait_event - blocking wait for next Efsd event.
 * @ec: The Efsd connection
 * @ev: Pointer to an allocated EfsdEvent.
 *
 * Blocks until an efsd event arrives, then returns it.
 * Returns -1 when called on closed connection, >= 0
 * otherwise.
 */
int            efsd_wait_event(EfsdConnection *ec, EfsdEvent *ev);


/**
 * efsd_event_cleanup - cleans up internals of an Efsd event.
 * @ev: Pointer to an allocated EfsdEvent.
 *
 * Events may contain allocated data, which gets cleaned up here.
 * Call this one before any other calls to efsd_next_event
 * on the same EfsdEvent struct.
 * NOTE -- this does not free the EfsdEvent itself -- only
 * data that was read into it. If you want to entirely get rid of
 * a dynamically allocated EfsdEvent, call efsd_cleanup_event()
 * first and then simply free() the pointer.
 */
void           efsd_event_cleanup(EfsdEvent *ev);


/* Various commands to operate on the fs.
 *
 *  UNLESS there's a communication problem (e.g. clogged
 *  buffers), each command returns and efsd command ID, which
 *  is also contained in the efsd events returned by the server
 *  so that commands and the generated replies can be associated.
 *  In case of an error, a value < 0 is returned.
 *
 *  Filenames are internally converted to fully canonical
 *  path names based on the current working directory, if not
 *  fully specified.
 */

/**
 * efsd_symlink - creates a symbolic link between files.
 * @ec: The Efsd connection
 * @from_file: Existing source file of the link
 * @to_file: Link destination
 *
 * Returns command id when successful, a value smaller than zero
 * if he command couldn't be sent.
 */
EfsdCmdId      efsd_symlink(EfsdConnection *ec, char *from_file, char *to_file);

/*  Many of the calls below can be passed options along with the command.
 *  Each option is an EfsdOption pointer, see the efsd_op calls below,
 *  which are plugged together into an EfsdOptions object.
 *  You can assemble those pointers in one of two ways:
 *
 *  1. Using a convenience wrapper, which gets the number
 *     of options as first value:
 *
 *  efsd_listdir(ec, "/home/foo/",
 *               efsd_ops(2, efsd_op_get_stat(), efsd_op_get_filetype()));
 *
 *  2. Doing things manually:
 *
 *  EfsdOptions *ops = efsd_ops_create(2);
 *  efsd_ops_add(ops, efsd_op_get_stat());
 *  efsd_ops_add(ops, efsd_op_get_filetype());
 *  efsd_listdir(ec, "/home/foo", ops);
 *
 *  You do NOT need to free the EfsdOptions pointer. It is cleaned up
 *  by the time the function returns. 
 */

/**
 * efsd_listdir - lists the contents of a directory (or also a single file).
 * @ec: The Efsd connection
 * @dirname: Directory path. If not an absolute path (starting with '/'), it
 * is interpreted as being relative to the current working directory.
 * @ops: Efsd options object.
 *
 * When issuing this command, your client will receive %EFSD_FILE_EXISTS
 * events for all files in the directory, or, if for some reason the
 * command is issued on a file, for the file only. The directory is
 * not monitored afterwards, you just get the contents delivered once.
 */
EfsdCmdId      efsd_listdir(EfsdConnection *ec, char *dirname,
			    EfsdOptions *ops);

/* cp, mv, rm. You can multiple files through the FILES array, pass the
 *  length of the array in NUM_FILES. For mv and cp, if the number of
 *  files is > 2, the last file must be a directory, or the command will
 *  fail. If you want to remove only one file, simply pass the address
 *  of the char* of the file to be removed.
 *
 *  Apply options as desired -- see above.
 */
EfsdCmdId      efsd_copy(EfsdConnection *ec, int num_files, char **files, EfsdOptions *ops);
EfsdCmdId      efsd_move(EfsdConnection *ec, int num_files, char **files, EfsdOptions *ops);
EfsdCmdId      efsd_remove(EfsdConnection *ec, int num_files, char **files, EfsdOptions *ops);

/* Create a directory. Behaves like "mkdir -p", i.e. it can
 *  create directories recursively. Multiple slashes are
 *  treated as one, prefixed or suffixed slashes don't matter.
 */
EfsdCmdId      efsd_makedir(EfsdConnection *ec, char *dirname);

/* Chmods the given file to the given mode.
 */
EfsdCmdId      efsd_chmod(EfsdConnection *ec, char *filename,  mode_t mode);

/* Metadata operations.
 */
EfsdCmdId      efsd_set_metadata_raw(EfsdConnection *ec, char *key,
				     char *filename, EfsdDatatype datatype,
				     int datalength, void *data);
EfsdCmdId      efsd_set_metadata_int(EfsdConnection *ec, char *key,
				     char *filename, int val);
EfsdCmdId      efsd_set_metadata_float(EfsdConnection *ec, char *key,
				       char *filename, float val);
EfsdCmdId      efsd_set_metadata_str(EfsdConnection *ec, char *key,
				       char *filename, char *val);

EfsdCmdId      efsd_get_metadata(EfsdConnection *ec, char *key,
				 char *filename, EfsdDatatype datatype);

/* Convenience functions for acessing the metadata
 *  contained in a reply event. The return value is
 *  zero or NULL, respectively, when the EfsdEvent is
 *  not a reply to a command of type EFSD_CMD_GETMETA,
 *  or when the type of the metadata does not match
 *  the type requested, greater than zero otherwise.
 *
 *  No memory is allocated for returned pointers, so
 *  nothing needs to be freed.
 */
EfsdDatatype   efsd_metadata_get_type(EfsdEvent *ee);
int            efsd_metadata_get_int(EfsdEvent *ee, int *val);
int            efsd_metadata_get_float(EfsdEvent *ee, float *val);
char          *efsd_metadata_get_str(EfsdEvent *ee);
char          *efsd_metadata_get_key(EfsdEvent *ee);
char          *efsd_metadata_get_file(EfsdEvent *ee);

/* If you're for some reason not interested in the
 *  data length, you can pass DATA_LEN as NULL.
 */
void          *efsd_metadata_get_raw(EfsdEvent *ee, int *data_len);


/* Convenience function to access the filenames in reply or
   filechange events. If the event is a reply event and the
   contained command is an efsd_file_cmd, it returns the first file
   (efsd_file_cmd.files[0]). Returns NULL if no file could be
   found.
 */
char          *efsd_reply_filename(EfsdEvent *ee);

/* Convenience function to access the command ID in reply or
   filechange events. Returns -1 if no ID is contained in the event.
 */
EfsdCmdId      efsd_reply_id(EfsdEvent *ee);


/* Start/stop a FAM monitor for a given file or directory.
 *  Add options as desired, like with efsd_listdir().
 */
EfsdCmdId      efsd_start_monitor_file(EfsdConnection *ec, char *filename,
				       EfsdOptions *ops);
EfsdCmdId      efsd_start_monitor_dir(EfsdConnection *ec, char *filename,
				       EfsdOptions *ops);

EfsdCmdId      efsd_stop_monitor(EfsdConnection *ec, char *filename);

/* Returns the full file stats in the generated reply,
 *  as returned by the stat()/lstat() command.
 */
EfsdCmdId      efsd_stat(EfsdConnection *ec, char *filename);
EfsdCmdId      efsd_lstat(EfsdConnection *ec, char *filename);

/* Returns the file a symlink points to 
 */
EfsdCmdId      efsd_readlink(EfsdConnection *ec, char *filename);

/* Returns the filetype for a file 
 */
EfsdCmdId      efsd_get_filetype(EfsdConnection *ec, char *filename);


/* Command options:
 */

/**
 * efsd_ops - statically assemble an EfsdOptions object.
 * @num_options: The number of options that you create in the call.
 * You have to make this many option constructor calls (efsd_op_XXX())
 * afterwards.
 *
 * This is the solution for passing options to commands when you
 * know at compile time what options you want to pass. Returns
 * a pointer to a ready-made EfsdOptions object. You do NOT need
 * to free it after you've launched the command, it is
 * freed by the time the command routine returns.
 */
EfsdOptions  *efsd_ops(int num_options, ...);

EfsdOptions  *efsd_ops_create(int num_options);
void          efsd_ops_add(EfsdOptions *ops, EfsdOption *op);

/* Send stat events for all files seen in a directory: 
 */
EfsdOption    *efsd_op_get_stat(void);

/* Send lstat events for all files seen in a directory: 
 */
EfsdOption    *efsd_op_get_lstat(void);

/* Send metadata for certain key and data type for all files
 *  seen in a directory. The key is duplicated inside, i.e.
 *  you need not allocate a copy of the string before passing.
 */
EfsdOption    *efsd_op_get_metadata(char *key, EfsdDatatype type);

/* Send FILE type for all files seen in a directory
 */
EfsdOption    *efsd_op_get_filetype(void);

/* "Force" and "recursive" options for commands like rm, cp,
 *  mv, as known and loved on the command line.
 */
EfsdOption    *efsd_op_force(void);
EfsdOption    *efsd_op_recursive(void);

/* "Sort" option for efsd_start_monitor_dir --
 *  EFSD_FILE_EXISTS events are reported in alphabetical
 *  order when selected.
 */
EfsdOption    *efsd_op_sort(void);

/* Include files starting with . in listings, like ls -a.
 *  They're otherwise not reported.
 */
EfsdOption    *efsd_op_list_all(void);

#ifdef __cplusplus
}
#endif

#endif
