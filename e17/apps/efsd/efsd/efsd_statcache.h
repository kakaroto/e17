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
#ifndef efsd_statcache_h
#define efsd_statcache_h

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void         efsd_stat_init(void);
void         efsd_stat_cleanup(void);

/* These are the implementations of Efsd's stat()/lstat()
   commands.
*/
int          efsd_stat(char *filename, struct stat *st);
int          efsd_lstat(char *filename, struct stat *st);

/* Removes a file from the stat/lstat caches. MONITOR_UPDATE
   specifies whether or not the monitoring system should be
   notified of the change or not.
*/
void         efsd_stat_remove(char *filename, int monitor_update);

/* Changes the name of a file in the statcaches. */
void         efsd_stat_change_filename(char *file1, char *file2);

#endif
