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
#ifndef __efsd_meta_h
#define __efsd_meta_h

#include <efsd.h>

void        efsd_meta_init(void);
void        efsd_meta_cleanup(void);


/**
 * efsd_meta_set - Metadata setting function.
 * @ec:            Efsd command
 *
 * This function is called when a client wants to set metadata
 * on a file.
 */
int         efsd_meta_set(EfsdCommand *ec);

/**
 * efsd_meta_get - Metadata retrieval function.
 * @ec:            Efsd command
 * @data_len:      Result pointer that receives the length of the metadata chunk.
 *
 * This function is called when a client wants to retrieve
 * metadata associated with a file.
 */
void       *efsd_meta_get(EfsdCommand *ec, int *data_len);

/**
 * efsd_meta_copy_data - Copies metadata from one file to another.
 * @from_file:    The file from which to copy the metadata
 * @to_file:      The file to which to add the metadata
 *
 * This function copies metadata from one file to another, as
 * it is needed for example after copying files.
 */
int         efsd_meta_copy_data(char *from_file, char *to_file);

/**
 * efsd_meta_move_data - Moves metadata from one file to another.
 * @from_file:    The file from which to take the metadata
 * @to_file:      The file to which to add the metadata
 *
 * This function moves metadata from one file to another, as
 * it is needed for example after moving files.
 */
int         efsd_meta_move_data(char *from_file, char *to_file);

/**
 * efsd_meta_remove_data - Removes metadata for a file.
 * @file:   The file whose metadata needs to be removed.
 *
 * This function removes all metadata associated with a file.
 */
int         efsd_meta_remove_data(char *file);

/**
 * efsd_meta_dir_cleanup - Cleans up empty directories.
 * @dir: Directory to clean up.
 *
 * This function checks whether the given file contains
 * any real files (files other than ".", ".." and the metadata
 * directory), and if not, removes the metadata directory.
 */
void        efsd_meta_dir_cleanup(char *dir);

/**
 * efsd_meta_idle - Idlehandler for metadata module
 *
 * This function is an idlehandler that tries to flush
 * the metadata db settings to disk.
 */
void        efsd_meta_idle(void);

#endif
