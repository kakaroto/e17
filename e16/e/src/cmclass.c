
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
	char                stuff[255];

	Esnprintf(stuff, sizeof(stuff), "Error : still have %u references\n",
		  cm->ref_count);
	DIALOG_OK("ColorModClass Error", stuff);

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
CreateCMClass(char *name,
	      int rnum, unsigned char *rpx, unsigned char *rpy,
	      int gnum, unsigned char *gpx, unsigned char *gpy,
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
ModifyCMClass(char *name,
	      int rnum, unsigned char *rpx, unsigned char *rpy,
	      int gnum, unsigned char *gpx, unsigned char *gpy,
	      int bnum, unsigned char *bpx, unsigned char *bpy)
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
