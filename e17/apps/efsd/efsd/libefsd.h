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

#include <efsd.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef	FALSE
#define	FALSE	(0)
#endif

#ifndef	TRUE
#define	TRUE	(!FALSE)
#endif

/* Efsd connection: */
typedef struct efsd_connection EfsdConnection;
typedef struct efsd_options EfsdOptions;


/* Function prototype typedefs for the various file change
   event handlers: */

typedef void (*EfsdFileEventFunc) (EfsdFileChangeEvent *ev);
typedef void (*EfsdFileMetadataEventFunc) (EfsdMetadataChangeEvent *ev);
typedef void (*EfsdReplyFunc) (EfsdReplyEvent *ev);

/* And a struct that puts them all together. Look at the
   efsd_callbacks_... functions */
typedef struct efsd_event_callbacks
{
  EfsdFileEventFunc          changed_cb;
  EfsdFileEventFunc          delete_cb;
  EfsdFileEventFunc          startexec_cb;
  EfsdFileEventFunc          stopexec_cb;
  EfsdFileEventFunc          created_cb;
  EfsdFileEventFunc          moved_cb;
  EfsdFileEventFunc          ack_cb;
  EfsdFileEventFunc          exists_cb;
  EfsdFileEventFunc          endexists_cb;
  EfsdFileMetadataEventFunc  metadata_cb;
  EfsdReplyFunc              reply_cb;
}
EfsdEventCallbacks;



/**
 * efsd_open - Creates and returns an efsd connection. 
 * 
 * Returns pointer to a newly allocated and initialized
 * Efsd connection structure. You need this structure for all
 * other calls in order to identify the connection to
 * libefsd.
 */
EfsdConnection *efsd_open(void);


/**
 * efsd_close - Closes a connection to Efsd.
 * @ec: The Efsd connection
 *
 * Use this to close an efsd connection.
 * Frees the allocated EfsdConnection structure.
 * Returns value < 0 if the the final
 * command could not be sent to Efsd.
 */
int            efsd_close(EfsdConnection *ec);


#if HAVE_ECORE
/** 
 * efsd_event_callback_register
 * @func: the function to register
 * 
 */
void efsd_event_callback_register(void* func);
#endif


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
 * greater than zero, zero when there are none, and a value smaller
 * than zero when there was an error.
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
 * efsd_dispatch_event - handles events through the dispatching mechanism.
 * @ev: The Efsd event to dispatch.
 *
 * This function looks up the set of callbacks for the command ID contained
 * in the event, and then dispatches this event to the appropriate callback.
 * If no set of callbacks is found or the matching callback isn't defined
 * in the EfsdEventCallbacks struct, nothing happens.
 *
 * Returns -1 when an error occurs, FALSE when the event didn't get
 * dispatched, and TRUE when it got dispatched.
 */
int            efsd_dispatch_event(EfsdEvent *ev);


/** 
 * efsd_wait_event - blocking wait for next Efsd event.
 * @ec: The Efsd connection
 * @ev: Pointer to an allocated EfsdEvent.
 *
 * Blocks until an efsd event arrives, then returns it by filling
 * in the @ev structure. Returns -1 when there was an error,
 * >= 0 otherwise.
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

/**
 * efsd_commands_pending - whether there are commands still waiting to be sent to Efsd.
 * @ec: The Efsd connection
 *
 * Use this function to find out whether commands are currently queued
 * on the client side. Returns value > 0 when commands are queued, 0 when
 * the queue is empty, value < 0 when an error occured.
 */
int            efsd_commands_pending(EfsdConnection *ec);

/**
 * efsd_flush - tries to flush the queue of unsent commands.
 * @ec: The Efsd connection
 *
 * When clients send bursts of commands to the server, it can happen that
 * Efsd cannot immediately accept a command. In that case, commands are
 * queued on the client side. This queue can be flushed using this command.
 * This function tries to send as many commands as possible, and then 
 * returns. It does not block. Returns value > 0 when the queue could be
 * flushed, 0 otherwise, < 0 when there was an error.
 */
int            efsd_flush(EfsdConnection *ec);


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
 *  which are plugged together into an EfsdOptions structure.
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
 * @ops: Efsd options structure.
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

/**
 * efsd_copy - copies a number of files to a target file.
 * @ec: The Efsd connection
 * @num_files: The number of files passed
 * @files: Array of strings, must contain at least @num_files items.
 * @ops: EfsdOptions pointer, created using either efsd_ops() or
 * efsd_ops_create() and efsd_ops_add().
 *
 * Copies files. If @num_files > 2, the last file must be a directory.
 * Currently, meaningful options are efsd_op_force() and efsd_op_recursive().
 * The force option causes existing files at the target to be removed.
 */
EfsdCmdId      efsd_copy(EfsdConnection *ec, int num_files, char **files, EfsdOptions *ops);

/**
 * efsd_move - moves a number of files to a target file.
 * @ec: The Efsd connection
 * @num_files: The number of files passed
 * @files: Array of strings, must contain at least @num_files items.
 * @ops: EfsdOptions pointer, created using either efsd_ops() or
 * efsd_ops_create() and efsd_ops_add().
 *
 * Moves files. If @num_files > 2, the last file must be a directory.
 * Currently, the only meaningful option is efsd_op_force(),
 * which causes existing files at the target to be removed.
 */
EfsdCmdId      efsd_move(EfsdConnection *ec, int num_files, char **files, EfsdOptions *ops);

/**
 * efsd_remove - removes a number of files.
 * @ec: The Efsd connection
 * @num_files: The number of files passed
 * @files: Array of strings, must contain at least @num_files items.
 * @ops: EfsdOptions pointer, created using either efsd_ops() or
 * efsd_ops_create() and efsd_ops_add().
 *
 * Copies files. If @num_files > 2, the last file must be a directory.
 * Currently, meaningful options are efsd_op_force() and efsd_op_recursive().
 * The force option causes the success status of the command to be
 * "success" even when targets don't exist.
 */
EfsdCmdId      efsd_remove(EfsdConnection *ec, int num_files, char **files, EfsdOptions *ops);

/** 
 * efsd_makedir - creates a directory.
 * @ec: The Efsd connection
 * @dirname: The directory path.
 *
 * efsd_makedir() behaves like mkdir -p, i.e. it
 * can create directories recursively.
 */
EfsdCmdId      efsd_makedir(EfsdConnection *ec, char *dirname);

/**
 * efsd_chmod - change permissions on a file.
 * @ec: The Efsd connection
 * @filename: The file whose permissions are to be changed
 * @mode: the new permissions
 *
 * Changes the given file to the given mode.
 */
EfsdCmdId      efsd_chmod(EfsdConnection *ec, char *filename,  mode_t mode);


/* Metadata operations.
 */

/**
 * efsd_set_metadata_raw - set raw binary metadata on a file.
 * @ec: The Efsd connection
 * @key: A character string which unambiguously identifies the data
 * @datatype: The type of the data. One of %EFSD_INT,
 * %EFSD_FLOAT, %EFSD_STRING or %EFSD_RAW.
 * @filename: The file on which to set the metadata.
 * @datalength: The length of the data chunk, in bytes.
 * @data: The data itself.
 *
 * This command sets arbitrary binary data as metadata on a file.
 * The data is labeled by the given key, which must therefore be
 * unique among all the metadata set for a file.
 */
EfsdCmdId      efsd_set_metadata_raw(EfsdConnection *ec, char *key,				     
				     char *filename, EfsdDatatype datatype,
				     int datalength, void *data);

/**
 * efsd_set_metadata_int - set integer metadata on a file.
 * @ec: The Efsd connection
 * @key: A character string which unambiguously identifies the data
 * @filename: The file on which to set the metadata.
 * @val: An integer value.
 *
 * This command sets an integer value as metadata on a file.
 * The data is labeled by the given key, which must therefore be
 * unique among all the metadata set for a file.
 */
EfsdCmdId      efsd_set_metadata_int(EfsdConnection *ec, char *key,
				     char *filename, int val);

/**
 * efsd_set_metadata_float - set floating-point metadata on a file.
 * @ec: The Efsd connection
 * @key: A character string which unambiguously identifies the data
 * @filename: The file on which to set the metadata.
 * @val: A floating-point value.
 *
 * This command sets a floating point value as metadata on a file.
 * The data is labeled by the given key, which must therefore be
 * unique among all the metadata set for a file.
 */
EfsdCmdId      efsd_set_metadata_float(EfsdConnection *ec, char *key,
				       char *filename, float val);

/**
 * efsd_set_metadata_str - set character string metadata on a file.
 * @ec: The Efsd connection
 * @key: A character string which unambiguously identifies the data
 * @filename: The file on which to set the metadata.
 * @val: A C string.
 *
 * This command sets C string data as metadata on a file.
 * The data is labeled by the given key, which must therefore be
 * unique among all the metadata set for a file.
 */
EfsdCmdId      efsd_set_metadata_str(EfsdConnection *ec, char *key,
				       char *filename, char *val);

/**
 * efsd_get_metadata - retrieve file metadata.
 * @ec: The Efsd connection
 * @key: A character string which unambiguously identifies the data
 * @filename: The file on which to set the metadata.
 * @datatype: The datatype of the data to retrieve. One of %EFSD_INT,
 * %EFSD_FLOAT, %EFSD_STRING or %EFSD_RAW.
 *
 * This command causes the metadata indexed by the given key to be
 * transmitted to the client.
 */
EfsdCmdId      efsd_get_metadata(EfsdConnection *ec, char *key,
				 char *filename, EfsdDatatype datatype);

/* Convenience functions for acessing the metadata
 * contained in a reply event. The return value is
 * zero or NULL, respectively, when the EfsdEvent is
 * not a reply to a command of type EFSD_CMD_GETMETA,
 * or when the type of the metadata does not match
 * the type requested, greater than zero otherwise.
 *
 * No memory is allocated for returned pointers, so
 * nothing needs to be freed. The metadata is cleaned
 * up when the EfsdEvent is cleaned up, so if you want
 * to keep the data around, you'll have to memcpy() it. 
 */

/**
 * efsd_metadata_get_type - returns the data type of retrieved metadata.
 * @ee: The received EfsdEvent.
 * 
 * Convenience function that returns the data type of the metadata
 * received. If the event does not contain any metadata, the return
 * value is 0 (which is not a valid data type), > 0 otherwise.
 */
EfsdDatatype   efsd_metadata_get_type(EfsdEvent *ee);

/**
 * efsd_metadata_get_int - returns integer value from retrieved metadata.
 * @ee: The received EfsdEvent.
 * @val: Pointer to an integer that is overwritten with the received value.
 *
 * Convenience function that returns integer metadata from a reply
 * event in @val. If the event does not contain integer metadata,
 * the function returns 0, a value > 0 otherwise.
 */
int            efsd_metadata_get_int(EfsdEvent *ee, int *val);

/**
 * efsd_metadata_get_float - returns floating-point value from retrieved metadata.
 * @ee: The received EfsdEvent.
 * @val: Pointer to a float that is overwritten with the received value.
 *
 * Convenience function that returns floating-point metadata from a reply
 * event in @val. If the event does not contain floating-point metadata,
 * the function returns 0, a value > 0 otherwise.
 */
int            efsd_metadata_get_float(EfsdEvent *ee, float *val);

/**
 * efsd_metadata_get_str - returns a character string from retrieved metadata.
 * @ee: The received EfsdEvent.
 *
 * Convenience function that returns character string metadata from a reply
 * event in @val. If the event does not contain string metadata,
 * the function returns %NULL, the string otherwise. You do not need to free()
 * the string, it gets deallocated when the event gets cleaned up. So when you
 * want to keep it around, you need to strdup() it.
 */
char          *efsd_metadata_get_str(EfsdEvent *ee);

/**
 * efsd_metadata_get_raw -  returns the raw data from retrieved metadata.
 * @ee: The received EfsdEvent.
 * @data_len: Pointer to an integer that returns the length of the data chunk.
 *
 * Convenience function that returns raw metadata from a reply
 * event. If the event does not contain string metadata, the function returns
 * NULL, the data otherwise. You do not need to free() the data, it gets
 * deallocated when the event gets cleaned up. So when you
 * want to keep it around, you need to memdup() it. If for some reason you're
 * not interested in the data length, you can pass @data_len as %NULL.
 */
void          *efsd_metadata_get_raw(EfsdEvent *ee, int *data_len);

/**
 * efsd_metadata_get_key - returns key that identifies retrieved metadata.
 * @ee: The received EfsdEvent.
 *
 * Convenience function that returns the key that was used to look up
 * the metadata returned to the client. If the event does not contain
 * any metadata, the return value is %NULL. You do not need to free()
 * the string, it gets deallocated when the event gets cleaned up. So when you
 * want to keep it around, you need to strdup() it.
 */
char          *efsd_metadata_get_key(EfsdEvent *ee);

/**
 * efsd_metadata_get_file - returns the full filename for which metadata
 * was retrieved.
 * @ee: The received EfsdEvent.
 *
 * Convenience function that returns the filename from a metadata reply
 * event. If the event does not contain metadata, the function returns
 * %NULL, the filename otherwise. You do not need to free() the string,
 * it gets deallocated when the event gets cleaned up. So when you
 * want to keep it around, you need to strdup() it.
 */
char          *efsd_metadata_get_file(EfsdEvent *ee);


/**
 * efsd_event_filename - returns filename contained in an event.
 * @ee: The EfsdEvent.
 * 
 * Convenience function to access the filenames in received
 * events. If the event is a reply event and the
 * contained command is an efsd_file_cmd, it returns the first file
 * (efsd_file_cmd.files[0]). For filechange events, it returns
 * efsd_filechange_event.file. Returns %NULL otherwise.
 */
char          *efsd_event_filename(EfsdEvent *ee);


/**
 * efsd_event_id - returns command id contained in an event.
 * @ee: The EfsdEvent.
 * 
 * Convenience function to access the command ID in received
 * events. Returns -1 if no ID is contained in the event.
 */
EfsdCmdId      efsd_event_id(EfsdEvent *ee);


/**
 * efsd_event_data - returns data contained in a event
 * @ee: The EfsdEvent.
 *
 * Convenience function access the data returned in a received
 * event. Returns NULL if the event does not contain any
 * returned data, or if the command is not applicable.
 */
void          *efsd_event_data(EfsdEvent *ee);


/**
 * efsd_start_monitor - start monitoring a directory for file events.
 * @ec: The Efsd connection
 * @filename: The name of the directory that is to be monitored.
 * @ops: Pointer to EfsdOptions.
 * @dir_mode: Whether this is a directory monitor or not, either TRUE
 * or FALSE.
 *
 * Use this command when you want to be informed when things happen to
 * a directory and all of the files contained in it. You will receive
 * a series of %EFSD_FILE_EXISTS events for all files in the directory.
 * You know that no further %EFSD_FILE_EXISTS events will be received when
 * you see %EFDS_FILE_END_EXISTS.
 *
 * If you pass options, they are applied to every file for which an
 * %EFSD_FILE_EXISTS event is generated. Therefore, meaningful options
 * are efsd_op_get_stat, efsd_op_get_lstat(), efsd_op_get_metadata(),
 * efsd_op_get_filetype(), efsd_op_sort() and efsd_op_list_all().
 * You can rely on the fact that the %EFSD_FILE_EXISTS events will be
 * received before any results of options applied to the files. You
 * can not rely on receiving the %EFSD_FILE_EXISTS events and all the
 * results generated through the options, before receiving the
 * %EFSD_FILE_EXISTS event for the next file. 
 */
EfsdCmdId      efsd_start_monitor(EfsdConnection *ec, char *filename,
				  EfsdOptions *ops, int dir_mode);

/**
 * efsd_stop_monitor - stops monitoring a file or directory.
 * @ec: The Efsd connection.
 * @filename: The file that is to be no longer monitored.
 * @dir_mode: Whether this is a directory monitor or not, either TRUE
 * or FALSE.
 *
 * This command stops reporting of filechange events to @filename (and,
 * if @filename is a directory, any files in the directory).
 * Clients receive a %EFSD_FILE_ACKNOWLEDGE event as a reply, which
 * guarantees that no further file change events for this file will
 * be sent to the client that requested the monitoring to be stopped.
 */
EfsdCmdId      efsd_stop_monitor(EfsdConnection *ec, char *filename, int dir_mode);


/**
 * efsd_start_monitor_metadata - starts monitoring metadata.
 * @ec: The Efsd connection.
 * @filename: The file with metadata entries to be monitored.
 * @key: The key of the metadata item that is to be monitored.
 *
 * This command requests monitoring of the metadata associated
 * to the given @key on the given @filename. The client issuing
 * this command will receive events of type %EFSD_EVENT_METADATA_CHANGE
 * when the specified metadata entry changes.
 */
EfsdCmdId      efsd_start_monitor_metadata(EfsdConnection *ec, char *filename, char *key);

/**
 * efsd_stop_monitor_metadata - stops monitoring metadata.
 * @ec: The Efsd connection.
 * @filename: The file with a metadata entry that is monitored.
 * @key: The key of the metadata item that no longer needs to be monitored.
 *
 * This command stops monitoring of the metadata associated
 * to the given @key on the given @filename.
 */
EfsdCmdId      efsd_stop_monitor_metadata(EfsdConnection *ec, char *filename, char *key);

/**
 * efsd_stat - returns the result of stat() on a file.
 * @ec: The Efsd connection.
 * @filename: The name of the file that is to be stat()ed.
 */
EfsdCmdId      efsd_stat(EfsdConnection *ec, char *filename);

/**
 * efsd_lstat - returns the result of lstat() on a file.
 * @ec: The Efsd connection.
 * @filename: The name of the file that is to be lstat()ed.
 */
EfsdCmdId      efsd_lstat(EfsdConnection *ec, char *filename);

/**
 * efsd_readlink - returns the file a symlink points to.
 * @ec: The Efsd connection.
 * @filename: The name of the symbolic link whose target is to be read.
 */
EfsdCmdId      efsd_readlink(EfsdConnection *ec, char *filename);

/**
 * efsd_get_filetype - returns the filetype of a file.
 * @ec: The Efsd connection.
 * @filename: The name of the file whose type is to be computed.
 *
 * This function returns the filetype of a file, using a fairly
 * sophisticated algorithm not unlike that of the file(1) command.
 */
EfsdCmdId      efsd_get_filetype(EfsdConnection *ec, char *filename);


/* Command options:
 */

/**
 * efsd_ops - statically assemble an EfsdOptions structure.
 * @num_options: The number of options that you create in the call.
 * You have to make this many option constructor calls (efsd_op_XXX())
 * afterwards.
 *
 * This is the solution for passing options to commands when you
 * know at compile time what options you want to pass. Returns
 * a pointer to a ready-made EfsdOptions structure. You do NOT need
 * to free it after you've launched the command, it is
 * freed by the time the command routine returns.
 */
EfsdOptions  *efsd_ops(int num_options, ...);

/**
 * efsd_ops_create - create an empty EfsdOptions structure.
 *
 * Use this function when you need to pass a number of options and do not
 * know their number at compile time. Pass the returned EfsdOptions structure
 * to subsequent efsd_ops_add() calls, then pass it to the actual command.
 */
EfsdOptions  *efsd_ops_create(void);

/**
 * efsd_ops_add - adds an option to an EfsdOptions structure.
 * @ops: EfsdOptions structure, created via efsd_ops_create()
 * @op: Option to add, created via one of the efsd_op_XXX() calls
 *
 * Add options to an EfsdOptions structure using this function.
 */
void          efsd_ops_add(EfsdOptions *ops, EfsdOption *op);

/**
 * efsd_op_get_stat - requests stat events
 * 
 * Creates an option that causes stat() results to be sent to the
 * client for all files seen in %EFSD_FILE_EXIST events.
 */
EfsdOption    *efsd_op_get_stat(void);

/**
 * efsd_op_get_stat - requests lstat events
 * 
 * Creates an option that causes lstat() results to be sent to the
 * client for all files seen in %EFSD_FILE_EXIST events.
 */
EfsdOption    *efsd_op_get_lstat(void);

/**
 * efsd_op_get_metadata - request metadata information
 * @key: character string that identifies the metadata
 * @type: datatype of the metadata, one of %EFSD_INT,
 * %EFSD_FLOAT, %EFSD_STRING or %EFSD_RAW.
 *
 * Creates an option that causes metadata of certain key and type
 * to be sent to the client, for all files reported in %EFSD_FILE_EXIST
 * events.
 */
EfsdOption    *efsd_op_get_metadata(char *key, EfsdDatatype type);

/**
 * efsd_op_get_filetype - request filetype information
 *
 * Creates an option that causes file type information to be sent
 * to the client, for all files reported in %EFSD_FILE_EXIST
 * events.
 */
EfsdOption    *efsd_op_get_filetype(void);

/**
 * efsd_op_force - request "-f" behaviour
 *
 * Creates an option that causes commands like efsd_move(), efsd_copy()
 * and efsd_remove() to operate like the command-line versions, when
 * passed the "-f" option.
 */
EfsdOption    *efsd_op_force(void);

/**
 * efsd_op_force - request recursive behaviour
 *
 * Creates an option that causes commands like efsd_copy()
 * and efsd_remove() to operate like the command-line versions, when
 * passed the "-r" option.
 */
EfsdOption    *efsd_op_recursive(void);

/**
 * efsd_op_sort - report %EFSD_FILE_EXISTS events alphabetically.
 *
 * This option constructor returns an EfsdOption that causes %EFSD_FILE_EXISTS events to
 * be reported in alphabetical order.
 */
EfsdOption    *efsd_op_sort(void);

/**
 * efsd_op_list_all - include hidden files in %EFSD_FILE_EXISTS events.
 * 
 * This option constructor returns an EfsdOption that causes Efsd to send
 * %EFSD_FILE_EXISTS also for hidden files (starting with a '.').
 */
EfsdOption    *efsd_op_list_all(void);



/**
 * efsd_callbacks_create - returns an initialized event callback struct.
 *
 */
EfsdEventCallbacks *efsd_callbacks_create(void);

/**
 * efsd_callbacks_cleanup - cleans up memory occupied by a callbacks struct.
 * @callbacks: callbacks structure to clean up.
 */
void efsd_callbacks_cleanup(EfsdEventCallbacks *callbacks);


/**
 * efsd_callbacks_register - registers a set of handlers for a particular monitor.
 * @id: Command ID of the monitoring request.
 * @callbacks: Initialized callback set.
 *
 * You can register a set of event handlers for a particular file monitor
 * here. Pass the command ID that was returned by the corresponding
 * monitor request call as first argument, and an EfsdEventCallback struct
 * that is initialized properly second. You don't need to initialize
 * callbacks you don't want to use.
 *
 * If you want to change the set of callbacks for an ID, just call this
 * function again, you'll get the old set in return (and NULL otherwise).
 * Similarly, if you want to remove a callback set for a command ID, just
 * pass NULL as the second argument.
 */
EfsdEventCallbacks *efsd_callbacks_register(EfsdCmdId id, EfsdEventCallbacks *callbacks);

#ifdef __cplusplus
}
#endif

#endif
