/* gib_imlib.c

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

#include "geist_imlib.h"

int
geist_imlib_load_image(Imlib_Image * im, char *filename)
{
   Imlib_Load_Error err;

   imlib_context_set_progress_function(NULL);
   if (!filename)
      return (0);
   *im = imlib_load_image_with_error_return(filename, &err);
   if ((err) || (!im))
   {
      /* Check error code */
      switch (err)
      {
        case IMLIB_LOAD_ERROR_FILE_DOES_NOT_EXIST:
           weprintf("%s - File does not exist", filename);
           break;
        case IMLIB_LOAD_ERROR_FILE_IS_DIRECTORY:
           weprintf("%s - Directory specified for image filename", filename);
           break;
        case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_READ:
           weprintf("%s - No read access to directory", filename);
           break;
        case IMLIB_LOAD_ERROR_NO_LOADER_FOR_FILE_FORMAT:
           weprintf("%s - No Imlib2 loader for that file format", filename);
           break;
        case IMLIB_LOAD_ERROR_PATH_TOO_LONG:
           weprintf("%s - Path specified is too long", filename);
           break;
        case IMLIB_LOAD_ERROR_PATH_COMPONENT_NON_EXISTANT:
           weprintf("%s - Path component does not exist", filename);
           break;
        case IMLIB_LOAD_ERROR_PATH_COMPONENT_NOT_DIRECTORY:
           weprintf("%s - Path component is not a directory", filename);
           break;
        case IMLIB_LOAD_ERROR_PATH_POINTS_OUTSIDE_ADDRESS_SPACE:
           weprintf("%s - Path points outside address space", filename);
           break;
        case IMLIB_LOAD_ERROR_TOO_MANY_SYMBOLIC_LINKS:
           weprintf("%s - Too many levels of symbolic links", filename);
           break;
        case IMLIB_LOAD_ERROR_OUT_OF_MEMORY:
           eprintf("While loading %s - Out of memory", filename);
           break;
        case IMLIB_LOAD_ERROR_OUT_OF_FILE_DESCRIPTORS:
           eprintf("While loading %s - Out of file descriptors", filename);
           break;
        case IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_WRITE:
           weprintf("%s - Cannot write to directory", filename);
           break;
        case IMLIB_LOAD_ERROR_OUT_OF_DISK_SPACE:
           weprintf("%s - Cannot write - out of disk space", filename);
           break;
        case IMLIB_LOAD_ERROR_UNKNOWN:
        default:
           weprintf
              ("While loading %s - Unknown error. Attempting to continue",
               filename);
           break;
      }
      return (0);
   }
   return (1);
}

DATA8
geist_imlib_image_part_is_transparent(Imlib_Image im, int x, int y)
{
   Imlib_Color c;
   int num = 0;
   int ave = 0;
   int w, h;
   int leftmost, rightmost, topmost, bottommost, i, j;

   imlib_context_set_image(im);
   w = imlib_image_get_width();
   h = imlib_image_get_height();

   if ((x > w) || (y > h))
      return 1;

   leftmost = x - 1;
   if (leftmost < 0)
      leftmost = 0;
   rightmost = x + 1;
   if (rightmost > w)
      rightmost = w;
   topmost = y - 1;
   if (topmost < 0)
      topmost = 0;
   bottommost = y + 1;
   if (bottommost > h)
      bottommost = h;

   for (i = leftmost; i < rightmost + 1; i++)
      for (j = topmost; j < bottommost + 1; j++)
      {
         imlib_image_query_pixel(i, j, &c);
         ave += c.alpha;
         num++;
      }

   ave = ave / num;

/* TODO Make this fuzziness an OPTION */
   if (ave > TRANS_THRESHOLD)
      return 0;

   return 1;
}
