/* stalk_line.c

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

#include "stalk_line.h"

stalk_line *
stalk_line_new(char *line, int len, char *filename)
{
   stalk_line *ret;

   ret = (stalk_line *) emalloc(sizeof(stalk_line));
   memset(ret, 0, sizeof(stalk_line));
   ret->line = estrdup(line);
   ret->len = len;
   ret->filename = estrdup(filename);

   return ret;
}

void
stalk_line_free(stalk_line *line)
{
   if (!line)
      return;
   if (line->filename)
      free(line->filename);
   if (line->line)
      free(line->line);
   free(line);
}

char * stalk_line_get_string(stalk_line *line)
{
   char * ret;
   if(gib_list_has_more_than_one_item(opt.files))
      ret = estrjoin("", line->filename, "  ", line->line, NULL);
   else
      ret = estrdup(line->line);
   return ret;
}
