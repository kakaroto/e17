/* stalk_file.c

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

#include "stalk_file.h"
#include "stalk_line.h"

stalk_file *
stalk_file_new(char *filename)
{
   stalk_file *ret;
   char *s;

   ret = (stalk_file *) emalloc(sizeof(stalk_file));
   memset(ret, 0, sizeof(stalk_file));
   ret->filename = estrdup(filename);
   s = strrchr(filename, '/');
   if (s)
      ret->name = estrdup(s + 1);
   else
      ret->name = estrdup(filename);
   ret->fp = fopen(filename, "r");
   if (!ret->fp)
   {
      weprintf("unable to open file %s for reading:", filename);
      stalk_file_free(ret);
      return NULL;
   }
   if (fstat(fileno(ret->fp), &(ret->st)) == -1)
   {
      weprintf("unable to stat file %s:", filename);
      stalk_file_free(ret);
      return NULL;
   }
   /* need to read from the beginning */
   ret->st.st_size = 0;

   return ret;
}

void
stalk_file_free(stalk_file * file)
{
   if (!file)
      return;
   if (file->filename)
      free(file->filename);
   if (file->name)
      free(file->name);
   if (file->fp)
      fclose(file->fp);
   free(file);
}

/* get all lines from file after position pos */
gib_list *
stalk_file_get_new_lines(stalk_file * file, int *error)
{
   char buffer[BUFSIZ];
   gib_list *ret = NULL;
   struct stat st;
   gib_list *last = NULL;
   gib_list *l;

   *error = READ_ERROR_NONE;

   if (!file)
   {
      weprintf("no file");
      *error = READ_ERROR_UNKNOWN;
      return NULL;
   }
   if (!file->fp)
   {
      weprintf("no file pointer");
      stalk_file_free(file);
      *error = READ_ERROR_UNKNOWN;
      return NULL;
   }

   D(3, ("previous st_size: %d\t", file->st.st_size));
   if (fstat(fileno(file->fp), &st) == -1)
   {
      weprintf("unable to stat file %s:", file->filename);
      stalk_file_free(file);
      *error = READ_ERROR_STAT;
      return NULL;
   }
   D(3, ("current st_size: %d\n", st.st_size));
   if (file->st.st_size < st.st_size)
   {
      D(1, ("file grew\n"));

      fseek(file->fp, file->st.st_size, SEEK_SET);

      while (!feof(file->fp))
      {
         if (fgets(buffer, sizeof(buffer), file->fp))
         {
            /* append line to line list */
            int len;
            stalk_line *line;

            len = strlen(buffer);
            /* trim \n */
            if (buffer[len - 1] == '\n')
               buffer[--len] = '\0';
            /* printf("file %s - got line: %s\n", file->filename, buffer); */
            line = stalk_line_new(buffer, len, file->filename);
            if (line)
            {
               /* add_end is too slow here, we keep a reference to the end of the
                * list */
               l = gib_list_new();
               l->prev = last;
               l->data = line;
               if (!ret)
                  ret = l;
               else
                  last->next = l;
               last = l;
            }
         }
      }

      file->st = st;

      return ret;
   }
   else
   {
      *error = READ_ERROR_NONE;
      return NULL;
   }
}
