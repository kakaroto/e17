#include <X11/Xlib.h>
#include "common.h"
#include "colormod.h"
#include "image.h"
#include "scale.h"
#include "context.h"
#include "rgba.h"
#include "blend.h"
#include "updates.h"
#include "rgbadraw.h"

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
     case 0:                               /*\ DOWN_RIGHT \ */
        tmp = im->border.top;
        im->border.top = im->border.left;
        im->border.left = tmp;
        tmp = im->border.bottom;
        im->border.bottom = im->border.right;
        im->border.right = tmp;
        to = data;
        hw = -hw + 1;
        break;
     case 1:                               /*\ DOWN_LEFT \ */
        tmp = im->border.top;
        im->border.top = im->border.left;
        im->border.left = im->border.bottom;
        im->border.bottom = im->border.right;
        im->border.right = tmp;
        to = data + w - 1;
        hw = -hw - 1;
        break;
     case 2:                               /*\ UP_RIGHT \ */
        tmp = im->border.top;
        im->border.top = im->border.right;
        im->border.right = im->border.bottom;
        im->border.bottom = im->border.left;
        im->border.left = tmp;
        to = data + hw - w;
        w = -w;
        hw = hw + 1;
        break;
     case 3:                               /*\ UP_LEFT \ */
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

   /* FIXME: impliment */

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
         p2 = im->data + ((y - (im->h - (im->h >> 1))) * im->w);
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
   DATA8               aaa, nr, ng, nb, rr, gg, bb, aa, na;

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
__imlib_draw_box(ImlibImage * im, int x, int y, int w, int h, DATA8 r, DATA8 g,
                 DATA8 b, DATA8 a, ImlibOp op)
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
   __imlib_draw_line_clipped(im, x, y + h - 1, x + w - 1, y + h - 1, clip_xmin,
                             clip_xmax, clip_ymin, clip_ymax, r, g, b, a, op,
                             0);
   __imlib_draw_line_clipped(im, x + w - 1, y, x + w - 1, y + h - 1, clip_xmin,
                             clip_xmax, clip_ymin, clip_ymax, r, g, b, a, op,
                             0);
}

void
__imlib_draw_filled_box(ImlibImage * im, int x, int y, int w, int h, DATA8 r,
                        DATA8 g, DATA8 b, DATA8 a, ImlibOp op)
{
   int                 yy, xx, tmp;
   DATA32             *p;
   DATA8               nr, ng, nb, rr, gg, bb, aa, na;

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
__imlib_copy_alpha_data(ImlibImage * src, ImlibImage * dst, int x, int y, int w,
                        int h, int nx, int ny)
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
       (x1, y1, x2, y2, clip_xmin, clip_xmax, clip_ymin, clip_ymax, &cx0, &cy0,
        &cx1, &cy1))
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

   outcode0 = __imlib_comp_outcode(x0, y0, xmin, xmax, ymin, ymax);
   outcode1 = __imlib_comp_outcode(x1, y1, xmin, xmax, ymin, ymax);

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
            x = x0 + (x1 - x0) * (ymax - y0) / (y1 - y0);
            y = ymax;
         }
         else if (outcode_out & BOTTOM)
         {
            x = x0 + (x1 - x0) * (ymin - y0) / (y1 - y0);
            y = ymin;
         }
         else if (outcode_out & RIGHT)
         {
            y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0);
            x = xmax;
         }
         else
         {
            y = y0 + (y1 - y0) * (xmin - x0) / (x1 - x0);
            x = xmin;
         }
         if (outcode_out == outcode0)
         {
            x0 = x;
            y0 = y;
            outcode0 = __imlib_comp_outcode(x0, y0, xmin, xmax, ymin, ymax);
         }
         else
         {
            x1 = x;
            y1 = y;
            outcode1 = __imlib_comp_outcode(x1, y1, xmin, xmax, ymin, ymax);
         }
      }
   }
   while (done == FALSE);

   *clip_x0 = x0;
   *clip_y0 = y0;
   *clip_x1 = x1;
   *clip_y1 = y1;

   return accept;
}

ImlibOutCode
__imlib_comp_outcode(double x, double y, double xmin, double xmax, double ymin,
                     double ymax)
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
