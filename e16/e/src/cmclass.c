/*
 * Copyright (C) 2000-2003 Carsten Haitzler, Geoff Harrison and various contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "E.h"

void
CreateCurve(ModCurve * c)
{
   int                 i, j, cx, v1, v2, val, dist;

   EDBUG(6, "CreateCurve");

   if (!c)
      EDBUG_RETURN_;

   if (c->num == 0)
     {
	for (i = 0; i < 256; i++)
	   c->map[i] = i;
	EDBUG_RETURN_;
     }

   cx = 0;
   c->map[cx++] = c->py[0];

   for (i = 1; i < c->num; i++)
     {
	v1 = c->py[i - 1];
	v2 = c->py[i];
	dist = c->px[i] - c->px[i - 1];
	if (dist < 2)
	  {
	     c->map[cx++] = v2;
	  }
	else
	  {
	     for (j = 0; j < dist; j++)
	       {
		  val = ((v2 * j) + (v1 * (dist - j - 1))) / (dist - 1);
		  c->map[cx++] = (unsigned char)val;
	       }
	  }
     }

   EDBUG_RETURN_;
}

void
FreeModCurve(ModCurve * c)
{
   EDBUG(6, "FreeModCurve");

   if (!c)
      EDBUG_RETURN_;

   Efree(c->px);
   Efree(c->py);

   EDBUG_RETURN_;
}

void
FreeCMClass(ColorModifierClass * cm)
{
   EDBUG(5, "FreeCmClass");

   if (!cm)
      EDBUG_RETURN_;

   if (cm->ref_count > 0)
     {
	DialogOK(_("ColorModClass Error!"), _("%u references remain\n"),
		 cm->ref_count);
	EDBUG_RETURN_;
     }

   while (RemoveItemByPtr(cm, LIST_TYPE_COLORMODIFIER));

   Efree(cm->name);
   FreeModCurve(&(cm->red));
   FreeModCurve(&(cm->green));
   FreeModCurve(&(cm->blue));

   EDBUG_RETURN_;
}

ColorModifierClass *
CreateCMClass(char *name, int rnum, unsigned char *rpx,
	      unsigned char *rpy, int gnum,
	      unsigned char *gpx, unsigned char *gpy,
	      int bnum, unsigned char *bpx, unsigned char *bpy)
{
   ColorModifierClass *cm;

   EDBUG(5, "CreateCMCLass");

   cm = Emalloc(sizeof(ColorModifierClass));
   if (!cm)
      EDBUG_RETURN(NULL);

   cm->name = duplicate(name);
   cm->red.px = NULL;
   cm->red.py = NULL;
   cm->green.px = NULL;
   cm->green.py = NULL;
   cm->blue.px = NULL;
   cm->blue.py = NULL;
   cm->ref_count = 0;

   if (rnum < 2)
     {
	cm->red.num = 0;
     }
   else
     {
	cm->red.num = rnum;
	cm->red.px = Emalloc(rnum);
	memcpy(cm->red.px, rpx, rnum);
	cm->red.py = Emalloc(rnum);
	memcpy(cm->red.py, rpy, rnum);
     }

   if (gnum < 2)
     {
	cm->green.num = 0;
     }
   else
     {
	cm->green.num = gnum;
	cm->green.px = Emalloc(gnum);
	memcpy(cm->green.px, gpx, gnum);
	cm->green.py = Emalloc(gnum);
	memcpy(cm->green.py, gpy, gnum);
     }

   if (bnum < 2)
     {
	cm->blue.num = 0;
     }
   else
     {
	cm->blue.num = bnum;
	cm->blue.px = Emalloc(bnum);
	memcpy(cm->blue.px, bpx, bnum);
	cm->blue.py = Emalloc(bnum);
	memcpy(cm->blue.py, bpy, bnum);
     }

   CreateCurve(&(cm->red));
   CreateCurve(&(cm->green));
   CreateCurve(&(cm->blue));

   EDBUG_RETURN(cm);
}

void
ModifyCMClass(char *name, int rnum, unsigned char *rpx, unsigned char *rpy,
	      int gnum, unsigned char *gpx, unsigned char *gpy, int bnum,
	      unsigned char *bpx, unsigned char *bpy)
{
   ColorModifierClass *cm;

   EDBUG(5, "ModifyCMCLass");
   cm = (ColorModifierClass *) FindItem(name, 0, LIST_FINDBY_NAME,
					LIST_TYPE_COLORMODIFIER);
   if (!cm)
      EDBUG_RETURN_;

   if (cm->red.px)
      Efree(cm->red.px);
   if (cm->red.py)
      Efree(cm->red.py);
   if (cm->green.px)
      Efree(cm->green.px);
   if (cm->green.py)
      Efree(cm->green.py);
   if (cm->blue.px)
      Efree(cm->blue.px);
   if (cm->blue.py)
      Efree(cm->blue.py);

   cm->red.px = NULL;
   cm->red.py = NULL;
   cm->green.px = NULL;
   cm->green.py = NULL;
   cm->blue.px = NULL;
   cm->blue.py = NULL;

   if (rnum < 2)
     {
	cm->red.num = 0;
     }
   else
     {
	cm->red.num = rnum;
	cm->red.px = Emalloc(rnum);
	memcpy(cm->red.px, rpx, rnum);
	cm->red.py = Emalloc(rnum);
	memcpy(cm->red.py, rpy, rnum);
     }

   if (gnum < 2)
     {
	cm->green.num = 0;
     }
   else
     {
	cm->green.num = gnum;
	cm->green.px = Emalloc(gnum);
	memcpy(cm->green.px, gpx, gnum);
	cm->green.py = Emalloc(gnum);
	memcpy(cm->green.py, gpy, gnum);
     }

   if (bnum < 2)
     {
	cm->blue.num = 0;
     }
   else
     {
	cm->blue.num = bnum;
	cm->blue.px = Emalloc(bnum);
	memcpy(cm->blue.px, bpx, bnum);
	cm->blue.py = Emalloc(bnum);
	memcpy(cm->blue.py, bpy, bnum);
     }

   CreateCurve(&(cm->red));
   CreateCurve(&(cm->green));
   CreateCurve(&(cm->blue));

   EDBUG_RETURN_;
}
