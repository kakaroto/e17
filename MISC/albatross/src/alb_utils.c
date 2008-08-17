/* alb_utils.c

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

#include "alb_utils.h"
#include "gib_list.h"

gib_list *
alb_string_split(const char *string,
                 const char *delimiter)
{
  gib_list *string_list = NULL;
  char *s;
  unsigned int n = 1;

  D_ENTER(3);

  if (!string || !delimiter)
    D_RETURN(3, NULL);

  s = strstr(string, delimiter);
  if (s) {
    unsigned int delimiter_len = strlen(delimiter);

    do {
      unsigned int len;
      char *new_string;

      len = s - string;
      new_string = emalloc(sizeof(char) * (len + 1));

      strncpy(new_string, string, len);
      new_string[len] = 0;
      string_list = gib_list_add_front(string_list, new_string);
      n++;
      string = s + delimiter_len;
      s = strstr(string, delimiter);
    }
    while (s);
  }
  if (*string) {
    n++;
    string_list = gib_list_add_front(string_list, estrdup((char *) string));
  }

  string_list = gib_list_reverse(string_list);

  D_RETURN(3, string_list);
}
