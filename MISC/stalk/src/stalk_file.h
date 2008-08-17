/* stalk_file.h

Copyright (C) 1999,2000 Tom Gilbert.

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

#ifndef STALK_FILE_H
#define STALK_FILE_H

#include "stalk.h"
#include "structs.h"

enum __read_error {
   READ_ERROR_NONE,
   READ_ERROR_NO_FILE,
   READ_ERROR_STAT,
   READ_ERROR_UNKNOWN
};

struct __stalk_file
{
   char *filename;
   char *name;

   FILE *fp;
   struct stat st;
};

stalk_file *stalk_file_new(char *filename);
void stalk_file_free(stalk_file *file);
gib_list *stalk_file_get_new_lines(stalk_file * file, int *error);

#define STALK_FILE(l) ((stalk_file *) l)

#endif
