/* geist_style.c

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

#include "geist.h"
#include "geist_style.h"

geist_style *
geist_style_new(char *name)
{
   geist_style *s = NULL;

   D_ENTER(3);

   s = emalloc(sizeof(geist_style));

   memset(s, 0, sizeof(geist_style));
   if (name)
      s->name = estrdup(name);

   D_RETURN(3, s);
}

void
geist_style_free(geist_style * s)
{
   D_ENTER(3);

   if (s)
   {
      if (s->name)
         efree(s->name);
      if (s->bits)
      {
         geist_list *l;

         l = s->bits;
         while (l)
         {
            geist_style_bit_free((geist_style_bit *) l->data);
            l = l->next;
         }
         geist_list_free(s->bits);
      }
      efree(s);
   }
   D_RETURN_(3);
}

geist_style_bit *
geist_style_bit_new(int x_offset, int y_offset, int r, int g, int b, int a)
{
   geist_style_bit *sb;

   D_ENTER(3);

   sb = emalloc(sizeof(geist_style_bit));
   memset(sb, 0, sizeof(geist_style_bit));

   sb->x_offset = x_offset;
   sb->y_offset = y_offset;
   sb->r = r;
   sb->g = g;
   sb->b = b;
   sb->a = a;

   D_RETURN(3, sb);
}

void
geist_style_bit_free(geist_style_bit * s)
{
   D_ENTER(3);

   if (s)
   {
      efree(s);
   }

   D_RETURN_(3);
}

geist_style *
geist_style_dup(geist_style * s)
{
   geist_style *ret;

   D_ENTER(3);

   ret = geist_style_new(s->name);
   ret->bits = geist_list_dup_special(s->bits, geist_dup_style_bit);

   D_RETURN(3, ret);
}

void
geist_dup_style_bit(void **dest, void *data)
{
   D_ENTER(3);

   *dest = emalloc(sizeof(geist_style_bit));
   memcpy(*dest, data, sizeof(geist_style_bit));

   D_RETURN_(3);
}

void
geist_style_save_ascii(geist_style * style, char *file)
{
   FILE *stylefile;
   geist_list *l;
   geist_style_bit *b;

   D_ENTER(3);
   if (!style || !style->bits)
      D_RETURN_(3);

   stylefile = fopen(file, "w");
   if (stylefile)
   {
      fprintf(stylefile, "#Style\n");
      fprintf(stylefile, "#NAME %s\n", style->name);
      l = style->bits;
      while (l)
      {
         b = (geist_style_bit *) l->data;
         fprintf(stylefile, "%d %d %d %d %d %d\n", b->r, b->g, b->b, b->a,
                 b->x_offset, b->y_offset);
         l = l->next;
      }
   }
   fclose(stylefile);

   D_RETURN_(3);
}

geist_style *
geist_style_new_from_ascii(char *file)
{
   FILE *stylefile;
   char current[4096];
   char *s;
   geist_style *ret = NULL;

   D_ENTER(3);

   stylefile = fopen(file, "r");
   if (stylefile)
   {
      int r = 0, g = 0, b = 0, a = 0, x_off = 0, y_off = 0;

      ret = geist_style_new(NULL);
      /* skip initial idenifier line */
      fgets(current, sizeof(current), stylefile);
      while (fgets(current, sizeof(current), stylefile))
      {
         if (!strncmp(current, "#NAME", 5))
         {
            int l;

            l = strlen(current) - 1;
            if (current[l] == '\n')
               current[l] = '\0';
            if (l > 6)
               ret->name = estrdup(current + 6);
            D(2, ("got style name %s\n", ret->name));
            continue;
         }
         else
         {
            /* support EFM style bits */
            s = strtok(current, " ");
            if (strlen(s) == 2)
            {
               if (!strcmp(s, "ol"))
               {
                  r = g = b = 0;
                  s = strtok(NULL, " ");
                  x_off = atoi(s);
                  s = strtok(NULL, " ");
                  y_off = atoi(s);
               }
               else if (!strcmp(s, "sh"))
               {
                  r = g = b = 0;
                  s = strtok(NULL, " ");
                  x_off = atoi(s);
                  s = strtok(NULL, " ");
                  y_off = atoi(s);
                  s = strtok(NULL, " ");
                  a = atoi(s);
               }
               else if (!strcmp(s, "fg"))
               {
                  r = g = b = a = 0;
                  s = strtok(NULL, " ");
                  x_off = atoi(s);
                  s = strtok(NULL, " ");
                  y_off = atoi(s);
               }
            }
            else
            {
               /* our own format */
               r = atoi(s);
               s = strtok(NULL, " ");
               g = atoi(s);
               s = strtok(NULL, " ");
               b = atoi(s);
               s = strtok(NULL, " ");
               a = atoi(s);
               s = strtok(NULL, " ");
               x_off = atoi(s);
               s = strtok(NULL, " ");
               y_off = atoi(s);
            }
         }
         ret->bits =
            geist_list_add_end(ret->bits,
                               geist_style_bit_new(x_off, y_off, r, g, b, a));
      }
      fclose(stylefile);
   }

   D_RETURN(3, ret);
}
