/*
 * Copyright (C) 2000-2005 Carsten Haitzler, Geoff Harrison and various contributors
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
#include "conf.h"

#if ENABLE_COLOR_MODIFIERS

void
CreateCurve(ModCurve * c)
{
   int                 i, j, cx, v1, v2, val, dist;

   if (!c)
      return;

   if (c->num == 0)
     {
	for (i = 0; i < 256; i++)
	   c->map[i] = i;
	return;
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
}

void
FreeModCurve(ModCurve * c)
{
   if (!c)
      return;

   Efree(c->px);
   Efree(c->py);
}

void
FreeCMClass(ColorModifierClass * cm)
{
   if (!cm)
      return;

   if (cm->ref_count > 0)
     {
	DialogOK(_("ColorModClass Error!"), _("%u references remain\n"),
		 cm->ref_count);
	return;
     }

   while (RemoveItemByPtr(cm, LIST_TYPE_COLORMODIFIER));

   Efree(cm->name);
   FreeModCurve(&(cm->red));
   FreeModCurve(&(cm->green));
   FreeModCurve(&(cm->blue));
}

ColorModifierClass *
CreateCMClass(char *name, int rnum, unsigned char *rpx,
	      unsigned char *rpy, int gnum,
	      unsigned char *gpx, unsigned char *gpy,
	      int bnum, unsigned char *bpx, unsigned char *bpy)
{
   ColorModifierClass *cm;

   cm = Emalloc(sizeof(ColorModifierClass));
   if (!cm)
      return NULL;

   cm->name = Estrdup(name);
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

   return cm;
}

int
ColorModifierConfigLoad(FILE * fs)
{
   int                 err = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   char               *name = NULL;
   const char         *params = NULL;
   const char         *current_param = NULL;
   unsigned char      *rx = NULL;
   unsigned char      *ry = NULL;
   unsigned char      *gx = NULL;
   unsigned char      *gy = NULL;
   unsigned char      *bx = NULL;
   unsigned char      *by = NULL;
   int                 i = 0, tx, ty;
   int                 rnum = 0, gnum = 0, bnum = 0;
   ColorModifierClass *cm;
   int                 fields;

   while (GetLine(s, sizeof(s), fs))
     {
	s2[0] = 0;
	i = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	  {
	     i1 = CONFIG_INVALID;
	  }
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
	       {
		  RecoverUserConfig();
		  Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	       }
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  RecoverUserConfig();
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
	       }
	  }
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     cm = FindItem(name, 0, LIST_FINDBY_NAME, LIST_TYPE_COLORMODIFIER);
	     if (cm)
	       {
		  ModifyCMClass(name, rnum, rx, ry, gnum, gx, gy, bnum, bx, by);
	       }
	     else
	       {
		  cm = CreateCMClass(name, rnum, rx, ry, gnum, gx, gy, bnum,
				     bx, by);
		  AddItem(cm, cm->name, 0, LIST_TYPE_COLORMODIFIER);
	       }
	     goto done;
	  case CONFIG_CLASSNAME:
	     if (ConfigSkipIfExists(fs, s2, LIST_TYPE_COLORMODIFIER))
		goto done;
	     name = Estrdup(s2);
	     break;
	  case COLORMOD_RED:
	     params = atword(s, 2);
	     current_param = params;
	     if (!current_param)
		goto done;
	     do
	       {
		  while (*current_param == ' ')
		     current_param++;
		  if (rx)
		    {
		       rx = Erealloc(rx, sizeof(unsigned char) * (i + 1));
		       ry = Erealloc(ry, sizeof(unsigned char) * (i + 1));
		    }
		  else
		    {
		       rx = Emalloc(sizeof(unsigned char));
		       ry = Emalloc(sizeof(unsigned char));
		    }
		  if (strstr(current_param, ","))
		     *(strstr(current_param, ",")) = ' ';
		  sscanf(current_param, "%i %i", &tx, &ty);
		  rx[i] = (unsigned char)tx;
		  ry[i++] = (unsigned char)ty;
		  current_param = strstr(current_param, " ") + 1;
		  current_param = strstr(current_param, " ");
	       }
	     while ((current_param)
		    && (current_param = strstr(current_param, " "))
		    && (current_param));
	     rnum = i;
	     break;
	  case COLORMOD_GREEN:
	     params = atword(s, 2);
	     current_param = params;
	     if (!current_param)
		goto done;
	     do
	       {
		  while (*current_param == ' ')
		     current_param++;
		  if (gx)
		    {
		       gx = Erealloc(gx, sizeof(unsigned char) * (i + 1));
		       gy = Erealloc(gy, sizeof(unsigned char) * (i + 1));
		    }
		  else
		    {
		       gx = Emalloc(sizeof(unsigned char));
		       gy = Emalloc(sizeof(unsigned char));
		    }
		  if (strstr(current_param, ","))
		     *(strstr(current_param, ",")) = ' ';
		  sscanf(current_param, "%i %i", &tx, &ty);
		  gx[i] = (unsigned char)tx;
		  gy[i++] = (unsigned char)ty;
		  current_param = strstr(current_param, " ") + 1;
		  current_param = strstr(current_param, " ");
	       }
	     while ((current_param)
		    && (current_param = strstr(current_param, " "))
		    && (current_param));
	     gnum = i;
	     break;
	  case COLORMOD_BLUE:
	     params = atword(s, 2);
	     current_param = params;
	     if (!current_param)
		goto done;
	     do
	       {
		  while (*current_param == ' ')
		     current_param++;
		  if (bx)
		    {
		       bx = Erealloc(bx, sizeof(unsigned char) * (i + 1));
		       by = Erealloc(by, sizeof(unsigned char) * (i + 1));
		    }
		  else
		    {
		       bx = Emalloc(sizeof(unsigned char));
		       by = Emalloc(sizeof(unsigned char));
		    }
		  if (strstr(current_param, ","))
		     *(strstr(current_param, ",")) = ' ';
		  sscanf(current_param, "%i %i", &tx, &ty);
		  bx[i] = (unsigned char)tx;
		  by[i++] = (unsigned char)ty;
		  current_param = strstr(current_param, " ") + 1;
		  current_param = strstr(current_param, " ");
	       }
	     while ((current_param)
		    && (current_param = strstr(current_param, " "))
		    && (current_param));
	     bnum = i;
	     break;
	  default:
	     RecoverUserConfig();
	     Alert(_("Warning: unable to determine what to do with\n"
		     "the following text in the middle of current "
		     " ColorModifier definition:\n"
		     "%s\nWill ignore and continue...\n"), s);
	     break;
	  }
     }
   err = -1;

 done:
   if (name)
      Efree(name);
   if (rx)
      Efree(rx);
   if (ry)
      Efree(ry);
   if (gx)
      Efree(gx);
   if (gy)
      Efree(gy);
   if (bx)
      Efree(bx);
   if (by)
      Efree(by);

   return err;
}

void
ModifyCMClass(char *name, int rnum, unsigned char *rpx, unsigned char *rpy,
	      int gnum, unsigned char *gpx, unsigned char *gpy, int bnum,
	      unsigned char *bpx, unsigned char *bpy)
{
   ColorModifierClass *cm;

   cm = FindItem(name, 0, LIST_FINDBY_NAME, LIST_TYPE_COLORMODIFIER);
   if (!cm)
      return;

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
}

#if 0				/* Unused */

static void
IPC_ColorModifierClass(const char *params, Client * c)
{
   char                buf[FILEPATH_LEN_MAX];

   buf[0] = 0;

   if (params)
     {
	char                param1[FILEPATH_LEN_MAX];
	char                param2[FILEPATH_LEN_MAX];
	char                param3[FILEPATH_LEN_MAX];

	param1[0] = 0;
	param2[0] = 0;
	param3[0] = 0;

	word(params, 1, param1);
	word(params, 2, param2);
	if (param2[0])
	  {
	     if (!strcmp(param1, "create"))
	       {
	       }
	     else if (!strcmp(param2, "delete"))
	       {
		  ColorModifierClass *cm;

		  cm = FindItem(param1, 0, LIST_FINDBY_NAME,
				LIST_TYPE_COLORMODIFIER);
		  if (cm)
		     FreeCMClass(cm);
	       }
	     else if (!strcmp(param2, "modify"))
	       {
	       }
	     else if (!strcmp(param2, "ref_count"))
	       {
		  ColorModifierClass *cm;

		  cm = FindItem(param1, 0, LIST_FINDBY_NAME,
				LIST_TYPE_COLORMODIFIER);
		  if (cm)
		     Esnprintf(buf, sizeof(buf), "%u references remain.",
			       cm->ref_count);
	       }
	     else
	       {
		  Esnprintf(buf, sizeof(buf),
			    "Error: unknown operation specified");
	       }
	  }
	else
	  {
	     Esnprintf(buf, sizeof(buf), "Error: no class specified");
	  }
     }
   else
     {
	Esnprintf(buf, sizeof(buf), "Error: no operation specified");
     }

   if (buf[0])
      CommsSend(c, buf);
}

static void
IPC_ColormodifierDelete(const char *params, Client * c __UNUSED__)
{
   char                param1[FILEPATH_LEN_MAX], buf[FILEPATH_LEN_MAX];
   ColorModifierClass *cm;

   if (params == NULL)
      return;

   sscanf(params, "%1000s", param1);
   cm = FindItem(param1, 0, LIST_FINDBY_NAME, LIST_TYPE_COLORMODIFIER);
   Esnprintf(buf, sizeof(buf), "(null)");
   if (cm)
      FreeCMClass(cm);
}

static void
IPC_ColormodifierGet(const char *params, Client * c)
{
   char                param1[FILEPATH_LEN_MAX];
   char                buf[FILEPATH_LEN_MAX], buf2[FILEPATH_LEN_MAX];
   ColorModifierClass *cm;
   int                 i;

   if (params == NULL)
      return;

   sscanf(params, "%1000s", param1);
   cm = FindItem(param1, 0, LIST_FINDBY_NAME, LIST_TYPE_COLORMODIFIER);
   Esnprintf(buf, sizeof(buf), "(null)");
   if (cm)
     {
	Esnprintf(buf, sizeof(buf), "%i", (int)(cm->red.num));
	for (i = 0; i < cm->red.num; i++)
	  {
	     Esnprintf(buf2, sizeof(buf2), " %i", (int)(cm->red.px[i]));
	     strcat(buf, buf2);
	     Esnprintf(buf2, sizeof(buf2), " %i", (int)(cm->red.py[i]));
	     strcat(buf, buf2);
	  }
	Esnprintf(buf2, sizeof(buf2), "\n%i", (int)(cm->green.num));
	strcat(buf, buf2);
	for (i = 0; i < cm->green.num; i++)
	  {
	     Esnprintf(buf2, sizeof(buf2), " %i", (int)(cm->green.px[i]));
	     strcat(buf, buf2);
	     Esnprintf(buf2, sizeof(buf2), " %i", (int)(cm->green.py[i]));
	     strcat(buf, buf2);
	  }
	Esnprintf(buf2, sizeof(buf2), "\n%i", (int)(cm->red.num));
	strcat(buf, buf2);
	for (i = 0; i < cm->blue.num; i++)
	  {
	     Esnprintf(buf2, sizeof(buf2), " %i", (int)(cm->blue.px[i]));
	     strcat(buf, buf2);
	     Esnprintf(buf2, sizeof(buf2), " %i", (int)(cm->blue.py[i]));
	     strcat(buf, buf2);
	  }
     }
   CommsSend(c, buf);
}

static void
IPC_ColormodifierSet(const char *params, Client * c __UNUSED__)
{
   char                w[FILEPATH_LEN_MAX];
   ColorModifierClass *cm;
   int                 i, j, k;
   char               *name;
   int                 rnum = 0, gnum = 0, bnum = 0;
   unsigned char      *rpx = NULL, *rpy = NULL;
   unsigned char      *gpx = NULL, *gpy = NULL;
   unsigned char      *bpx = NULL, *bpy = NULL;

   if (params == NULL)
      return;

   sscanf(params, "%1000s", w);
   cm = FindItem(w, 0, LIST_FINDBY_NAME, LIST_TYPE_COLORMODIFIER);
   name = Estrdup(w);
   i = 2;
   word(params, i++, w);
   rnum = atoi(w);
   j = 0;
   rpx = Emalloc(rnum);
   rpy = Emalloc(rnum);
   while (j < rnum)
     {
	word(params, i++, w);
	k = atoi(w);
	rpx[j] = k;
	word(params, i++, w);
	k = atoi(w);
	rpy[j++] = k;
     }
   word(params, i++, w);
   gnum = atoi(w);
   j = 0;
   gpx = Emalloc(gnum);
   gpy = Emalloc(gnum);
   while (j < gnum)
     {
	word(params, i++, w);
	k = atoi(w);
	gpx[j] = k;
	word(params, i++, w);
	k = atoi(w);
	gpy[j++] = k;
     }
   word(params, i++, w);
   bnum = atoi(w);
   j = 0;
   bpx = Emalloc(bnum);
   bpy = Emalloc(bnum);
   while (j < bnum)
     {
	word(params, i++, w);
	k = atoi(w);
	bpx[j] = k;
	word(params, i++, w);
	k = atoi(w);
	bpy[j++] = k;
     }
   if (cm)
      ModifyCMClass(name, rnum, rpx, rpy, gnum, gpx, gpy, bnum, bpx, bpy);
   else
     {
	cm = CreateCMClass(name, rnum, rpx, rpy, gnum, gpx, gpy, bnum, bpx,
			   bpy);
	AddItem(cm, cm->name, 0, LIST_TYPE_COLORMODIFIER);
     }
   Efree(name);
   if (rpx)
      Efree(rpx);
   if (rpy)
      Efree(rpy);
   if (gpx)
      Efree(gpx);
   if (gpy)
      Efree(gpy);
   if (bpx)
      Efree(bpx);
   if (bpy)
      Efree(bpy);
}

IpcItem             CmClassIpcArray[] = {
   {
    IPC_ColorModifierClass,
    "colormod", NULL,
    "Create/Delete/Modify a ColorModifierClass",
    "This doesn't do anything yet."}
   ,
   {
    IPC_ColormodifierDelete, "del_colmod", NULL, "TBD", NULL}
   ,
   {
    IPC_ColormodifierGet, "get_colmod", NULL, "TBD", NULL}
   ,
   {
    IPC_ColormodifierSet, "set_colmod", NULL, "TBD", NULL}
   ,
};
#define N_IPC_FUNCS (sizeof(CmClassIpcArray)/sizeof(IpcItem))

#endif

#endif /* ENABLE_COLOR_MODIFIERS */
