/*------------------------------------------------------------------------------
 * Reads regular Amiga IFF ILBM files. Does not handle sliced HAM, etc.
 *
 * Author:  John Bickers <jbickers@ihug.co.nz>
 * Since:   2004-08-21
 * Version: 2004-08-23
 *------------------------------------------------------------------------------*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common.h"
#include "image.h"

char                load(ImlibImage * im, ImlibProgressFunction progress,
                         char progress_granularity, char immediate_load);
char                save(ImlibImage * im, ImlibProgressFunction progress,
                         char progress_granularity);
void                formats(ImlibLoader * l);

#define L2RLONG(a) ((((long)((a)[0]) & 0xff) << 24) + (((long)((a)[1]) & 0xff) << 16) + (((long)((a)[2]) & 0xff) << 8) + ((long)((a)[3]) & 0xff))
#define L2RWORD(a) ((((long)((a)[0]) & 0xff) << 8) + ((long)((a)[1]) & 0xff))

typedef struct CHUNK {
   long                size;
   unsigned char      *data;
} CHUNK;

typedef struct BODY {
   long                size;
   unsigned char      *data;
   int                 offset;
   int                 count;
   int                 rle;
   int                 rletype;
} BODY;

static int          loadchunks(char *name, CHUNK * bmhd, CHUNK * camg,
                               CHUNK * cmap, BODY * body);
static unsigned char bodybyte(BODY *);
static void         deplane(DATA32 * row, int w, CHUNK * bmhd, CHUNK * cmap,
                            unsigned char *plane[], int depth, int mask,
                            int ham, int hbrite);

/*------------------------------------------------------------------------------
 * Reads the given chunks out of a file, returns 0 if the file had a problem.
 *
 * Format FORMsizeILBMBMHDsize....CAMGsize....CMAPsize....BODYsize....
 *------------------------------------------------------------------------------*/
static int
loadchunks(char *name, CHUNK * bmhd, CHUNK * camg, CHUNK * cmap, BODY * body)
{
   FILE               *f;
   size_t              s;
   long                formsize, pos, z;
   int                 ok, seek;
   char                buf[12];

   if (bmhd)
     {
        bmhd->size = 0;
        bmhd->data = NULL;
     }
   if (camg)
     {
        camg->size = 0;
        camg->data = NULL;
     }
   if (cmap)
     {
        cmap->size = 0;
        cmap->data = NULL;
     }
   if (body)
     {
        body->size = 0;
        body->data = NULL;
     }

   ok = 0;

   f = fopen(name, "rb");
   if (f)
     {
        s = fread(buf, 1, 12, f);
        if (s == 12 && !memcmp(buf, "FORM", 4) && !memcmp(buf + 8, "ILBM", 4))
          {
             formsize = L2RLONG(buf + 4);

             while (1)
               {
                  pos = ftell(f);
                  if (pos < 0 || pos >= formsize + 8)
                     break;     /* Error or FORM data is finished. */
                  seek = 1;

                  s = fread(buf, 1, 8, f);
                  if (s != 8)
                     break;     /* Error or short file. */

                  z = L2RLONG(buf + 4);
                  if (z < 0)
                     break;     /* Corrupt file. */

                  if (!memcmp(buf, "BMHD", 4) && bmhd && !bmhd->data)
                    {
                       bmhd->size = z;
                       bmhd->data = malloc(bmhd->size);
                       if (!bmhd->data)
                          break;

                       s = fread(bmhd->data, 1, bmhd->size, f);
                       if (s != bmhd->size)
                          break;        /* Error or short file. */

                       if (!cmap && !camg && !body)
                         {
                            ok = 1;
                            break;
                         }

                       seek = 0;
                    }

                  if (!memcmp(buf, "CAMG", 4) && camg && !camg->data)
                    {
                       camg->size = z;
                       camg->data = malloc(camg->size);
                       if (!camg->data)
                          break;

                       s = fread(camg->data, 1, camg->size, f);
                       if (s != camg->size)
                          break;

                       seek = 0;
                    }

                  if (!memcmp(buf, "CMAP", 4) && cmap && !cmap->data)
                    {
                       cmap->size = z;
                       cmap->data = malloc(cmap->size);
                       if (!cmap->data)
                          break;

                       s = fread(cmap->data, 1, cmap->size, f);
                       if (s != cmap->size)
                          break;

                       seek = 0;
                    }

                  if (!memcmp(buf, "BODY", 4) && body && !body->data)
                    {
                       body->size = z;
                       body->data = malloc(body->size);
                       if (!body->data)
                          break;

                       s = fread(body->data, 1, body->size, f);
                       if (s != body->size)
                          break;

                       if (bmhd && bmhd->data)
                         {      /* BMHD must be before BODY. */
                            ok = 1;
                            break;
                         }

                       seek = 0;
                    }

                  if (pos + 8 + z >= formsize + 8)
                     break;     /* This was last chunk. */

                  if (seek && fseek(f, z, SEEK_CUR) != 0)
                     break;
               }
          }
        fclose(f);
     }

   if (!ok)
     {
        if (bmhd && bmhd->data)
           free(bmhd->data);
        if (camg && camg->data)
           free(camg->data);
        if (cmap && cmap->data)
           free(cmap->data);
        if (body && body->data)
           free(body->data);
     }

   return ok;
}

/*------------------------------------------------------------------------------
 * Returns a byte of data from a BODY chunk.
 *
 * RLE compression depends on a count byte, followed by data bytes.
 *
 * 0x80 means skip.
 * 0xff to 0x81 means repeat one data byte (256 - count) + 1 times.
 * 0x00 to 0x7f means copy count + 1 data bytes.
 *------------------------------------------------------------------------------*/
static unsigned char
bodybyte(BODY * body)
{
   unsigned char       b;

   if (body->offset >= body->size)
      return 0;

   if (body->rle)
     {
        if (body->count == 0)
          {
             b = body->data[body->offset++];
             while (b == 0x80 && body->offset < body->size)
                b = body->data[body->offset++];

             body->rletype = (b & 0x80);
             if (body->rletype)
                body->count = (0x100 - b) + 1;
             else
                body->count = (b & 0x7f) + 1;

             if (body->offset >= body->size)
                return 0;
          }

        body->count--;
        if (body->rletype)
          {
             if (body->count == 0)
                return body->data[body->offset++];
             return body->data[body->offset];
          }

        return body->data[body->offset++];
     }

   return body->data[body->offset++];
}

/*------------------------------------------------------------------------------
 * Deplanes and converts an array of bitplanes to a single scanline of DATA32
 * (unsigned int) values. DATA32 is ARGB.
 *------------------------------------------------------------------------------*/
static void
deplane(DATA32 * row, int w, CHUNK * bmhd, CHUNK * cmap, unsigned char *plane[],
        int depth, int mask, int ham, int hbrite)
{
   unsigned long       l;
   int                 i, o, x;
   unsigned char       bit, r, g, b, a, v, h;

   bit = 0x80;
   o = 0;
   for (x = 0; x < w; x++)
     {
        l = 0;
        for (i = depth - 1; i >= 0; i--)
          {
             l = l << 1;
             if (plane[i][o] & bit)
                l = l | 1;
          }
        a = (mask == 0 || (mask == 1 && (plane[depth][o] & bit))
             || mask == 2) ? 0xff : 0x00;

        if (depth == 32)
          {
             a = (l >> 24) & 0xff;
             b = (l >> 16) & 0xff;
             g = (l >> 8) & 0xff;
             r = l & 0xff;
          }
        else if (depth == 24)
          {
             b = (l >> 16) & 0xff;
             g = (l >> 8) & 0xff;
             r = l & 0xff;
          }
        else if (ham)
          {
             v = l & ((1 << (depth - 2)) - 1);
             h = (l & ~v) >> (depth - 2);

             if (x == 0 || h == 0x00)
               {
                  r = cmap->data[v * 3];
                  g = cmap->data[v * 3 + 1];
                  b = cmap->data[v * 3 + 2];
               }
             else if (h == 0x01)
               {
                  b = v << (8 - (depth - 2));
               }
             else if (h == 0x02)
               {
                  r = v << (8 - (depth - 2));
               }
             else if (h == 0x03)
               {
                  g = v << (8 - (depth - 2));
               }
          }
        else if (hbrite)
          {
             v = l & ((1 << (depth - 1)) - 1);
             h = (l & ~v) >> (depth - 1);

             r = cmap->data[v * 3];
             g = cmap->data[v * 3 + 1];
             b = cmap->data[v * 3 + 2];

             if (h)
               {
                  r = r >> 1;
                  g = g >> 1;
                  b = b >> 1;
               }

             if (mask == 2 && v == L2RWORD(bmhd->data + 12))
                a = 0x00;
          }
        else
          {
             v = l & 0xff;
             if ((v + 1) * 3 <= cmap->size)
               {
                  r = cmap->data[v * 3];
                  g = cmap->data[v * 3 + 1];
                  b = cmap->data[v * 3 + 2];
               }
             else
                r = g = b = 0;

             if (mask == 2 && v == L2RWORD(bmhd->data + 12))
                a = 0x00;
          }

        row[x] =
            ((unsigned long)a << 24) | ((unsigned long)r << 16) |
            ((unsigned long)g << 8) | (unsigned long)b;

        bit = bit >> 1;
        if (bit == 0)
          {
             o++;
             bit = 0x80;
          }
     }
}

/*------------------------------------------------------------------------------
 * Loads an image. If im->loader is non-zero, or immediate_load is non-zero, or
 * progress is non-zero, then the file is fully loaded, otherwise only the width
 * and height are read.
 *
 * Uses BMHD, CAMG, CMAP, and BODY chunks. Ignores others, e.g. DPI_ or ANNO.
 *------------------------------------------------------------------------------*/
char
load(ImlibImage * im, ImlibProgressFunction progress, char progress_granularity,
     char immediate_load)
{
   int                 cancel, full, ham, hbrite, i, j, k, mask, n, rle, ok,
       planes, y, z, prevy;
   unsigned char      *plane[40];
   char                per, pper;
   BODY                body;
   CHUNK               bmhd, camg, cmap;

  /*----------
   * Do nothing if the data is already loaded.
   *----------*/
   if (im->data)
      return 0;

  /*----------
   * Load the chunk(s) we're interested in. If full is not true, then
   * we only want the image size and format.
   *----------*/
   full = (im->loader || immediate_load || progress);
   if (full)
      ok = loadchunks(im->real_file, &bmhd, &camg, &cmap, &body);
   else
      ok = loadchunks(im->real_file, &bmhd, NULL, NULL, NULL);
   if (!ok)
      return 0;

  /*----------
   * Use and check header.
   *----------*/
   ok = 0;
   if (bmhd.size >= 20)
     {
        ok = 1;

        im->w = L2RWORD(bmhd.data);
        im->h = L2RWORD(bmhd.data + 2);

        planes = bmhd.data[8];
        if (planes < 1 || (planes > 8 && planes != 24 && planes != 32))
           ok = 0;              /* Only 1 to 8, 24, or 32 planes. */

        if (bmhd.data[10] > 1)
           ok = 0;              /* Only NONE or RLE compression. */
        rle = bmhd.data[10];

        mask = bmhd.data[9];

        if (mask || planes == 32)
           SET_FLAG(im->flags, F_HAS_ALPHA);
        else
           UNSET_FLAG(im->flags, F_HAS_ALPHA);

        if (!im->format)
           im->format = strdup("lbm");

        ham = 0;
        hbrite = 0;
        if (planes <= 8)
          {
             if (camg.size == 4)
               {
                  if (camg.data[2] & 0x08)
                     ham = 1;
                  if (camg.data[3] & 0x80)
                     hbrite = 1;

                  if (ham && full
                      && (planes < 6 || cmap.size < 3 * (1 << (planes - 2))))
                     ham = 0;
                  if (hbrite && full
                      && (planes < 5 || cmap.size < 3 * (1 << (planes - 1))))
                     hbrite = 0;
               }
             else
               {
                  if (planes == 6 && full && cmap.size >= 3 * 16)
                     ham = 1;
                  if (full && !ham && planes > 1
                      && cmap.size == 3 * (1 << (planes - 1)))
                     hbrite = 1;
               }
          }
     }
   if (!full || !ok)
     {
        free(bmhd.data);
        if (full && camg.data)
           free(camg.data);
        if (full && cmap.data)
           free(cmap.data);
        if (full && body.data)
           free(body.data);
        return ok;
     }

  /*----------
   * The source data is planar. Each plane is an even number of bytes wide. If
   * masking type is 1, there is an extra plane that defines the mask. Scanlines
   * from each plane are interleaved, from top to bottom. The first plane is the
   * 0 bit.
   *----------*/
   ok = 0;
   cancel = 0;
   plane[0] = NULL;

   im->data = malloc(im->w * im->h * sizeof(DATA32));
   if (im->data)
     {
        body.offset = 0;
        body.count = 0;
        body.rle = rle;
        body.rletype = 0;

        n = planes;
        if (mask == 1)
           n++;

        plane[0] = malloc(((im->w + 15) / 16) * 2 * n);
        for (i = 1; i < n; i++)
           plane[i] = plane[i - 1] + ((im->w + 15) / 16) * 2;

        z = ((im->w + 15) / 16) * 2 * n;

        pper = 0;
        prevy = 0;

        for (y = 0; y < im->h; y++)
          {
             for (i = 0; i < z; i++)
                plane[0][i] = bodybyte(&body);

             deplane(im->data + im->w * y, im->w, &bmhd, &cmap, plane, planes,
                     mask, ham, hbrite);

             if (progress)
               {
                  per = (char)((100 * y) / im->h);
                  if (per - pper >= progress_granularity || y == im->h - 1)
                    {
                       if (!progress(im, per, 0, prevy, im->w, y + 1))
                         {
                            cancel = 1;
                            break;
                         }
                       pper = per;
                       prevy = y;
                    }
               }
          }

        ok = !cancel;
     }

  /*----------
   * We either had a successful decode, the user cancelled, or we couldn't get
   * the memory for im->data.
   *----------*/
   if (!ok)
     {
        if (im->data)
           free(im->data);
        im->data = NULL;
     }

   if (plane[0])
      free(plane[0]);

   free(bmhd.data);
   if (camg.data)
      free(camg.data);
   if (cmap.data)
      free(cmap.data);
   if (body.data)
      free(body.data);

   return (cancel) ? 2 : ok;
}

/*------------------------------------------------------------------------------
 * Does nothing. Perhaps save only in 24-bit format?
 *------------------------------------------------------------------------------*/
char
save(ImlibImage * im, ImlibProgressFunction progress, char progress_granularity)
{
   return 0;
}

/*------------------------------------------------------------------------------
 * Identifies the file extensions this loader handles. Standard code from other
 * loaders.
 *------------------------------------------------------------------------------*/
void
formats(ImlibLoader * l)
{
   char               *list_formats[] = { "iff", "ilbm", "lbm" };
   int                 i;

   l->num_formats = sizeof(list_formats) / sizeof(list_formats[0]);
   l->formats = malloc(l->num_formats * sizeof(list_formats[0]));
   for (i = 0; i < l->num_formats; i++)
      l->formats[i] = strdup(list_formats[i]);
}
