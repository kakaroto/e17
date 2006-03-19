/*
 * Based off of Peter Alm's BMP loader from xmms, with additions from
 * imlib's old BMP loader
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "common.h"
#include <sys/stat.h>
#include <string.h>

#include "image.h"

char                load(ImlibImage * im, ImlibProgressFunction progress,
                         char progress_granularity, char immediate_load);
char                save(ImlibImage * im, ImlibProgressFunction progress,
                         char progress_granularity);
void                formats(ImlibLoader * l);

typedef struct tagRGBQUAD {
   unsigned char       rgbBlue;
   unsigned char       rgbGreen;
   unsigned char       rgbRed;
   unsigned char       rgbReserved;
} RGBQUAD;

#define BI_RGB       0
#define BI_RLE8      1
#define BI_RLE4      2
#define BI_BITFIELDS 3

static int
ReadleShort(FILE * file, unsigned short *ret)
{
   unsigned char       b[2];

   if (fread(b, sizeof(unsigned char), 2, file) != 2)
      return 0;

   *ret = (b[1] << 8) | b[0];
   return 1;
}

static int
ReadleLong(FILE * file, unsigned long *ret)
{
   unsigned char       b[4];

   if (fread(b, sizeof(unsigned char), 4, file) != 4)
      return 0;

   *ret = (b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0];
   return 1;
}

static int
WriteleByte(FILE * file, unsigned char val)
{
   int rc;

   rc = fputc ((int) val & 0xff, file);
   if (rc == EOF)
      return 0;

   return 1;
}

static int
WriteleShort(FILE * file, unsigned short val)
{
   int rc;

   rc = fputc ((int) (val & 0xff), file);
   if (rc == EOF)
      return 0;
   rc = fputc ((int) ((val >> 8) & 0xff), file);
   if (rc == EOF)
      return 0;

   return 1;
}

static int
WriteleLong(FILE * file, unsigned long val)
{
   int rc;

   rc = fputc ((int) (val & 0xff), file);
   if (rc == EOF)
      return 0;
   rc = fputc ((int) ((val >> 8) & 0xff), file);
   if (rc == EOF)
      return 0;
   rc = fputc ((int) ((val >> 16) & 0xff), file);
   if (rc == EOF)
      return 0;
   rc = fputc ((int) ((val >> 24) & 0xff), file);
   if (rc == EOF)
      return 0;

   return 1;
}

char
load(ImlibImage * im, ImlibProgressFunction progress,
     char progress_granularity, char immediate_load)
{
   FILE               *f;
   char                pper = 0;
   int                 pl = 0;
   char                type[2];
   unsigned long       size, offset, headSize, comp, imgsize, j, k, l;
   unsigned short      tmpShort, planes, bitcount, ncols, skip;
   unsigned char       tempchar, byte = 0, g, b, r;
   unsigned long       i, w, h;
   unsigned short      x, y;
   DATA32             *ptr, *data_end;
   unsigned char      *buffer_ptr, *buffer, *buffer_end;
   RGBQUAD             rgbQuads[256];
   unsigned long       rmask = 0xff, gmask = 0xff, bmask = 0xff;
   unsigned long       rshift = 0, gshift = 0, bshift = 0;

   if (im->data)
      return 0;
   f = fopen(im->real_file, "rb");
   if (!f)
      return 0;

   /* header */
   {
      struct stat         statbuf;

      if (stat(im->real_file, &statbuf) == -1)
        {
           fclose(f);
           return 0;
        }
      size = statbuf.st_size;

      if (fread(type, 1, 2, f) != 2)
        {
           fclose(f);
           return 0;
        }
      if (strncmp(type, "BM", 2))
        {
           fclose(f);
           return 0;
        }

      fseek(f, 8, SEEK_CUR);
      ReadleLong(f, &offset);
      ReadleLong(f, &headSize);
      if (offset >= size)
        {
           fclose(f);
           return 0;
        }
      if (headSize == 12)
        {
           ReadleShort(f, &tmpShort);
           w = tmpShort;
           ReadleShort(f, &tmpShort);
           h = tmpShort;
           ReadleShort(f, &planes);
           ReadleShort(f, &bitcount);
           imgsize = size - offset;
           comp = BI_RGB;
        }
      else if (headSize == 40)
        {
           ReadleLong(f, &w);
           ReadleLong(f, &h);
           ReadleShort(f, &planes);
           ReadleShort(f, &bitcount);
           ReadleLong(f, &comp);
           ReadleLong(f, &imgsize);
           imgsize = size - offset;

           fseek(f, 16, SEEK_CUR);
        }
      else
        {
           fclose(f);
           return 0;
        }
      
      if ((w < 1) || (h < 1) || (w > 8192) || (h > 8192))
	{
	   fclose(f);
	   return 0;
	}

      if (bitcount < 16)
        {
           ncols = (offset - headSize - 14);
           if (headSize == 12)
             {
                ncols /= 3;
                if (ncols > 256) ncols = 256;
                for (i = 0; i < ncols; i++)
                   fread(&rgbQuads[i], 3, 1, f);
             }
           else
             {
                ncols /= 4;
                if (ncols > 256) ncols = 256;
                fread(rgbQuads, 4, ncols, f);
             }
        }
      else if (bitcount == 16 || bitcount == 32)
        {
           if (comp == BI_BITFIELDS)
             {
                int                 bit;

                ReadleLong(f, &bmask);
                ReadleLong(f, &gmask);
                ReadleLong(f, &rmask);
                for (bit = bitcount - 1; bit >= 0; bit--)
                  {
                     if (bmask & (1 << bit))
                        bshift = bit;
                     if (gmask & (1 << bit))
                        gshift = bit;
                     if (rmask & (1 << bit))
                        rshift = bit;
                  }
             }
           else if (bitcount == 16)
             {
                rmask = 0x7C00;
                gmask = 0x03E0;
                bmask = 0x001F;
                rshift = 10;
                gshift = 5;
                bshift = 0;
             }
           else if (bitcount == 32)
             {
                rmask = 0x00FF0000;
                gmask = 0x0000FF00;
                bmask = 0x000000FF;
                rshift = 16;
                gshift = 8;
                bshift = 0;
             }
        }

      im->w = w;
      im->h = h;
      if (!im->format)
        {
           UNSET_FLAG(im->flags, F_HAS_ALPHA);
           im->format = strdup("bmp");
        }
   }
   if (((!im->data) && (im->loader)) || (immediate_load) || (progress))
     {
        fseek(f, offset, SEEK_SET);
        buffer = malloc(imgsize);
        if (!buffer)
          {
             fclose(f);
             return 0;
          }
        im->data = malloc(w * h * sizeof(DATA32));
        if (!im->data)
          {
             fclose(f);
             free(buffer);
             return 0;
          }

        fread(buffer, imgsize, 1, f);
        fclose(f);
        buffer_ptr = buffer;
        buffer_end = buffer + imgsize;

        data_end = im->data + w * h;
        ptr = im->data + ((h - 1) * w);

        if (bitcount == 1)
          {
             if (comp == BI_RGB)
               {
                  skip = ((((w + 31) / 32) * 32) - w) / 8;
                  for (y = 0; y < h; y++)
                    {
                       for (x = 0; x < w && buffer_ptr < buffer_end; x++)
                         {
                            if ((x & 7) == 0)
                               byte = *(buffer_ptr++);
                            k = (byte >> 7) & 1;
                            *ptr++ = 0xff000000 |
                                (rgbQuads[k].rgbRed << 16) |
                                (rgbQuads[k].rgbGreen << 8) |
                                rgbQuads[k].rgbBlue;
                            byte <<= 1;
                         }
                       buffer_ptr += skip;
                       ptr -= w * 2;
                       if (progress)
                         {
                            char                per;
                            int                 l;

                            per = (char)((100 * y) / im->h);
                            if (((per - pper) >= progress_granularity) ||
                                (y == (im->h - 1)))
                              {
                                 l = y - pl;
                                 if (!progress
                                     (im, per, 0, im->h - y - 1, im->w,
                                      im->h - y + l))
                                   {
                                      free(buffer);
                                      return 2;
                                   }
                                 pper = per;
                                 pl = y;
                              }
                         }
                    }
               }
          }
        if (bitcount == 4)
          {
             if (comp == BI_RLE4)
               {
                  x = 0;
                  y = 0;

                  for (i = 0, g = 1;
                       i < imgsize && g && buffer_ptr < buffer_end; i++)
                    {
                       byte = *(buffer_ptr++);
                       if (byte)
                         {
                            unsigned char       t1, t2;

                            l = byte;
                            byte = *(buffer_ptr++);
                            t1 = byte & 0xF;
                            t2 = (byte >> 4) & 0xF;
                            for (j = 0; j < l; j++)
                              {
                                 k = (j & 1) ? t1 : t2;

                                 if (x >= w)
                                    break;

                                 *ptr++ = 0xff000000 |
                                     (rgbQuads[k].rgbRed << 16) |
                                     (rgbQuads[k].rgbGreen << 8) |
                                     rgbQuads[k].rgbBlue;
                                 x++;
                                 if (ptr > data_end)
                                    ptr = data_end;
                              }
                         }
                       else
                         {
                            byte = *(buffer_ptr++);
                            switch (byte)
                              {
                                case 0:
                                   x = 0;
                                   y++;
                                   ptr = im->data + ((h - y - 1)
                                                     * w * sizeof(DATA32));
                                   if (ptr > data_end)
                                      ptr = data_end;
                                   break;
                                case 1:
                                   g = 0;
                                   break;
                                case 2:
                                   x += *(buffer_ptr++);
                                   y += *(buffer_ptr++);
                                   ptr = im->data + ((h - y - 1) * w *
                                                     sizeof(DATA32)) + x;
                                   if (ptr > data_end)
                                      ptr = data_end;
                                   break;
                                default:
                                   l = byte;
                                   for (j = 0; j < l; j++)
                                     {
                                        char                t1 = '\0', t2 =
                                            '\0';

                                        if ((j & 1) == 0)
                                          {
                                             byte = *(buffer_ptr++);
                                             t1 = byte & 0xF;
                                             t2 = (byte >> 4) & 0xF;
                                          }
                                        k = (j & 1) ? t1 : t2;

                                        if (x >= w)
                                          {
                                             buffer_ptr += (l - j) / 2;
                                             break;
                                          }

                                        *ptr++ = 0xff000000 |
                                            (rgbQuads[k].rgbRed << 16) |
                                            (rgbQuads[k].rgbGreen << 8) |
                                            rgbQuads[k].rgbBlue;
                                        x++;

                                        if (ptr > data_end)
                                           ptr = data_end;

                                     }

                                   if ((l & 3) == 1)
                                     {
                                        tempchar = *(buffer_ptr++);
                                        tempchar = *(buffer_ptr++);
                                     }
                                   else if ((l & 3) == 2)
                                      buffer_ptr++;
                                   break;
                              }
                         }
                       if (progress)
                         {
                            char                per;
                            int                 l;

                            per = (char)((100 * y) / im->h);
                            if (((per - pper) >= progress_granularity) ||
                                (y == (im->h - 1)))
                              {
                                 l = y - pl;
                                 if (!progress
                                     (im, per, 0, im->h - y - 1, im->w,
                                      im->h - y + l))
                                   {
                                      free(buffer);
                                      return 2;
                                   }
                                 pper = per;
                                 pl = y;
                              }
                         }

                    }
               }
             else if (comp == BI_RGB)
               {
                  skip = ((((w + 7) / 8) * 8) - w) / 2;
                  for (y = 0; y < h; y++)
                    {
                       for (x = 0; x < w && buffer_ptr < buffer_end; x++)
                         {
                            if ((x & 1) == 0)
                               byte = *(buffer_ptr++);
                            k = (byte & 0xF0) >> 4;
                            *ptr++ = 0xff000000 |
                                (rgbQuads[k].rgbRed << 16) |
                                (rgbQuads[k].rgbGreen << 8) |
                                rgbQuads[k].rgbBlue;
                            byte <<= 4;
                         }
                       buffer_ptr += skip;
                       ptr -= w * 2;
                       if (progress)
                         {
                            char                per;
                            int                 l;

                            per = (char)((100 * y) / im->h);
                            if (((per - pper) >= progress_granularity) ||
                                (y == (im->h - 1)))
                              {
                                 l = y - pl;
                                 if (!progress
                                     (im, per, 0, im->h - y - 1, im->w,
                                      im->h - y + l))
                                   {
                                      free(buffer);
                                      return 2;
                                   }
                                 pper = per;
                                 pl = y;
                              }
                         }
                    }
               }
          }
        if (bitcount == 8)
          {
             if (comp == BI_RLE8)
               {
                  x = 0;
                  y = 0;
                  for (i = 0, g = 1;
                       i < imgsize && buffer_ptr < buffer_end && g; i++)
                    {
                       byte = *(buffer_ptr++);
                       if (byte)
                         {
                            l = byte;
                            byte = *(buffer_ptr++);
                            for (j = 0; j < l; j++)
                              {
                                 if (x >= w)
                                    break;

                                 *ptr++ = 0xff000000 |
                                     (rgbQuads[byte].rgbRed << 16) |
                                     (rgbQuads[byte].rgbGreen << 8) |
                                     rgbQuads[byte].rgbBlue;

                                 x++;
                                 if (ptr > data_end)
                                    ptr = data_end;
                              }
                         }
                       else
                         {
                            byte = *(buffer_ptr++);
                            switch (byte)
                              {
                                case 0:
                                   x = 0;
                                   y++;
                                   ptr = im->data + ((h - y - 1)
                                                     * w * sizeof(DATA32));
                                   if (ptr > data_end)
                                      ptr = data_end;
                                   break;
                                case 1:
                                   g = 0;
                                   break;
                                case 2:
                                   x += *(buffer_ptr++);
                                   y += *(buffer_ptr++);
                                   ptr = im->data + ((h - y - 1)
                                                     * w *
                                                     sizeof(DATA32)) +
                                       (x * sizeof(DATA32));
                                   if (ptr > data_end)
                                      ptr = data_end;
                                   break;
                                default:
                                   l = byte;
                                   for (j = 0; j < l; j++)
                                     {
                                        byte = *(buffer_ptr++);

                                        if (x >= w)
                                          {
                                             buffer_ptr += l - j;
                                             break;
                                          }

                                        *ptr++ = 0xff000000 |
                                            (rgbQuads[byte].rgbRed << 16) |
                                            (rgbQuads[byte].rgbGreen << 8) |
                                            rgbQuads[byte].rgbBlue;
                                        x++;

                                        if (ptr > data_end)
                                           ptr = data_end;
                                     }
                                   if (l & 1)
                                      buffer_ptr++;
                                   break;
                              }
                         }
                    }
                  if (progress)
                    {
                       char                per;
                       int                 l;

                       per = (char)((100 * y) / im->h);
                       if (((per - pper) >= progress_granularity) ||
                           (y == (im->h - 1)))
                         {
                            l = y - pl;
                            if (!progress
                                (im, per, 0, im->h - y - 1, im->w,
                                 im->h - y + l))
                              {
                                 free(buffer);
                                 return 2;
                              }
                            pper = per;
                            pl = y;
                         }
                    }
               }
             else if (comp == BI_RGB)
               {
                  skip = (((w + 3) / 4) * 4) - w;
                  for (y = 0; y < h; y++)
                    {
                       for (x = 0; x < w && buffer_ptr < buffer_end; x++)
                         {
                            byte = *(buffer_ptr++);
                            *ptr++ = 0xff000000 |
                                (rgbQuads[byte].rgbRed << 16) |
                                (rgbQuads[byte].rgbGreen << 8) |
                                rgbQuads[byte].rgbBlue;
                         }
                       ptr -= w * 2;
                       buffer_ptr += skip;
                       if (progress)
                         {
                            char                per;
                            int                 l;

                            per = (char)((100 * y) / im->h);
                            if (((per - pper) >= progress_granularity) ||
                                (y == (im->h - 1)))
                              {
                                 l = y - pl;
                                 if (!progress
                                     (im, per, 0, im->h - y - 1, im->w,
                                      im->h - y + l))
                                   {
                                      free(buffer);
                                      return 2;
                                   }
                                 pper = per;
                                 pl = y;
                              }
                         }
                    }
               }

          }
        else if (bitcount == 16)
          {
             skip = (((w * 16 + 31) / 32) * 4) - (w * 2);
             for (y = 0; y < h; y++)
               {
                  for (x = 0; x < w && buffer_ptr < buffer_end; x++)
                    {
                       r = ((unsigned short)(*buffer_ptr) & rmask) >> rshift;
                       g = ((unsigned short)(*buffer_ptr) & gmask) >> gshift;
                       b = ((unsigned short)(*(buffer_ptr)) & bmask) >>
                           bshift;
                       *ptr++ = 0xff000000 | (r << 16) | (g << 8) | b;
		       buffer_ptr += 2;
                    }
                  ptr -= w * 2;
                  buffer_ptr += skip;
                  if (progress)
                    {
                       char                per;
                       int                 l;

                       per = (char)((100 * y) / im->h);
                       if (((per - pper) >= progress_granularity) ||
                           (y == (im->h - 1)))
                         {
                            l = y - pl;
                            if (!progress
                                (im, per, 0, im->h - y - 1, im->w,
                                 im->h - y + l))
                              {
                                 free(buffer);
                                 return 2;
                              }
                            pper = per;
                            pl = y;
                         }
                    }
               }
          }
        else if (bitcount == 24)
          {
             skip = (4 - ((w * 3) % 4)) & 3;
             for (y = 0; y < h; y++)
               {
                  for (x = 0; x < w && buffer_ptr < buffer_end; x++)
                    {
                       b = *(buffer_ptr++);
                       g = *(buffer_ptr++);
                       r = *(buffer_ptr++);
                       *ptr++ = 0xff000000 | (r << 16) | (g << 8) | b;
                    }
                  ptr -= w * 2;
                  buffer_ptr += skip;
                  if (progress)
                    {
                       char                per;
                       int                 l;

                       per = (char)((100 * y) / im->h);
                       if (((per - pper) >= progress_granularity) ||
                           (y == (im->h - 1)))
                         {
                            l = y - pl;
                            if (!progress
                                (im, per, 0, im->h - y - 1, im->w,
                                 im->h - y + l))
                              {
                                 free(buffer);
                                 return 2;
                              }
                            pper = per;
                            pl = y;
                         }
                    }
               }
          }
        else if (bitcount == 32)
          {
             skip = (((w * 32 + 31) / 32) * 4) - (w * 4);
             for (y = 0; y < h; y++)
               {
                  for (x = 0; x < w && buffer_ptr < buffer_end; x++)
                    {
                       r = ((unsigned long)(*buffer_ptr) & rmask) >> rshift;
                       g = ((unsigned long)(*buffer_ptr) & gmask) >> gshift;
                       b = ((unsigned long)(*buffer_ptr) & bmask) >> bshift;
                       *ptr++ = 0xff000000 | (r << 16) | (g << 8) | b;
		       buffer_ptr+=4;
                    }
                  ptr -= w * 2;
                  buffer_ptr += skip;
                  if (progress)
                    {
                       char                per;
                       int                 l;

                       per = (char)((100 * y) / im->h);
                       if (((per - pper) >= progress_granularity) ||
                           (y == (im->h - 1)))
                         {
                            l = y - pl;
                            if (!progress
                                (im, per, 0, im->h - y - 1, im->w,
                                 im->h - y + l))
                              {
                                 free(buffer);
                                 return 2;
                              }
                            pper = per;
                            pl = y;
                         }
                    }
               }
          }
        free(buffer);
     }
   return 1;
}

char
save(ImlibImage * im, ImlibProgressFunction progress, char progress_granularity)
{
   FILE               *f;
   Imlib_Color         pixel_color;
   unsigned long       i, j, pad, size;

   if (!im->data)
      return 0;

   f = fopen(im->real_file, "wb");
   if (!f)
      return 0;

   /* calculate number of bytes to pad on end of each row */
   pad = (4 - ((im->w * 3) % 4)) & 0x03;

   /* write BMP file header */
   WriteleShort(f, 0x4d42);   /* prefix */
   WriteleLong(f, 54 + 3 * im->w * im->h);  /* filesize */
   WriteleShort(f, 0x0000);   /* reserved #1 */
   WriteleShort(f, 0x0000);   /* reserved #2 */
   WriteleLong(f, 54);        /* offset to image data */

   /* write BMP bitmap header */
   WriteleLong(f, 40);   /* 40-byte header */
   WriteleLong(f, im->w);
   WriteleLong(f, im->h);
   WriteleShort(f, 1);   /* one plane      */
   WriteleShort(f, 24);  /* bits per pixel */
   WriteleLong(f, 0);    /* no compression */
   WriteleLong(f, 3 * im->w * im->h);
   for (i = 0; i < 4; i++)
     WriteleLong(f, 0x0000);  /* pad to end of header */

   /* write actual BMP data */
   for (i = 0; i < im->h; i++)
      {
         for (j = 0; j < im->w; j++)
            {
               imlib_image_query_pixel (j, im->h - i - 1, &pixel_color);
               WriteleByte(f, pixel_color.blue);
               WriteleByte(f, pixel_color.green);
               WriteleByte(f, pixel_color.red);
            }
         for (j = 0; j < pad; j++)
            WriteleByte(f, 0);
      }

   fclose(f);
   return 1;
}

void
formats(ImlibLoader * l)
{
   char               *list_formats[] = { "bmp" };

   {
      int                 i;

      l->num_formats = (sizeof(list_formats) / sizeof(char *));
      l->formats = malloc(sizeof(char *) * l->num_formats);
      for (i = 0; i < l->num_formats; i++)
         l->formats[i] = strdup(list_formats[i]);
   }
}
