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
#ifndef __efsd_filetype_h
#define __efsd_filetype_h

/* Initializes the filetype tests. Returns value > 0
   when at least a minum number of necessary db's
   could be found.
 */
int        efsd_filetype_init(void);

/* These re-read the databases. */
void       efsd_filetype_update_system_settings(void);
void       efsd_filetype_update_user_settings(void);

/* Clears the current magic test hierarchy.
 */
void       efsd_filetype_cleanup(void);

/* Returns filetype for a given file
   in TYPE, which is of size LEN.
*/
int        efsd_filetype_get(char *filename, char *type, int len);

/* These are not threadsafe on first call -- but they
   get properly initialized before any multithreading
   happens ... */
char      *efsd_filetype_get_system_file(void);
char      *efsd_filetype_get_user_file(void);

/* Saves the current magic tests db to an XML file specified
   in FILENAME. Returns FALSE when an error occurred.*/
int        efsd_filetype_save_user_settings_to_file(const char *filename);
int        efsd_filetype_save_system_settings_to_file(const char *filename);

#endif
