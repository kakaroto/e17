#include "common.h"
#include <math.h>
#include "colormod.h"
#include "image.h"
#include "scale.h"
#include "blend.h"
#include "updates.h"
#include "rgbadraw.h"
#include "Imlib2.h"

#define round(a) floor(a+0.5)

static void         span(ImlibImage * im, int y, int x1, int y1,
                         DATA8 r, DATA8 g, DATA8 b, DATA8 a, ImlibOp op);

void
__imlib_FlipImageHoriz(ImlibImage * im)
{
   DATA32             *p1, *p2, tmp;
   int                 x, y;

   for (y = 0; y < im->h; y++)
     {
        p1 = im->data + (y * im->w);
        p2 = im->data + ((y + 1) * im->w) - 1;
        for (x = 0; x < (im->w >> 1); x++)
          {
             tmp = *p1;
             *p1 = *p2;
             *p2 = tmp;
             p1++;
             p2--;
          }
     }
   x = im->border.left;
   im->border.left = im->border.right;
   im->border.right = x;
}

void
__imlib_FlipImageVert(ImlibImage * im)
{
   DATA32             *p1, *p2, tmp;
   int                 x, y;

   for (y = 0; y < (im->h >> 1); y++)
     {
        p1 = im->data + (y * im->w);
        p2 = im->data + ((im->h - 1 - y) * im->w);
        for (x = 0; x < im->w; x++)
          {
             tmp = *p1;
             *p1 = *p2;
             *p2 = tmp;
             p1++;
             p2++;
          }
     }
   x = im->border.top;
   im->border.top = im->border.bottom;
   im->border.bottom = x;
}

void
__imlib_FlipImageBoth(ImlibImage * im)
{
   DATA32             *p1, *p2, tmp;
   int                 x;

   p1 = im->data;
   p2 = im->data + (im->h * im->w) - 1;
   for (x = (im->w * im->h) / 2; --x >= 0;)
     {
        tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
     }
   x = im->border.top;
   im->border.top = im->border.bottom;
   im->border.bottom = x;
   x = im->border.left;
   im->border.left = im->border.right;
   im->border.right = x;
}

/*\ Directions (source is right/down):
|*| 0 = down/right (flip over ul-dr diagonal)
|*| 1 = down/left  (rotate 90 degrees clockwise)
|*| 2 = up/right   (rotate 90 degrees counterclockwise)
|*| 3 = up/left    (flip over ur-ll diagonal)
\*/
void
__imlib_FlipImageDiagonal(ImlibImage * im, int direction)
{
   DATA32             *data, *to, *from;
   int                 x, y, w, hw, tmp;

   data = malloc(im->w * im->h * sizeof(DATA32));
   from = im->data;
   w = im->h;
   im->h = im->w;
   im->w = w;
   hw = w * im->h;
   switch (direction)
     {
       default:
       case 0:                 /*\ DOWN_RIGHT \ */
          tmp = im->border.top;
          im->border.top = im->border.left;
          im->border.left = tmp;
          tmp = im->border.bottom;
          im->border.bottom = im->border.right;
          im->border.right = tmp;
          to = data;
          hw = -hw + 1;
          break;
       case 1:                 /*\ DOWN_LEFT \ */
          tmp = im->border.top;
          im->border.top = im->border.left;
          im->border.left = im->border.bottom;
          im->border.bottom = im->border.right;
          im->border.right = tmp;
          to = data + w - 1;
          hw = -hw - 1;
          break;
       case 2:                 /*\ UP_RIGHT \ */
          tmp = im->border.top;
          im->border.top = im->border.right;
          im->border.right = im->border.bottom;
          im->border.bottom = im->border.left;
          im->border.left = tmp;
          to = data + hw - w;
          w = -w;
          hw = hw + 1;
          break;
       case 3:                 /*\ UP_LEFT \ */
          tmp = im->border.top;
          im->border.top = im->border.right;
          im->border.right = tmp;
          tmp = im->border.bottom;
          im->border.bottom = im->border.left;
          im->border.left = tmp;
          to = data + hw - 1;
          w = -w;
          hw = hw - 1;
          break;
     }
   from = im->data;
   for (x = im->w; --x >= 0;)
     {
        for (y = im->h; --y >= 0;)
          {
             *to = *from;
             from++;
             to += w;
          }
        to += hw;
     }
   free(im->data);
   im->data = data;
}

void
__imlib_BlurImage(ImlibImage * im, int rad)
{
   DATA32             *p1, *p2, *data;
   int                 x, y, mx, my, mw, mh, mt, xx, yy;
   int                 a, r, g, b;
   int                *as, *rs, *gs, *bs;

   if (rad < 1)
      return;
   data = malloc(im->w * im->h * sizeof(DATA32));
   as = malloc(sizeof(int) * im->w);
   rs = malloc(sizeof(int) * im->w);
   gs = malloc(sizeof(int) * im->w);
   bs = malloc(sizeof(int) * im->w);

   for (y = 0; y < im->h; y++)
     {
        my = y - rad;
        mh = (rad << 1) + 1;
        if (my < 0)
          {
             mh += my;
             my = 0;
          }
        if ((my + mh) > im->h)
           mh = im->h - my;

        p1 = data + (y * im->w);
        memset(as, 0, im->w * sizeof(int));
        memset(rs, 0, im->w * sizeof(int));
        memset(gs, 0, im->w * sizeof(int));
        memset(bs, 0, im->w * sizeof(int));

        for (yy = 0; yy < mh; yy++)
          {
             p2 = im->data + ((yy + my) * im->w);
             for (x = 0; x < im->w; x++)
               {
                  as[x] += (*p2 >> 24) & 0xff;
                  rs[x] += (*p2 >> 16) & 0xff;
                  gs[x] += (*p2 >> 8) & 0xff;
                  bs[x] += *p2 & 0xff;
                  p2++;
               }
          }
        if (im->w > ((rad << 1) + 1))
          {
             for (x = 0; x < im->w; x++)
               {
                  a = 0;
                  r = 0;
                  g = 0;
                  b = 0;
                  mx = x - rad;
                  mw = (rad << 1) + 1;
                  if (mx < 0)
                    {
                       mw += mx;
                       mx = 0;
                    }
                  if ((mx + mw) > im->w)
                     mw = im->w - mx;
                  mt = mw * mh;
                  for (xx = mx; xx < (mw + mx); xx++)
                    {
                       a += as[xx];
                       r += rs[xx];
                       g += gs[xx];
                       b += bs[xx];
                    }
                  a = a / mt;
                  r = r / mt;
                  g = g / mt;
                  b = b / mt;
                  *p1 = (a << 24) | (r << 16) | (g << 8) | b;
                  p1++;
               }
          }
        else
          {
          }
     }
   free(as);
   free(rs);
   free(gs);
   free(bs);
   free(im->data);
   im->data = data;
}

void
__imlib_SharpenImage(ImlibImage * im, int rad)
{
   DATA32             *data, *p1, *p2;
   int                 a, r, g, b, x, y;

   data = malloc(im->w * im->h * sizeof(DATA32));
   if (rad == 0)
      return;
   else
     {
        int                 mul, mul2, tot;

        mul = (rad * 4) + 1;
        mul2 = rad;
        tot = mul - (mul2 * 4);
        for (y = 1; y < (im->h - 1); y++)
          {
             p1 = im->data + 1 + (y * im->w);
             p2 = data + 1 + (y * im->w);
             for (x = 1; x < (im->w - 1); x++)
               {
                  b = (int)((p1[0]) & 0xff) * 5;
                  g = (int)((p1[0] >> 8) & 0xff) * 5;
                  r = (int)((p1[0] >> 16) & 0xff) * 5;
                  a = (int)((p1[0] >> 24) & 0xff) * 5;
                  b -= (int)((p1[-1]) & 0xff);
                  g -= (int)((p1[-1] >> 8) & 0xff);
                  r -= (int)((p1[-1] >> 16) & 0xff);
                  a -= (int)((p1[-1] >> 24) & 0xff);
                  b -= (int)((p1[1]) & 0xff);
                  g -= (int)((p1[1] >> 8) & 0xff);
                  r -= (int)((p1[1] >> 16) & 0xff);
                  a -= (int)((p1[1] >> 24) & 0xff);
                  b -= (int)((p1[-im->w]) & 0xff);
                  g -= (int)((p1[-im->w] >> 8) & 0xff);
                  r -= (int)((p1[-im->w] >> 16) & 0xff);
                  a -= (int)((p1[-im->w] >> 24) & 0xff);
                  b -= (int)((p1[im->w]) & 0xff);
                  g -= (int)((p1[im->w] >> 8) & 0xff);
                  r -= (int)((p1[im->w] >> 16) & 0xff);
                  a -= (int)((p1[im->w] >> 24) & 0xff);

                  a = (a & ((~a) >> 16));
                  a = ((a | ((a & 256) - ((a & 256) >> 8))));
                  r = (r & ((~r) >> 16));
                  r = ((r | ((r & 256) - ((r & 256) >> 8))));
                  g = (g & ((~g) >> 16));
                  g = ((g | ((g & 256) - ((g & 256) >> 8))));
                  b = (b & ((~b) >> 16));
                  b = ((b | ((b & 256) - ((b & 256) >> 8))));

                  *p2 = (a << 24) | (r << 16) | (g << 8) | b;
                  p2++;
                  p1++;
               }
          }
     }
   free(im->data);
   im->data = data;
}

void
__imlib_TileImageHoriz(ImlibImage * im)
{
   DATA32             *p1, *p2, *p3, *p, *data;
   int                 x, y, per, tmp, na, nr, ng, nb, mix, a, r, g, b, aa, rr,
       gg, bb;

   data = malloc(im->w * im->h * sizeof(DATA32));
   p1 = im->data;
   p = data;
   for (y = 0; y < im->h; y++)
     {
        p2 = p1 + (im->w >> 1);
        p3 = p1;
        per = (im->w >> 1);
        for (x = 0; x < (im->w >> 1); x++)
          {
             mix = (x * 255) / per;
             b = (*p1) & 0xff;
             g = (*p1 >> 8) & 0xff;
             r = (*p1 >> 16) & 0xff;
             a = (*p1 >> 24) & 0xff;

             bb = (*p2) & 0xff;
             gg = (*p2 >> 8) & 0xff;
             rr = (*p2 >> 16) & 0xff;
             aa = (*p2 >> 24) & 0xff;

             tmp = (r - rr) * mix;
             nr = rr + ((tmp + (tmp >> 8) + 0x80) >> 8);
             tmp = (g - gg) * mix;
             ng = gg + ((tmp + (tmp >> 8) + 0x80) >> 8);
             tmp = (b - bb) * mix;
             nb = bb + ((tmp + (tmp >> 8) + 0x80) >> 8);
             tmp = (a - aa) * mix;
             na = aa + ((tmp + (tmp >> 8) + 0x80) >> 8);
             *p = (na << 24) | (nr << 16) | (ng << 8) | nb;
             p++;
             p1++;
             p2++;
          }
        p2 = p3;
        per = (im->w - (im->w >> 1));
        for (; x < im->w; x++)
          {
             mix = ((im->w - 1 - x) * 255) / per;
             b = (*p1) & 0xff;
             g = (*p1 >> 8) & 0xff;
             r = (*p1 >> 16) & 0xff;
             a = (*p1 >> 24) & 0xff;

             bb = (*p2) & 0xff;
             gg = (*p2 >> 8) & 0xff;
             rr = (*p2 >> 16) & 0xff;
             aa = (*p2 >> 24) & 0xff;

             tmp = (r - rr) * mix;
             nr = rr + ((tmp + (tmp >> 8) + 0x80) >> 8);
             tmp = (g - gg) * mix;
             ng = gg + ((tmp + (tmp >> 8) + 0x80) >> 8);
             tmp = (b - bb) * mix;
             nb = bb + ((tmp + (tmp >> 8) + 0x80) >> 8);
             tmp = (a - aa) * mix;
             na = aa + ((tmp + (tmp >> 8) + 0x80) >> 8);
             *p = (na << 24) | (nr << 16) | (ng << 8) | nb;
             p++;
             p1++;
             p2++;
          }
     }
   free(im->data);
   im->data = data;
}

void
__imlib_TileImageVert(ImlibImage * im)
{
   DATA32             *p1, *p2, *p, *data;
   int                 x, y, tmp, na, nr, ng, nb, mix, a, r, g, b, aa, rr, gg,
       bb;

   data = malloc(im->w * im->h * sizeof(DATA32));
   p = data;
   for (y = 0; y < im->h; y++)
     {
        p1 = im->data + (y * im->w);
        if (y < (im->h >> 1))
          {
             p2 = im->data + ((y + (im->h >> 1)) * im->w);
             mix = (y * 255) / (im->h >> 1);
          }
        else
          {
             p2 = im->data + ((y - (im->h >> 1)) * im->w);
             mix = ((im->h - y) * 255) / (im->h - (im->h >> 1));
          }
        for (x = 0; x < im->w; x++)
          {
             b = (*p1) & 0xff;
             g = (*p1 >> 8) & 0xff;
             r = (*p1 >> 16) & 0xff;
             a = (*p1 >> 24) & 0xff;

             bb = (*p2) & 0xff;
             gg = (*p2 >> 8) & 0xff;
             rr = (*p2 >> 16) & 0xff;
             aa = (*p2 >> 24) & 0xff;

             tmp = (r - rr) * mix;
             nr = rr + ((tmp + (tmp >> 8) + 0x80) >> 8);
             tmp = (g - gg) * mix;
             ng = gg + ((tmp + (tmp >> 8) + 0x80) >> 8);
             tmp = (b - bb) * mix;
             nb = bb + ((tmp + (tmp >> 8) + 0x80) >> 8);
             tmp = (a - aa) * mix;
             na = aa + ((tmp + (tmp >> 8) + 0x80) >> 8);
             *p = (na << 24) | (nr << 16) | (ng << 8) | nb;
             p++;
             p1++;
             p2++;
          }
     }
   free(im->data);
   im->data = data;
}

ImlibUpdate        *
__imlib_draw_line(ImlibImage * im, int x1, int y1, int x2, int y2, DATA8 r,
                  DATA8 g, DATA8 b, DATA8 a, ImlibOp op, char make_updates)
{
   int                 x, y, dx, dy, yy, xx, am, tmp;
   DATA32             *p;
   DATA8               aaa;

   /* clip to top edge */
   if ((y1 < 0) && (y2 < 0))
      return NULL;
   if (y1 < 0)
     {
        x1 += (y1 * (x1 - x2)) / (y2 - y1);
        y1 = 0;
     }
   if (y2 < 0)
     {
        x2 += (y2 * (x1 - x2)) / (y2 - y1);
        y2 = 0;
     }
   /* clip to bottom edge */
   if ((y1 >= im->h) && (y2 >= im->h))
      return NULL;
   if (y1 >= im->h)
     {
        x1 -= ((im->h - y1) * (x1 - x2)) / (y2 - y1);
        y1 = im->h - 1;
     }
   if (y2 >= im->h)
     {
        x2 -= ((im->h - y2) * (x1 - x2)) / (y2 - y1);
        y2 = im->h - 1;
     }
   /* clip to left edge */
   if ((x1 < 0) && (x2 < 0))
      return NULL;
   if (x1 < 0)
     {
        y1 += (x1 * (y1 - y2)) / (x2 - x1);
        x1 = 0;
     }
   if (x2 < 0)
     {
        y2 += (x2 * (y1 - y2)) / (x2 - x1);
        x2 = 0;
     }
   /* clip to right edge */
   if ((x1 >= im->w) && (x2 >= im->w))
      return NULL;
   if (x1 >= im->w)
     {
        y1 -= ((im->w - x1) * (y1 - y2)) / (x2 - x1);
        x1 = im->w - 1;
     }
   if (x2 >= im->w)
     {
        y2 -= ((im->w - x2) * (y1 - y2)) / (x2 - x1);
        x2 = im->w - 1;
     }
   dx = x2 - x1;
   dy = y2 - y1;
   if (x1 > x2)
     {
        int                 tmp;

        tmp = x1;
        x1 = x2;
        x2 = tmp;
        tmp = y1;
        y1 = y2;
        y2 = tmp;
        dx = x2 - x1;
        dy = y2 - y1;
     }
   switch (op)
     {
       case OP_COPY:
          /* vertical line */
          if (dx == 0)
            {
               if (y1 < y2)
                 {
                    p = &(im->data[(im->w * y1) + x1]);
                    for (y = y1; y <= y2; y++)
                      {
                         BLEND(r, g, b, a, p);
                         p += im->w;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y1, 1, (y2 - y1 + 1));
                 }
               else
                 {
                    p = &(im->data[(im->w * y2) + x1]);
                    for (y = y2; y <= y1; y++)
                      {
                         BLEND(r, g, b, a, p);
                         p += im->w;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y2, 1, (y1 - y2 + 1));
                 }
            }
          /* horizontal line */
          if (dy == 0)
            {
               if (x1 < x2)
                 {
                    p = &(im->data[(im->w * y1) + x1]);
                    for (x = x1; x <= x2; x++)
                      {
                         BLEND(r, g, b, a, p);
                         p++;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y1, (x2 - x1 + 1), 1);
                 }
               else
                 {
                    p = &(im->data[(im->w * y1) + x2]);
                    for (x = x2; x <= x1; x++)
                      {
                         BLEND(r, g, b, a, p);
                         p++;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x2, y1, (x1 - x2 + 1), 1);
                 }
            }
          /* 1    */
          /*  \   */
          /*   \  */
          /*    2 */
          if (y2 > y1)
            {
               /* steep */
               if (dy > dx)
                 {
                    dx = ((dx << 16) / dy);
                    x = x1 << 16;
                    for (y = y1; y <= y2; y++)
                      {
                         xx = x >> 16;
                         am = 256 - (((x - (xx << 16)) + 1) >> 8);
                         aaa = (a * am) >> 8;
                         p = &(im->data[(im->w * y) + xx]);
                         BLEND(r, g, b, aaa, p);
                         if (xx < (im->w - 1))
                           {
                              am = 256 - am;
                              aaa = (a * am) >> 8;
                              p++;
                              BLEND(r, g, b, aaa, p);
                           }
                         x += dx;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y1, (x2 - x1 + 1),
                                             (y2 - y1 + 1));
                 }
               /* shallow */
               else
                 {
                    dy = ((dy << 16) / dx);
                    y = y1 << 16;
                    for (x = x1; x <= x2; x++)
                      {
                         yy = y >> 16;
                         am = 256 - (((y - (yy << 16)) + 1) >> 8);
                         aaa = (a * am) >> 8;
                         p = &(im->data[(im->w * yy) + x]);
                         BLEND(r, g, b, aaa, p);
                         if (yy < (im->h - 1))
                           {
                              am = 256 - am;
                              aaa = (a * am) >> 8;
                              p += im->w;
                              BLEND(r, g, b, aaa, p);
                           }
                         y += dy;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y1, (x2 - x1 + 1),
                                             (y2 - y1 + 1));
                 }
            }
          /*    2 */
          /*   /  */
          /*  /   */
          /* 1    */
          else
            {
               /* steep */
               if (-dy > dx)
                 {
                    dx = ((dx << 16) / -dy);
                    x = (x1 + 1) << 16;
                    for (y = y1; y >= y2; y--)
                      {
                         xx = x >> 16;
                         am = (((x - (xx << 16)) + 1) >> 8);
                         aaa = (a * am) >> 8;
                         p = &(im->data[(im->w * y) + xx]);
                         BLEND(r, g, b, aaa, p);
                         if (xx < (im->w - 1))
                           {
                              am = 256 - am;
                              aaa = (a * am) >> 8;
                              p--;
                              BLEND(r, g, b, aaa, p);
                           }
                         x += dx;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y2, (x2 - x1 + 1),
                                             (y1 - y2 + 1));
                 }
               /* shallow */
               else
                 {
                    dy = ((dy << 16) / dx);
                    y = y1 << 16;
                    for (x = x1; x <= x2; x++)
                      {
                         yy = y >> 16;
                         am = 256 - (((y - (yy << 16)) + 1) >> 8);
                         aaa = (a * am) >> 8;
                         p = &(im->data[(im->w * yy) + x]);
                         BLEND(r, g, b, aaa, p);
                         if (yy < (im->h - 1))
                           {
                              am = 256 - am;
                              aaa = (a * am) >> 8;
                              p += im->w;
                              BLEND(r, g, b, aaa, p);
                           }
                         y += dy;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y2, (x2 - x1 + 1),
                                             (y1 - y2 + 1));
                 }
            }
          break;
       case OP_ADD:
          /* vertical line */
          if (dx == 0)
            {
               if (y1 < y2)
                 {
                    p = &(im->data[(im->w * y1) + x1]);
                    for (y = y1; y <= y2; y++)
                      {
                         BLEND_ADD(r, g, b, a, p);
                         p += im->w;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y1, 1, (y2 - y1 + 1));
                 }
               else
                 {
                    p = &(im->data[(im->w * y2) + x1]);
                    for (y = y2; y <= y1; y++)
                      {
                         BLEND_ADD(r, g, b, a, p);
                         p += im->w;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y2, 1, (y1 - y2 + 1));
                 }
            }
          /* horizontal line */
          if (dy == 0)
            {
               if (x1 < x2)
                 {
                    p = &(im->data[(im->w * y1) + x1]);
                    for (x = x1; x <= x2; x++)
                      {
                         BLEND_ADD(r, g, b, a, p);
                         p++;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y1, (x2 - x1 + 1), 1);
                 }
               else
                 {
                    p = &(im->data[(im->w * y1) + x2]);
                    for (x = x2; x <= x1; x++)
                      {
                         BLEND_ADD(r, g, b, a, p);
                         p++;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x2, y1, (x1 - x2 + 1), 1);
                 }
            }
          /* 1    */
          /*  \   */
          /*   \  */
          /*    2 */
          if (y2 > y1)
            {
               /* steep */
               if (dy > dx)
                 {
                    dx = ((dx << 16) / dy);
                    x = x1 << 16;
                    for (y = y1; y <= y2; y++)
                      {
                         xx = x >> 16;
                         am = 256 - (((x - (xx << 16)) + 1) >> 8);
                         aaa = (a * am) >> 8;
                         p = &(im->data[(im->w * y) + xx]);
                         BLEND_ADD(r, g, b, aaa, p);
                         if (xx < (im->w - 1))
                           {
                              am = 256 - am;
                              aaa = (a * am) >> 8;
                              p++;
                              BLEND_ADD(r, g, b, aaa, p);
                           }
                         x += dx;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y1, (x2 - x1 + 1),
                                             (y2 - y1 + 1));
                 }
               /* shallow */
               else
                 {
                    dy = ((dy << 16) / dx);
                    y = y1 << 16;
                    for (x = x1; x <= x2; x++)
                      {
                         yy = y >> 16;
                         am = 256 - (((y - (yy << 16)) + 1) >> 8);
                         aaa = (a * am) >> 8;
                         p = &(im->data[(im->w * yy) + x]);
                         BLEND_ADD(r, g, b, aaa, p);
                         if (yy < (im->h - 1))
                           {
                              am = 256 - am;
                              aaa = (a * am) >> 8;
                              p += im->w;
                              BLEND_ADD(r, g, b, aaa, p);
                           }
                         y += dy;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y1, (x2 - x1 + 1),
                                             (y2 - y1 + 1));
                 }
            }
          /*    2 */
          /*   /  */
          /*  /   */
          /* 1    */
          else
            {
               /* steep */
               if (-dy > dx)
                 {
                    dx = ((dx << 16) / -dy);
                    x = (x1 + 1) << 16;
                    for (y = y1; y >= y2; y--)
                      {
                         xx = x >> 16;
                         am = (((x - (xx << 16)) + 1) >> 8);
                         aaa = (a * am) >> 8;
                         p = &(im->data[(im->w * y) + xx]);
                         BLEND_ADD(r, g, b, aaa, p);
                         if (xx < (im->w - 1))
                           {
                              am = 256 - am;
                              aaa = (a * am) >> 8;
                              p--;
                              BLEND_ADD(r, g, b, aaa, p);
                           }
                         x += dx;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y2, (x2 - x1 + 1),
                                             (y1 - y2 + 1));
                 }
               /* shallow */
               else
                 {
                    dy = ((dy << 16) / dx);
                    y = y1 << 16;
                    for (x = x1; x <= x2; x++)
                      {
                         yy = y >> 16;
                         am = 256 - (((y - (yy << 16)) + 1) >> 8);
                         aaa = (a * am) >> 8;
                         p = &(im->data[(im->w * yy) + x]);
                         BLEND_ADD(r, g, b, aaa, p);
                         if (yy < (im->h - 1))
                           {
                              am = 256 - am;
                              aaa = (a * am) >> 8;
                              p += im->w;
                              BLEND_ADD(r, g, b, aaa, p);
                           }
                         y += dy;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y2, (x2 - x1 + 1),
                                             (y1 - y2 + 1));
                 }
            }
          break;
       case OP_SUBTRACT:
          /* vertical line */
          if (dx == 0)
            {
               if (y1 < y2)
                 {
                    p = &(im->data[(im->w * y1) + x1]);
                    for (y = y1; y <= y2; y++)
                      {
                         BLEND_SUB(r, g, b, a, p);
                         p += im->w;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y1, 1, (y2 - y1 + 1));
                 }
               else
                 {
                    p = &(im->data[(im->w * y2) + x1]);
                    for (y = y2; y <= y1; y++)
                      {
                         BLEND_SUB(r, g, b, a, p);
                         p += im->w;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y2, 1, (y1 - y2 + 1));
                 }
            }
          /* horizontal line */
          if (dy == 0)
            {
               if (x1 < x2)
                 {
                    p = &(im->data[(im->w * y1) + x1]);
                    for (x = x1; x <= x2; x++)
                      {
                         BLEND_SUB(r, g, b, a, p);
                         p++;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y1, (x2 - x1 + 1), 1);
                 }
               else
                 {
                    p = &(im->data[(im->w * y1) + x2]);
                    for (x = x2; x <= x1; x++)
                      {
                         BLEND_SUB(r, g, b, a, p);
                         p++;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x2, y1, (x1 - x2 + 1), 1);
                 }
            }
          /* 1    */
          /*  \   */
          /*   \  */
          /*    2 */
          if (y2 > y1)
            {
               /* steep */
               if (dy > dx)
                 {
                    dx = ((dx << 16) / dy);
                    x = x1 << 16;
                    for (y = y1; y <= y2; y++)
                      {
                         xx = x >> 16;
                         am = 256 - (((x - (xx << 16)) + 1) >> 8);
                         aaa = (a * am) >> 8;
                         p = &(im->data[(im->w * y) + xx]);
                         BLEND_SUB(r, g, b, aaa, p);
                         if (xx < (im->w - 1))
                           {
                              am = 256 - am;
                              aaa = (a * am) >> 8;
                              p++;
                              BLEND_SUB(r, g, b, aaa, p);
                           }
                         x += dx;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y1, (x2 - x1 + 1),
                                             (y2 - y1 + 1));
                 }
               /* shallow */
               else
                 {
                    dy = ((dy << 16) / dx);
                    y = y1 << 16;
                    for (x = x1; x <= x2; x++)
                      {
                         yy = y >> 16;
                         am = 256 - (((y - (yy << 16)) + 1) >> 8);
                         aaa = (a * am) >> 8;
                         p = &(im->data[(im->w * yy) + x]);
                         BLEND_SUB(r, g, b, aaa, p);
                         if (yy < (im->h - 1))
                           {
                              am = 256 - am;
                              aaa = (a * am) >> 8;
                              p += im->w;
                              BLEND_SUB(r, g, b, aaa, p);
                           }
                         y += dy;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y1, (x2 - x1 + 1),
                                             (y2 - y1 + 1));
                 }
            }
          /*    2 */
          /*   /  */
          /*  /   */
          /* 1    */
          else
            {
               /* steep */
               if (-dy > dx)
                 {
                    dx = ((dx << 16) / -dy);
                    x = (x1 + 1) << 16;
                    for (y = y1; y >= y2; y--)
                      {
                         xx = x >> 16;
                         am = (((x - (xx << 16)) + 1) >> 8);
                         aaa = (a * am) >> 8;
                         p = &(im->data[(im->w * y) + xx]);
                         BLEND_SUB(r, g, b, aaa, p);
                         if (xx < (im->w - 1))
                           {
                              am = 256 - am;
                              aaa = (a * am) >> 8;
                              p--;
                              BLEND_SUB(r, g, b, aaa, p);
                           }
                         x += dx;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y2, (x2 - x1 + 1),
                                             (y1 - y2 + 1));
                 }
               /* shallow */
               else
                 {
                    dy = ((dy << 16) / dx);
                    y = y1 << 16;
                    for (x = x1; x <= x2; x++)
                      {
                         yy = y >> 16;
                         am = 256 - (((y - (yy << 16)) + 1) >> 8);
                         aaa = (a * am) >> 8;
                         p = &(im->data[(im->w * yy) + x]);
                         BLEND_SUB(r, g, b, aaa, p);
                         if (yy < (im->h - 1))
                           {
                              am = 256 - am;
                              aaa = (a * am) >> 8;
                              p += im->w;
                              BLEND_SUB(r, g, b, aaa, p);
                           }
                         y += dy;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y2, (x2 - x1 + 1),
                                             (y1 - y2 + 1));
                 }
            }
          break;
       case OP_RESHADE:
          /* vertical line */
          if (dx == 0)
            {
               if (y1 < y2)
                 {
                    p = &(im->data[(im->w * y1) + x1]);
                    for (y = y1; y <= y2; y++)
                      {
                         BLEND_RE(r, g, b, a, p);
                         p += im->w;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y1, 1, (y2 - y1 + 1));
                 }
               else
                 {
                    p = &(im->data[(im->w * y2) + x1]);
                    for (y = y2; y <= y1; y++)
                      {
                         BLEND_RE(r, g, b, a, p);
                         p += im->w;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y2, 1, (y1 - y2 + 1));
                 }
            }
          /* horizontal line */
          if (dy == 0)
            {
               if (x1 < x2)
                 {
                    p = &(im->data[(im->w * y1) + x1]);
                    for (x = x1; x <= x2; x++)
                      {
                         BLEND_RE(r, g, b, a, p);
                         p++;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y1, (x2 - x1 + 1), 1);
                 }
               else
                 {
                    p = &(im->data[(im->w * y1) + x2]);
                    for (x = x2; x <= x1; x++)
                      {
                         BLEND_RE(r, g, b, a, p);
                         p++;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x2, y1, (x1 - x2 + 1), 1);
                 }
            }
          /* 1    */
          /*  \   */
          /*   \  */
          /*    2 */
          if (y2 > y1)
            {
               /* steep */
               if (dy > dx)
                 {
                    dx = ((dx << 16) / dy);
                    x = x1 << 16;
                    for (y = y1; y <= y2; y++)
                      {
                         xx = x >> 16;
                         am = 256 - (((x - (xx << 16)) + 1) >> 8);
                         aaa = (a * am) >> 8;
                         p = &(im->data[(im->w * y) + xx]);
                         BLEND_RE(r, g, b, aaa, p);
                         if (xx < (im->w - 1))
                           {
                              am = 256 - am;
                              aaa = (a * am) >> 8;
                              p++;
                              BLEND_RE(r, g, b, aaa, p);
                           }
                         x += dx;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y1, (x2 - x1 + 1),
                                             (y2 - y1 + 1));
                 }
               /* shallow */
               else
                 {
                    dy = ((dy << 16) / dx);
                    y = y1 << 16;
                    for (x = x1; x <= x2; x++)
                      {
                         yy = y >> 16;
                         am = 256 - (((y - (yy << 16)) + 1) >> 8);
                         aaa = (a * am) >> 8;
                         p = &(im->data[(im->w * yy) + x]);
                         BLEND_RE(r, g, b, aaa, p);
                         if (yy < (im->h - 1))
                           {
                              am = 256 - am;
                              aaa = (a * am) >> 8;
                              p += im->w;
                              BLEND_RE(r, g, b, aaa, p);
                           }
                         y += dy;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y1, (x2 - x1 + 1),
                                             (y2 - y1 + 1));
                 }
            }
          /*    2 */
          /*   /  */
          /*  /   */
          /* 1    */
          else
            {
               /* steep */
               if (-dy > dx)
                 {
                    dx = ((dx << 16) / -dy);
                    x = (x1 + 1) << 16;
                    for (y = y1; y >= y2; y--)
                      {
                         xx = x >> 16;
                         am = (((x - (xx << 16)) + 1) >> 8);
                         aaa = (a * am) >> 8;
                         p = &(im->data[(im->w * y) + xx]);
                         BLEND_RE(r, g, b, aaa, p);
                         if (xx < (im->w - 1))
                           {
                              am = 256 - am;
                              aaa = (a * am) >> 8;
                              p--;
                              BLEND_RE(r, g, b, aaa, p);
                           }
                         x += dx;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y2, (x2 - x1 + 1),
                                             (y1 - y2 + 1));
                 }
               /* shallow */
               else
                 {
                    dy = ((dy << 16) / dx);
                    y = y1 << 16;
                    for (x = x1; x <= x2; x++)
                      {
                         yy = y >> 16;
                         am = 256 - (((y - (yy << 16)) + 1) >> 8);
                         aaa = (a * am) >> 8;
                         p = &(im->data[(im->w * yy) + x]);
                         BLEND_RE(r, g, b, aaa, p);
                         if (yy < (im->h - 1))
                           {
                              am = 256 - am;
                              aaa = (a * am) >> 8;
                              p += im->w;
                              BLEND_RE(r, g, b, aaa, p);
                           }
                         y += dy;
                      }
                    if (!make_updates)
                       return NULL;
                    return __imlib_AddUpdate(NULL, x1, y2, (x2 - x1 + 1),
                                             (y1 - y2 + 1));
                 }
            }
          break;
       default:
          break;
     }
   return NULL;
}

void
__imlib_draw_box(ImlibImage * im, int x, int y, int w, int h, DATA8 r,
                 DATA8 g, DATA8 b, DATA8 a, ImlibOp op)
{
   __imlib_draw_line(im, x, y, x + w - 1, y, r, g, b, a, op, 0);
   __imlib_draw_line(im, x, y, x, y + h - 1, r, g, b, a, op, 0);
   __imlib_draw_line(im, x, y + h - 1, x + w - 1, y + h - 1, r, g, b, a, op, 0);
   __imlib_draw_line(im, x + w - 1, y, x + w - 1, y + h - 1, r, g, b, a, op, 0);
}

void
__imlib_draw_box_clipped(ImlibImage * im, int x, int y, int w, int h,
                         int clip_xmin, int clip_xmax, int clip_ymin,
                         int clip_ymax, DATA8 r, DATA8 g, DATA8 b, DATA8 a,
                         ImlibOp op)
{
   __imlib_draw_line_clipped(im, x, y, x + w - 1, y, clip_xmin, clip_xmax,
                             clip_ymin, clip_ymax, r, g, b, a, op, 0);
   __imlib_draw_line_clipped(im, x, y, x, y + h - 1, clip_xmin, clip_xmax,
                             clip_ymin, clip_ymax, r, g, b, a, op, 0);
   __imlib_draw_line_clipped(im, x, y + h - 1, x + w - 1, y + h - 1,
                             clip_xmin, clip_xmax, clip_ymin, clip_ymax, r, g,
                             b, a, op, 0);
   __imlib_draw_line_clipped(im, x + w - 1, y, x + w - 1, y + h - 1,
                             clip_xmin, clip_xmax, clip_ymin, clip_ymax, r, g,
                             b, a, op, 0);
}

void
__imlib_draw_filled_box_clipped(ImlibImage * im, int x, int y, int w, int h,
                                int clip_xmin, int clip_xmax, int clip_ymin,
                                int clip_ymax, DATA8 r, DATA8 g, DATA8 b,
                                DATA8 a, ImlibOp op)
{
   int                 yy, xx, tmp;
   DATA32             *p;

   if (x < 0)
     {
        w += x;
        x = 0;
     }
   if (w <= 0)
      return;
   if ((x + w) > im->w)
      w = (im->w - x);
   if (w <= 0)
      return;
   if (y < 0)
     {
        h += y;
        y = 0;
     }
   if (h <= 0)
      return;
   if ((y + h) > im->h)
      h = (im->h - y);
   if (h <= 0)
      return;

   if (clip_xmin < 0)
      clip_xmin = 0;
   if (clip_xmax > im->w)
      clip_xmax = im->w;
   if (clip_ymin < 0)
      clip_ymin = 0;
   if (clip_ymax > im->h)
      clip_ymax = im->h;

   if (x < clip_xmin)
     {
        w -= (clip_xmin - x);
        x = clip_xmin;
     }
   if ((x + w) > clip_xmax)
      w = clip_xmax - x;
   if (y < clip_ymin)
     {
        h -= (clip_ymin - y);
        y = clip_ymin;
     }
   if ((y + h) > clip_ymax)
      h = clip_ymax - y;

   switch (op)
     {
       case OP_COPY:
          for (yy = 0; yy < h; yy++)
            {
               p = im->data + ((y + yy) * im->w) + x;
               for (xx = 0; xx < w; xx++)
                 {
                    BLEND(r, g, b, a, p);
                    p++;
                 }
            }
          break;
       case OP_ADD:
          for (yy = 0; yy < h; yy++)
            {
               p = im->data + ((y + yy) * im->w) + x;
               for (xx = 0; xx < w; xx++)
                 {
                    BLEND_ADD(r, g, b, a, p);
                    p++;
                 }
            }
          break;
       case OP_SUBTRACT:
          for (yy = 0; yy < h; yy++)
            {
               p = im->data + ((y + yy) * im->w) + x;
               for (xx = 0; xx < w; xx++)
                 {
                    BLEND_SUB(r, g, b, a, p);
                    p++;
                 }
            }
          break;
       case OP_RESHADE:
          for (yy = 0; yy < h; yy++)
            {
               p = im->data + ((y + yy) * im->w) + x;
               for (xx = 0; xx < w; xx++)
                 {
                    BLEND_RE(r, g, b, a, p);
                    p++;
                 }
            }
          break;
       default:
          break;
     }
}

void
__imlib_draw_filled_box(ImlibImage * im, int x, int y, int w, int h, DATA8 r,
                        DATA8 g, DATA8 b, DATA8 a, ImlibOp op)
{
   int                 yy, xx, tmp;
   DATA32             *p;

   if (x < 0)
     {
        w += x;
        x = 0;
     }
   if (w <= 0)
      return;
   if ((x + w) > im->w)
      w = (im->w - x);
   if (w <= 0)
      return;
   if (y < 0)
     {
        h += y;
        y = 0;
     }
   if (h <= 0)
      return;
   if ((y + h) > im->h)
      h = (im->h - y);
   if (h <= 0)
      return;
   switch (op)
     {
       case OP_COPY:
          for (yy = 0; yy < h; yy++)
            {
               p = im->data + ((y + yy) * im->w) + x;
               for (xx = 0; xx < w; xx++)
                 {
                    BLEND(r, g, b, a, p);
                    p++;
                 }
            }
          break;
       case OP_ADD:
          for (yy = 0; yy < h; yy++)
            {
               p = im->data + ((y + yy) * im->w) + x;
               for (xx = 0; xx < w; xx++)
                 {
                    BLEND_ADD(r, g, b, a, p);
                    p++;
                 }
            }
          break;
       case OP_SUBTRACT:
          for (yy = 0; yy < h; yy++)
            {
               p = im->data + ((y + yy) * im->w) + x;
               for (xx = 0; xx < w; xx++)
                 {
                    BLEND_SUB(r, g, b, a, p);
                    p++;
                 }
            }
          break;
       case OP_RESHADE:
          for (yy = 0; yy < h; yy++)
            {
               p = im->data + ((y + yy) * im->w) + x;
               for (xx = 0; xx < w; xx++)
                 {
                    BLEND_RE(r, g, b, a, p);
                    p++;
                 }
            }
          break;
       default:
          break;
     }
}

void
__imlib_copy_image_data(ImlibImage * im, int x, int y, int w, int h, int nx,
                        int ny)
{
   int                 xx, yy, jump;
   DATA32             *p1, *p2;

   /* clip horizontal co-ordinates so that both dest and src fit inside */
   /* the image */
   if (x < 0)
     {
        w += x;
        nx -= x;
        x = 0;
     }
   if (w <= 0)
      return;
   if (nx < 0)
     {
        w += nx;
        x -= nx;
        nx = 0;
     }
   if (w <= 0)
      return;
   if ((x + w) > im->w)
      w = (im->w - x);
   if (w <= 0)
      return;
   if ((nx + w) > im->w)
      w = (im->w - nx);
   if (w <= 0)
      return;
   /* clip vertical co-ordinates so that both dest and src fit inside */
   /* the image */
   if (y < 0)
     {
        h += y;
        ny -= y;
        y = 0;
     }
   if (h <= 0)
      return;
   if (ny < 0)
     {
        h += ny;
        y -= ny;
        ny = 0;
     }
   if (h <= 0)
      return;
   if ((y + h) > im->h)
      h = (im->h - y);
   if (h <= 0)
      return;
   if ((ny + h) > im->h)
      h = (im->h - ny);
   if (h <= 0)
      return;

   /* figure out what our source and destnation start pointers are */
   p1 = im->data + (y * im->w) + x;
   p2 = im->data + (ny * im->w) + nx;
   /* the pointer jump between lines */
   jump = (im->w - w);
   /* dest < src address - we can copy forwards */
   if (p2 < p1)
     {
        /* work our way thru the array */
        for (yy = 0; yy < h; yy++)
          {
             for (xx = 0; xx < w; xx++)
               {
                  *p2 = *p1;
                  p1++;
                  p2++;
               }
             p1 += jump;
             p2 += jump;
          }
     }
   /* dst > src - we must copy backwards */
   else
     {
        /* new pointers to start working at (bottom-right of rect) */
        p1 = im->data + ((y + h - 1) * im->w) + x + w - 1;
        p2 = im->data + ((ny + h - 1) * im->w) + nx + w - 1;
        /* work our way thru the array */
        for (yy = 0; yy < h; yy++)
          {
             for (xx = 0; xx < w; xx++)
               {
                  *p2 = *p1;
                  p1--;
                  p2--;
               }
             p1 -= jump;
             p2 -= jump;
          }
     }
}

void
__imlib_copy_alpha_data(ImlibImage * src, ImlibImage * dst, int x, int y,
                        int w, int h, int nx, int ny)
{
   int                 xx, yy, jump, jump2;
   DATA32             *p1, *p2;

   /* clip horizontal co-ordinates so that both dest and src fit inside */
   /* the image */
   if (x < 0)
     {
        w += x;
        nx -= x;
        x = 0;
     }
   if (w <= 0)
      return;
   if (nx < 0)
     {
        w += nx;
        x -= nx;
        nx = 0;
     }
   if (w <= 0)
      return;
   if ((x + w) > src->w)
      w = (src->w - x);
   if (w <= 0)
      return;
   if ((nx + w) > dst->w)
      w = (dst->w - nx);
   if (w <= 0)
      return;
   /* clip vertical co-ordinates so that both dest and src fit inside */
   /* the image */
   if (y < 0)
     {
        h += y;
        ny -= y;
        y = 0;
     }
   if (h <= 0)
      return;
   if (ny < 0)
     {
        h += ny;
        y -= ny;
        ny = 0;
     }
   if (h <= 0)
      return;
   if ((y + h) > src->h)
      h = (src->h - y);
   if (h <= 0)
      return;
   if ((ny + h) > dst->h)
      h = (dst->h - ny);
   if (h <= 0)
      return;

   /* figure out what our source and destnation start pointers are */
   p1 = src->data + (y * src->w) + x;
   p2 = dst->data + (ny * dst->w) + nx;
   /* the pointer jump between lines */
   jump = (src->w - w);
   jump2 = (dst->w - w);
   /* copy forwards */
   if (p2 < p1)
     {
        /* work our way thru the array */
        for (yy = 0; yy < h; yy++)
          {
             for (xx = 0; xx < w; xx++)
               {
                  *p2 = (*p1 & 0xff000000) | (*p2 & 0x00ffffff);
                  p1++;
                  p2++;
               }
             p1 += jump;
             p2 += jump2;
          }
     }
}

ImlibUpdate        *
__imlib_draw_line_clipped(ImlibImage * im, int x1, int y1, int x2, int y2,
                          int clip_xmin, int clip_xmax, int clip_ymin,
                          int clip_ymax, DATA8 r, DATA8 g, DATA8 b, DATA8 a,
                          ImlibOp op, char make_updates)
{
   int                 cx0, cx1, cy0, cy1;

   if (imlib_clip_line
       (x1, y1, x2, y2, clip_xmin, clip_xmax - 1, clip_ymin, clip_ymax, &cx0,
        &cy0, &cx1, &cy1))
     {
        return __imlib_draw_line(im, cx0, cy0, cx1, cy1, r, g, b, a, op,
                                 make_updates);
     }
   else
      return NULL;
}

int
imlib_clip_line(int x0, int y0, int x1, int y1, int xmin, int xmax, int ymin,
                int ymax, int *clip_x0, int *clip_y0, int *clip_x1,
                int *clip_y1)
{
   ImlibOutCode        outcode0, outcode1, outcode_out;
   unsigned char       accept = FALSE, done = FALSE;
   double              dx0, dy0, dx1, dy1;

   dx0 = x0;
   dx1 = x1;
   dy0 = y0;
   dy1 = y1;

   outcode0 = __imlib_comp_outcode(dx0, dy0, xmin, xmax, ymin, ymax);
   outcode1 = __imlib_comp_outcode(dx1, dy1, xmin, xmax, ymin, ymax);

   do
     {
        if (!(outcode0 | outcode1))
          {
             accept = TRUE;
             done = TRUE;
          }
        else if (outcode0 & outcode1)
           done = TRUE;
        else
          {
             double              x, y;

             outcode_out = outcode0 ? outcode0 : outcode1;
             if (outcode_out & TOP)
               {
                  x = dx0 + (dx1 - dx0) * ((double)ymax - dy0) / (dy1 - dy0);
                  y = ymax;
               }
             else if (outcode_out & BOTTOM)
               {
                  x = dx0 + (dx1 - dx0) * ((double)ymin - dy0) / (dy1 - dy0);
                  y = ymin;
               }
             else if (outcode_out & RIGHT)
               {
                  y = dy0 + (dy1 - dy0) * ((double)xmax - dx0) / (dx1 - dx0);
                  x = xmax;
               }
             else
               {
                  y = dy0 + (dy1 - dy0) * ((double)xmin - dx0) / (dx1 - dx0);
                  x = xmin;
               }
             if (outcode_out == outcode0)
               {
                  dx0 = x;
                  dy0 = y;
                  outcode0 =
                      __imlib_comp_outcode(dx0, dy0, xmin, xmax, ymin, ymax);
               }
             else
               {
                  dx1 = x;
                  dy1 = y;
                  outcode1 =
                      __imlib_comp_outcode(dx1, dy1, xmin, xmax, ymin, ymax);
               }
          }
     }
   while (done == FALSE);

   /* round up before converting down to ints */
   dx0 = round(dx0);
   dx1 = round(dx1);
   dy0 = round(dy0);
   dy1 = round(dy1);

   *clip_x0 = dx0;
   *clip_y0 = dy0;
   *clip_x1 = dx1;
   *clip_y1 = dy1;

   return accept;
}

ImlibOutCode
__imlib_comp_outcode(double x, double y, double xmin,
                     double xmax, double ymin, double ymax)
{
   ImlibOutCode        code = 0;

   if (y > ymax)
      code |= TOP;
   else if (y < ymin)
      code |= BOTTOM;
   if (x > xmax)
      code |= RIGHT;
   else if (x < xmin)
      code |= LEFT;
   return code;
}

ImlibPoly
__imlib_polygon_new()
{
   ImlibPoly           poly;

   poly = malloc(sizeof(_ImlibPoly));
   if (!poly)
      return NULL;
   memset(poly, 0, sizeof(_ImlibPoly));
   return poly;
}

void
__imlib_polygon_add_point(ImlibPoly poly, int x, int y)
{
   poly->pointcount++;
   if (!poly->points)
      poly->points = malloc(sizeof(ImlibPoint));
   else
      poly->points =
          realloc(poly->points, (poly->pointcount * sizeof(ImlibPoint)));
   poly->points[poly->pointcount - 1].x = x;
   poly->points[poly->pointcount - 1].y = y;
}

void
__imlib_polygon_free(ImlibPoly poly)
{
   if (poly->points)
      free(poly->points);
   free(poly);
}

void
__imlib_draw_polygon(ImlibImage * im, ImlibPoly poly, unsigned char closed,
                     DATA8 r, DATA8 g, DATA8 b, DATA8 a, ImlibOp op)
{
   int                 i;

   if (!poly || !poly->points || (poly->pointcount < 2))
      return;

   for (i = 0; i < poly->pointcount; i++)
     {
        if (i < poly->pointcount - 1)
           __imlib_draw_line(im, poly->points[i].x, poly->points[i].y,
                             poly->points[i + 1].x, poly->points[i + 1].y, r, g,
                             b, a, op, 0);
        else if (closed)
           __imlib_draw_line(im, poly->points[i].x, poly->points[i].y,
                             poly->points[0].x, poly->points[0].y, r, g, b, a,
                             op, 0);
        else
           break;
     }
}

void
__imlib_draw_polygon_clipped(ImlibImage * im, ImlibPoly poly,
                             unsigned char closed, int clip_xmin,
                             int clip_xmax, int clip_ymin, int clip_ymax,
                             DATA8 r, DATA8 g, DATA8 b, DATA8 a, ImlibOp op)
{
   int                 i;

   if (!poly || !poly->points || (poly->pointcount < 2))
      return;

   for (i = 0; i < poly->pointcount; i++)
     {
        if (i < poly->pointcount - 1)
           __imlib_draw_line_clipped(im, poly->points[i].x, poly->points[i].y,
                                     poly->points[i + 1].x,
                                     poly->points[i + 1].y, clip_xmin,
                                     clip_xmax, clip_ymin, clip_ymax, r, g, b,
                                     a, op, 0);
        else if (closed)
           __imlib_draw_line_clipped(im, poly->points[i].x, poly->points[i].y,
                                     poly->points[0].x, poly->points[0].y,
                                     clip_xmin, clip_xmax, clip_ymin, clip_ymax,
                                     r, g, b, a, op, 0);
        else
           break;
     }
}

void
__imlib_polygon_get_bounds(ImlibPoly poly, int *px1, int *py1, int *px2,
                           int *py2)
{
   int                 x1 = 0, y1 = 0, x2 = 0, y2 = 0;
   int                 i;

   *px1 = 0;
   *py1 = 0;
   *px2 = 0;
   *py2 = 0;

   if (!poly || !poly->points || (poly->pointcount < 1))
      return;

   x1 = x2 = poly->points[0].x;
   y1 = y2 = poly->points[0].y;

   for (i = 1; i < poly->pointcount; i++)
      GROW_BOUNDS(x1, y1, x2, y2, poly->points[i].x, poly->points[i].y);

   *px1 = x1;
   *py1 = y1;
   *px2 = x2;
   *py2 = y2;
}

void
__imlib_draw_ellipse(ImlibImage * im, int xc, int yc, int aa, int bb, DATA8 r,
                     DATA8 g, DATA8 b, DATA8 a, ImlibOp op)
{
   int                 a2 = aa * aa;
   int                 b2 = bb * bb;

   int                 x, y, dec;

   for (x = 0, y = bb, dec = 2 * b2 + a2 * (1 - 2 * bb); b2 * x <= a2 * y; x++)
     {
        __imlib_draw_set_point(im, xc + x, yc + y, r, g, b, a, op);
        __imlib_draw_set_point(im, xc - x, yc + y, r, g, b, a, op);
        __imlib_draw_set_point(im, xc + x, yc - y, r, g, b, a, op);
        __imlib_draw_set_point(im, xc - x, yc - y, r, g, b, a, op);

        if (dec >= 0)
           dec += 4 * a2 * (1 - (y--));
        dec += b2 * (4 * x + 6);
     }

   for (x = aa, y = 0, dec = 2 * a2 + b2 * (1 - 2 * aa); a2 * y <= b2 * x; y++)
     {
        __imlib_draw_set_point(im, xc + x, yc + y, r, g, b, a, op);
        __imlib_draw_set_point(im, xc - x, yc + y, r, g, b, a, op);
        __imlib_draw_set_point(im, xc + x, yc - y, r, g, b, a, op);
        __imlib_draw_set_point(im, xc - x, yc - y, r, g, b, a, op);

        if (dec >= 0)
           dec += 4 * b2 * (1 - (x--));
        dec += a2 * (4 * y + 6);
     }
}

void
__imlib_fill_ellipse(ImlibImage * im, int xc, int yc, int aa, int bb,
                     int clip_xmin, int clip_xmax, int clip_ymin,
                     int clip_ymax, DATA8 r, DATA8 g, DATA8 b, DATA8 a,
                     ImlibOp op, unsigned char antialias)
{
   int                 a2 = aa * aa;
   int                 b2 = bb * bb;
   int                 y;
   int                 x, dec;
   int                 miny, maxy;
   int                 clip = 0;

   edgeRec            *table1, *table2;

   if ((clip_xmin != 0) && (clip_ymin != 0) && (clip_xmax != -1)
       && (clip_ymax != -1))
      clip = 1;

   table1 = malloc(sizeof(edgeRec) * (im->h + 1));
   table2 = malloc(sizeof(edgeRec) * (im->h + 1));
   memset(table1, 0, sizeof(edgeRec) * (im->h + 1));
   memset(table2, 0, sizeof(edgeRec) * (im->h + 1));

   miny = yc - bb - 1;
   maxy = yc + bb + 1;

   for (x = 0, y = bb, dec = 2 * b2 + a2 * (1 - 2 * bb); b2 * x <= a2 * y; x++)
     {
        if (((yc - y) >= 0) && ((yc - y) < im->h))
          {
             table1[yc - y].x = xc - x;
             table2[yc - y].x = xc + x;
          }

        if (((yc + y) >= 0) && ((yc + y) < im->h))
          {
             table1[yc + y].x = xc - x;
             table2[yc + y].x = xc + x;
          }

        if (dec >= 0.0)
           dec += 4.0 * a2 * (1 - (y--));
        dec += b2 * (4 * x + 6);
     }

   for (x = aa, y = 0, dec = 2 * a2 + b2 * (1 - 2 * aa); a2 * y <= b2 * x; y++)
     {
        if (((yc - y) >= 0) && ((yc - y) < im->h))
          {
             table1[yc - y].x = xc - x;
             table2[yc - y].x = xc + x;
          }

        if (((yc + y) >= 0) && ((yc + y) < im->h))
          {
             table1[yc + y].x = xc - x;
             table2[yc + y].x = xc + x;
          }

        if (dec >= 0)
           dec += 4 * b2 * (1 - (x--));
        dec += a2 * (4 * y + 6);
     }

   /* clip spans to image size */
   __spanlist_clip(table1, table2, &miny, &maxy, 0, im->w, 0, im->h - 1);

   /* clip to clip rect if it's there */
   if (clip)
      __spanlist_clip(table1, table2, &miny, &maxy, clip_xmin, clip_xmax,
                      clip_ymin, clip_ymax);

   if (miny < 0)
      miny = 0;
   if (miny >= im->h)
     {
        free(table1);
        free(table2);
        return;
     }
   if (maxy < 0)
     {
        free(table1);
        free(table2);
        return;
     }
   if (maxy >= im->h)
      maxy = im->h - 1;
   {
      do
        {
           int                 x1, x2;

           x1 = table1[miny].x;
           x2 = table2[miny].x;
           if (clip)
             {
                if (x1 < clip_xmin)
                   x1 = clip_xmin;
                if (x2 > clip_xmax)
                   x2 = clip_xmax;
             }
           if ((x1 != x2) && (x1 < im->w))
              span(im, miny, x1, x2, r, g, b, a, op);
           miny++;
        }
      while (miny < maxy);
   }
   free(table1);
   free(table2);
}

void
__imlib_draw_ellipse_clipped(ImlibImage * im, int xc, int yc, int aa, int bb,
                             int clip_xmin, int clip_xmax, int clip_ymin,
                             int clip_ymax, DATA8 r, DATA8 g, DATA8 b,
                             DATA8 a, ImlibOp op)
{
   int                 a2 = aa * aa;
   int                 b2 = bb * bb;

   int                 x, y, dec;

   for (x = 0, y = bb, dec = 2 * b2 + a2 * (1 - 2 * bb); b2 * x <= a2 * y; x++)
     {
        __imlib_draw_set_point_clipped(im, xc + x, yc + y, clip_xmin, clip_xmax,
                                       clip_ymin, clip_ymax, r, g, b, a, op);
        __imlib_draw_set_point_clipped(im, xc - x, yc + y, clip_xmin, clip_xmax,
                                       clip_ymin, clip_ymax, r, g, b, a, op);
        __imlib_draw_set_point_clipped(im, xc + x, yc - y, clip_xmin, clip_xmax,
                                       clip_ymin, clip_ymax, r, g, b, a, op);
        __imlib_draw_set_point_clipped(im, xc - x, yc - y, clip_xmin, clip_xmax,
                                       clip_ymin, clip_ymax, r, g, b, a, op);

        if (dec >= 0)
           dec += 4 * a2 * (1 - (y--));
        dec += b2 * (4 * x + 6);
     }

   for (x = aa, y = 0, dec = 2 * a2 + b2 * (1 - 2 * aa); a2 * y <= b2 * x; y++)
     {
        __imlib_draw_set_point_clipped(im, xc + x, yc + y, clip_xmin, clip_xmax,
                                       clip_ymin, clip_ymax, r, g, b, a, op);
        __imlib_draw_set_point_clipped(im, xc - x, yc + y, clip_xmin, clip_xmax,
                                       clip_ymin, clip_ymax, r, g, b, a, op);
        __imlib_draw_set_point_clipped(im, xc + x, yc - y, clip_xmin, clip_xmax,
                                       clip_ymin, clip_ymax, r, g, b, a, op);
        __imlib_draw_set_point_clipped(im, xc - x, yc - y, clip_xmin, clip_xmax,
                                       clip_ymin, clip_ymax, r, g, b, a, op);

        if (dec >= 0)
           dec += 4 * b2 * (1 - (x--));
        dec += a2 * (4 * y + 6);
     }
}

#if 0
/* unused */
static void
edge(edgeRec * table, ImlibPoint * pt1, ImlibPoint * pt2)
{
   double              x, dx;
   int                 idy, iy1, iy2;

   if (pt2->y < pt1->y)
      exchange(ImlibPoint *, pt1, pt2);
   iy1 = pt1->y;
   iy2 = pt2->y;
   idy = iy2 - iy1;
   if (idy == 0)
      return;
   idy = MAX(2, idy - 1);
   x = pt1->x;
   dx = (pt2->x - pt1->x) / (double)idy;
   do
     {
        table[iy1].x = x;
        x += dx;
        iy1++;
     }
   while (iy1 <= iy2);
}
#endif

static void
span(ImlibImage * im, int y, int x1, int x2, DATA8 r, DATA8 g,
     DATA8 b, DATA8 a, ImlibOp op)
{
   DATA32             *p;
   int                 ix1, ix2;
   int                 tmp;

   if (x1 > x2)
     {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
     }

   ix2 = x2;
   ix1 = x1;
   p = &(im->data[(im->w * y) + x1]);
   switch (op)
     {
          /* unrolled loop - on loop inside each render mode per span */
       case OP_RESHADE:
          do
            {
               BLEND_RE(r, g, b, a, p);
               p++;
               ix1++;
            }
          while (ix1 <= ix2);
          break;
       case OP_SUBTRACT:
          do
            {
               BLEND_SUB(r, g, b, a, p);
               p++;
               ix1++;
            }
          while (ix1 <= ix2);
          break;
       case OP_ADD:
          do
            {
               BLEND_ADD(r, g, b, a, p);
               p++;
               ix1++;
            }
          while (ix1 <= ix2);
          break;
       case OP_COPY:
          do
            {
               BLEND(r, g, b, a, p);
               p++;
               ix1++;
            }
          while (ix1 <= ix2);
          break;
       default:
          break;
     }

}

typedef struct _span Span;
struct _span {
   int                 x, xstart, ystart, vert, xend, yend;
   int                 pol, point;
   double              gradient;
   Span               *next;
};

#define SPANS_COMMON(x1, w1, x2, w2) \
(!((((x2) + (w2)) <= (x1)) || ((x2) >= ((x1) + (w1)))))
void
__imlib_draw_polygon_filled(ImlibImage * im, ImlibPoly poly, int clip_xmin,
                            int clip_xmax, int clip_ymin, int clip_ymax,
                            DATA8 r, DATA8 g, DATA8 b, DATA8 a, ImlibOp op,
                            unsigned char antialias)
{
   Span              **spans = NULL;
   int                 i, h, j, top, right, left, pol, ppol, dir;

   /* less than 3 points - not even a tirangle - abort */
   if (poly->pointcount < 3)
      return;

   /* if no clip is set or clip is outside image, clip it to the image */
   if (clip_xmin < clip_xmax)
     {
        if (clip_xmax < 0)
           return;
        if (clip_xmin >= im->w)
           return;
        if (clip_ymax < 0)
           return;
        if (clip_ymin >= im->h)
           return;
        if (clip_xmin < 0)
           clip_xmin = 0;
        if (clip_xmax >= im->w)
           clip_xmax = im->w - 1;
        if (clip_ymin < 0)
           clip_ymin = 0;
        if (clip_ymax >= im->h)
           clip_ymax = im->h - 1;
     }
   /* set clip to image size */
   else
     {
        clip_xmin = 0;
        clip_xmax = im->w - 1;
        clip_ymin = 0;
        clip_ymax = im->h - 1;
     }
   /* clip is invalid - ie 0 or less than size - abort */
   if (clip_ymax < clip_ymin)
      return;
   if (clip_xmax < clip_xmin)
      return;

   /* allocate spanlist for the output image */
   h = clip_ymax - clip_ymin + 1;
   if (h < 1)
      return;
   spans = malloc(h * sizeof(Span *));
   if (!spans)
      return;
   memset(spans, 0, h * sizeof(Span *));

   /* find top most poly pont */
   top = 0;
   left = 0;
   right = 0;
   {
      int                 miny, maxx, minx;

      miny = poly->points[0].y;
      minx = poly->points[0].x;
      maxx = poly->points[0].x;
      for (j = 0; j < poly->pointcount; j++)
        {
           if (poly->points[j].y < miny)
             {
                top = j;
                miny = poly->points[j].y;
             }
           if (poly->points[j].x < minx)
             {
                left = j;
                minx = poly->points[j].x;
             }
           if (poly->points[j].x > maxx)
             {
                right = j;
                maxx = poly->points[j].x;
             }
        }
   }
   /* for every point in the poly figure a span for it and the next point */
   pol = 0;
   ppol = 0;
   dir = 1;
   if (right > top)
      dir = 1;
   else if (right == top)
     {
        if (left < top)
           dir = 1;
        else
           dir = -1;
     }
   else
      dir = -1;
   for (j = 0; (j != poly->pointcount) && (j != -poly->pointcount); j += dir)
     {
        int                 pt1, pt2, x1, y1, x2, y2, vert, step;
        double              grad;

        /* current point and next point - line connecting the 2 */
        pt1 = ((j + top) % poly->pointcount);
        pt2 = ((j + top + 1) % poly->pointcount);
        while (pt1 < 0)
           pt1 += poly->pointcount;
        while (pt2 < 0)
           pt2 += poly->pointcount;
        /* conveneince for 2 points */
        x1 = poly->points[pt1].x;
        y1 = poly->points[pt1].y;
        x2 = poly->points[pt2].x;
        y2 = poly->points[pt2].y;
        ppol = pol;
        if (y2 > y1)
           pol = 1;
        else
           pol = -1;
        vert = 0;
        grad = 0;
        /* vertical ? */
        if (x2 == x1)
           vert = 1;
        /* gradient */
        else
           grad = (double)(y2 - y1) / (double)(x2 - x1);
        /* go thru the line span by span */
        step = 1;
        if (y1 >= y2)
           step = -1;
        for (i = y1;;)
          {
             Span               *s;
             int                 x, sx, bx, w;

             sx = x1;
             bx = x2;
             if (x2 < x1)
               {
                  sx = x2;
                  bx = x1;
               }
             w = clip_xmax - clip_xmin + 1;
             if (grad != 0)
               {
                  double              dbx;

                  dbx = ((double)x1 + ((double)(i - y1) / grad));
                  x = (int)dbx;
                  if ((dbx - ((double)x)) >= 0.5)
                     x += 1;
               }
             else if (vert)
                x = x1;
             else
                x = x1;
             if ((i >= clip_ymin) && (i <= clip_ymax))
               {
                  /* for every scanline this line spans add a span point */
                  s = malloc(sizeof(Span));
                  if (pol == 1)
                     s->x = x + 1;
                  else
                     s->x = x;
                  s->pol = pol;
                  s->xstart = x1;
                  s->ystart = y1;
                  s->xend = x2;
                  s->yend = y2;
                  s->vert = vert;
                  s->gradient = grad;
                  s->next = NULL;

                  if ((i == y1) || (i == y2))
                     s->point = 1;
                  else
                     s->point = 0;

                  /* actually add the scan point to the scan list array */
                  if (!(spans[i - clip_ymin]))
                     spans[i - clip_ymin] = s;
                  else
                    {
                       Span               *ps, *ss;

                       ps = NULL;
                       for (ss = spans[i - clip_ymin]; ss; ss = ss->next)
                         {
                            if (s->x <= ss->x)
                              {
                                 if (!ps)
                                    spans[i - clip_ymin] = s;
                                 else
                                    ps->next = s;
                                 s->next = ss;
                                 goto nospans;
                              }
                            ps = ss;
                         }
                       /* last span on line and still not < ss->x */
                       if (ps)
                          ps->next = s;
                     nospans: ;
                    }
               }
             if (i == y2)
                goto nolines;
             i += step;
          }
      nolines: ;
     }
   for (i = 0; i < h; i++)
     {
        Span               *s;

        if (spans[i])
          {
             for (s = spans[i]; s; s = s->next)
               {
                  if ((s->next) &&
                      (s->next->x == s->x) && (s->next->pol == s->pol))
                    {
                       Span               *ss;

                       ss = s->next;
                       s->next = ss->next;
                       free(ss);
                    }
               }
          }
        /* go thru the spans again */
        for (s = spans[i]; s; s = s->next)
          {
             int                 x1, x2;
             Span               *ss;

             x1 = s->x;
             ss = s;
             s = s->next;
             /* if there is... draw it */
             if (s)
               {
                  x2 = s->x;
                  if ((ss->pol == 1) && (s->pol == -1))
                    {
                       x1 = ss->x - 1;
                       x2 = s->x + 1;
                    }
                  if ((x1 <= clip_xmax) && (x2 > clip_xmin) && (x1 < x2))
                    {
                       if (x1 < clip_xmin)
                          x1 = clip_xmin;
                       if (x2 > (clip_xmax + 1))
                          x2 = clip_xmax + 1;
                       span(im, i + clip_ymin, x1, x2 - 1, r, g, b, a, op);
                    }
               }
             else
               {
                  if ((x1 <= clip_xmax) && (x1 >= clip_xmin))
                     span(im, i + clip_ymin, x1, x1, r, g, b, a, op);
                  break;
               }
          }
     }
   /* free the spans */
   for (i = 0; i < h; i++)
     {
        Span               *s, *ss;

        s = spans[i];
        while (s)
          {
             ss = s;
             s = s->next;
             free(ss);
          }
     }
   free(spans);
}

void
__spanlist_clip(edgeRec * table1, edgeRec * table2, int *sy, int *ey,
                int xmin, int xmax, int ymin, int ymax)
{
   edgeRec            *pt1, *pt2;
   int                 iy1, iy2;

   iy1 = MAX(*sy, ymin);
   iy2 = MIN(*ey, ymax);
   *sy = iy1;
   *ey = iy2;

   do
     {
        pt1 = &(table1[iy1]);
        pt2 = &(table2[iy1]);

        if (pt2->x < pt1->x)
           exchange(double, pt2->x, pt1->x);

        if (pt1->x < xmax)
          {
             pt1->x = MAX(pt1->x, xmin);
             pt2->x = MIN(pt2->x, xmax);
          }
        iy1++;
     }
   while (iy1 <= iy2);
}

unsigned char
__imlib_polygon_contains_point(ImlibPoly poly, int x, int y)
{
   int                 count = 0;
   int                 start = 0;
   int                 ysave = 0;       /* initial value arbitrary */
   int                 cx, nx, out_x, out_y, i, n;
   int                 curr_x, curr_y, next_x, next_y;

   /* find a vertex of poly that does not lie on the test line */
   while (start < poly->pointcount && poly->points[start].y == y)
      start++;
   /* if one doesn't exist we will use point on segment test
    * and can start with vertex 0 anyway */
   cx = start % poly->pointcount;

   out_x = poly->points[0].x;
   out_y = y;

   for (i = 1; i < poly->pointcount; i++)
     {
        out_x = MAX(out_x, poly->points[i].x);
     }
   out_x++;                     /* out now guaranteed to be outside poly */

   for (n = 0; n < poly->pointcount; n++)
     {
        nx = (cx + 1) % poly->pointcount;

        curr_x = poly->points[cx].x;
        curr_y = poly->points[cx].y;
        next_x = poly->points[nx].x;
        next_y = poly->points[nx].y;

        if (__imlib_point_on_segment(x, y, curr_x, curr_y, next_x, next_y))
           return TRUE;

        /* ignore horizontal segments from this point on */
        if (poly->points[cx].y != poly->points[nx].y)
          {
             if (__imlib_segments_intersect
                 (curr_x, curr_y, next_x, next_y, x, y, out_x, out_y))
               {
                  count++;

                  if (__imlib_point_on_segment
                      (next_x, next_y, x, y, out_x, out_y))
                    {
                       /* current seg intersects test seg @ 2nd vtx
                        * reset ysave */
                       ysave = curr_y;
                    }
                  if (__imlib_point_on_segment
                      (curr_x, curr_y, x, y, out_x, out_y)
                      && (ysave < y != next_y < y))
                    {
                       /* current seg xsects test seg @ 1st vtx and
                        * ysave on opposite side of test line from
                        * curr seg 2nd vtx;
                        * decrement hits (2-1) for odd parity */
                       count--;
                    }
               }
          }
        cx = nx;
     }
   return (count % 2 == 1);
}

unsigned char
__imlib_segments_intersect(int r1_x, int r1_y, int r2_x, int r2_y, int s1_x,
                           int s1_y, int s2_x, int s2_y)
{
   double              testS1R =
       __imlib_point_delta_from_line(s1_x, s1_y, r1_x, r1_y, r2_x, r2_y);
   double              testS2R =
       __imlib_point_delta_from_line(s2_x, s2_y, r1_x, r1_y, r2_x, r2_y);
   double              testR1S =
       __imlib_point_delta_from_line(r1_x, r1_y, s1_x, s1_y, s2_x, s2_y);
   double              testR2S =
       __imlib_point_delta_from_line(r2_x, r2_y, s1_x, s1_y, s2_x, s2_y);

   /* check if segments are collinear */
   if (testS1R == 0.0 && testS2R == 0.0)
     {
        if (__imlib_point_inside_segment(s1_x, s1_y, r1_x, r1_y, r2_x, r2_y)
            || __imlib_point_inside_segment(s2_x, s2_y, r1_x, r1_y, r2_x, r2_y)
            || __imlib_point_inside_segment(r1_x, r1_y, s1_x, s1_y, s2_x, s2_y)
            || __imlib_point_inside_segment(r2_x, r2_y, s1_x, s1_y, s2_x, s2_y))
           return TRUE;
        else
           return FALSE;
     }

   if (testS1R * testS2R <= 0.0 && testR1S * testR2S <= 0.0)
      return TRUE;
   else
      return FALSE;
}

double
__imlib_point_delta_from_line(int p_x, int p_y, int s1_x, int s1_y, int s2_x,
                              int s2_y)
{
   if (s2_x - s1_x == 0.0)
      return p_x - s1_x;
   else
     {
        double              m = (double)(s2_y - s1_y) / (double)(s2_x - s1_x);

        return (p_y - s1_y - (double)(p_x - s1_x) * m);
     }
}
