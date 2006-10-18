/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2006 Kim Woelders
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
#include "eimage.h"
#include "tclass.h"
#include "xwin.h"

#if FONT_TYPE_IFT
extern const FontOps FontOpsIft;
#endif

static              GC
_get_gc(Win win)
{
   static GC           gc = None;
   static Visual      *last_vis = NULL;
   Visual             *vis;

   vis = WinGetVisual(win);
   if (vis != last_vis)
     {
	if (gc)
	   EXFreeGC(gc);
	gc = None;
	last_vis = vis;
     }

   if (!gc)
      gc = EXCreateGC(WinGetXwin(win), 0, NULL);

   return gc;
}

static void
TextDrawRotTo(Win win, Drawable src, Drawable dst, int x, int y,
	      int w, int h, TextState * ts)
{
   EImage             *im;
   int                 win_w;

   switch (ts->style.orientation)
     {
     case FONT_TO_UP:
	im = EImageGrabDrawable(src, 0, y, x, h, w, 0);
	EImageOrientate(im, 1);
	EImageRenderOnDrawable(im, win, dst, 0, 0, w, h, 0);
	EImageFree(im);
	break;
     case FONT_TO_DOWN:
	EXGetGeometry(src, NULL, NULL, NULL, &win_w, NULL, NULL, NULL);
	im = EImageGrabDrawable(src, None, win_w - y - h, x, h, w, 0);
	EImageOrientate(im, 3);
	EImageRenderOnDrawable(im, win, dst, 0, 0, w, h, 0);
	EImageFree(im);
	break;
     case FONT_TO_LEFT:	/* Holy carumba! That's for yoga addicts, maybe .... */
	im = EImageGrabDrawable(src, None, x, y, w, h, 0);
	EImageOrientate(im, 2);
	EImageRenderOnDrawable(im, win, dst, 0, 0, w, h, 0);
	EImageFree(im);
	break;
     default:
	break;
     }
}

static void
TextDrawRotBack(Win win, Drawable dst, Drawable src, int x, int y,
		int w, int h, TextState * ts)
{
   EImage             *im;
   int                 win_w;

   switch (ts->style.orientation)
     {
     case FONT_TO_UP:
	im = EImageGrabDrawable(src, None, 0, 0, w, h, 0);
	EImageOrientate(im, 3);
	EImageRenderOnDrawable(im, win, dst, y, x, h, w, 0);
	EImageFree(im);
	break;
     case FONT_TO_DOWN:
	EXGetGeometry(dst, NULL, NULL, NULL, &win_w, NULL, NULL, NULL);
	im = EImageGrabDrawable(src, None, 0, 0, w, h, 0);
	EImageOrientate(im, 1);
	EImageRenderOnDrawable(im, win, dst, win_w - y - h, x, h, w, 0);
	EImageFree(im);
	break;
     case FONT_TO_LEFT:	/* Holy carumba! That's for yoga addicts, maybe .... */
	im = EImageGrabDrawable(src, None, 0, 0, w, h, 0);
	EImageOrientate(im, 2);
	EImageRenderOnDrawable(im, win, dst, x, y, w, h, 0);
	EImageFree(im);
	break;
     default:
	break;
     }
}

#if FONT_TYPE_IFT
static EImage      *
TextImageGet(Win win __UNUSED__, Drawable src, int x, int y, int w, int h,
	     TextState * ts)
{
   EImage             *im;
   int                 win_w;

   switch (ts->style.orientation)
     {
     default:
     case FONT_TO_RIGHT:
	im = EImageGrabDrawable(src, None, x, y, w, h, 0);
	break;
     case FONT_TO_LEFT:
	im = EImageGrabDrawable(src, None, x, y, w, h, 0);
	EImageOrientate(im, 2);
	break;
     case FONT_TO_UP:
	im = EImageGrabDrawable(src, 0, y, x, h, w, 0);
	EImageOrientate(im, 1);
	break;
     case FONT_TO_DOWN:
	EXGetGeometry(src, NULL, NULL, NULL, &win_w, NULL, NULL, NULL);
	im = EImageGrabDrawable(src, None, win_w - y - h, x, h, w, 0);
	EImageOrientate(im, 3);
	break;
     }

   return im;
}

static void
TextImagePut(EImage * im, Win win, Drawable dst, int x, int y,
	     int w, int h, TextState * ts)
{
   int                 win_w;

   switch (ts->style.orientation)
     {
     default:
     case FONT_TO_RIGHT:
	EImageRenderOnDrawable(im, win, dst, x, y, w, h, 0);
	break;
     case FONT_TO_LEFT:
	EImageOrientate(im, 2);
	EImageRenderOnDrawable(im, win, dst, x, y, w, h, 0);
	break;
     case FONT_TO_UP:
	EImageOrientate(im, 3);
	EImageRenderOnDrawable(im, win, dst, y, x, h, w, 0);
	break;
     case FONT_TO_DOWN:
	EXGetGeometry(dst, NULL, NULL, NULL, &win_w, NULL, NULL, NULL);
	EImageOrientate(im, 1);
	EImageRenderOnDrawable(im, win, dst, win_w - y - h, x, h, w, 0);
	break;
     }
   EImageFree(im);
}
#endif /* FONT_TYPE_IFT */

TextState          *
TextclassGetTextState(TextClass * tclass, int state, int active, int sticky)
{
   if (active)
     {
	if (!sticky)
	  {
	     switch (state)
	       {
	       case STATE_NORMAL:
		  return tclass->active.normal;
	       case STATE_HILITED:
		  return tclass->active.hilited;
	       case STATE_CLICKED:
		  return tclass->active.clicked;
	       case STATE_DISABLED:
		  return tclass->active.disabled;
	       default:
		  break;
	       }
	  }
	else
	  {
	     switch (state)
	       {
	       case STATE_NORMAL:
		  return tclass->sticky_active.normal;
	       case STATE_HILITED:
		  return tclass->sticky_active.hilited;
	       case STATE_CLICKED:
		  return tclass->sticky_active.clicked;
	       case STATE_DISABLED:
		  return tclass->sticky_active.disabled;
	       default:
		  break;
	       }

	  }
     }
   else if (sticky)
     {
	switch (state)
	  {
	  case STATE_NORMAL:
	     return tclass->sticky.normal;
	  case STATE_HILITED:
	     return tclass->sticky.hilited;
	  case STATE_CLICKED:
	     return tclass->sticky.clicked;
	  case STATE_DISABLED:
	     return tclass->sticky.disabled;
	  default:
	     break;
	  }
     }
   else
     {
	switch (state)
	  {
	  case STATE_NORMAL:
	     return tclass->norm.normal;
	  case STATE_HILITED:
	     return tclass->norm.hilited;
	  case STATE_CLICKED:
	     return tclass->norm.clicked;
	  case STATE_DISABLED:
	     return tclass->norm.disabled;
	  default:
	     break;
	  }
     }
   return NULL;
}

#if FONT_TYPE_XFS
/*
 * XFontSet - XCreateFontSet
 */
extern const FontOps FontOpsXfs;

static int
XfsLoad(TextState * ts)
{
   int                 i, missing_cnt, font_cnt;
   char              **missing_list, *def_str, **fn;
   XFontStruct       **fs;

   ts->f.xfs.font = XCreateFontSet(disp, ts->fontname, &missing_list,
				   &missing_cnt, &def_str);
   if (missing_cnt)
      XFreeStringList(missing_list);

   if (!ts->f.xfs.font)
     {
	ts->f.xfs.font = XCreateFontSet(disp, "fixed", &missing_list,
					&missing_cnt, &def_str);
	if (missing_cnt)
	   XFreeStringList(missing_list);
     }

   if (!ts->f.xfs.font)
      return -1;

   if (EventDebug(EDBUG_TYPE_FONTS))
     {
	Eprintf("- XBaseFontNameListOfFontSet %s\n",
		XBaseFontNameListOfFontSet(ts->f.xfs.font));
	font_cnt = XFontsOfFontSet(ts->f.xfs.font, &fs, &fn);
	for (i = 0; i < font_cnt; i++)
	   Eprintf("- XFontsOfFontSet %d: %s\n", i, fn[i]);
     }

   ts->f.xfs.ascent = 0;
   font_cnt = XFontsOfFontSet(ts->f.xfs.font, &fs, &fn);
   for (i = 0; i < font_cnt; i++)
      ts->f.xfs.ascent = MAX(fs[i]->ascent, ts->f.xfs.ascent);

   ts->type = FONT_TYPE_XFS;
   ts->ops = &FontOpsXfs;
   return 0;
}

static void
XfsUnload(TextState * ts)
{
   XFreeFontSet(disp, ts->f.xfs.font);
}

static void
XfsTextSize(TextState * ts, const char *text, int len,
	    int *width, int *height, int *ascent)
{
   XRectangle          ret2;

   if (len == 0)
      len = strlen(text);
   XmbTextExtents(ts->f.xfs.font, text, len, NULL, &ret2);
   *height = ret2.height;
   *width = ret2.width;
   *ascent = ts->f.xfs.ascent;
}

static void
XfsTextDraw(TextState * ts, FontDrawContext * fdc, int x, int y,
	    const char *text, int len)
{
   XmbDrawString(disp, fdc->draw, ts->f.xfs.font, fdc->gc, x, y, text, len);
}

static void
XfsFdcInit(TextState * ts __UNUSED__, FontDrawContext * fdc, Win win)
{
   fdc->win = win;
   fdc->gc = _get_gc(win);
}

static void
XfsFdcSetColor(TextState * ts __UNUSED__, FontDrawContext * fdc, XColor * xc)
{
   EAllocColor(WinGetCmap(fdc->win), xc);
   XSetForeground(disp, fdc->gc, xc->pixel);
}

static void
XfsTextMangle(TextState * ts, char **ptext, int *pw, int textwidth_limit)
{
   char               *text = *ptext;
   int                 hh, ascent;
   char               *new_line;
   int                 nuke_count = 0;
   int                 len;
   wchar_t            *wc_line = NULL;
   int                 wc_len;

   len = strlen(text);
   new_line = Emalloc(len + 10);
   if (!new_line)
      return;

   wc_len = mbstowcs(NULL, text, 0);
   if (wc_len > 0)
     {
	wc_line = (wchar_t *) Emalloc((wc_len + 1) * sizeof(wchar_t));
	mbstowcs(wc_line, text, len);
	wc_line[wc_len] = (wchar_t) '\0';
     }

   while (*pw > textwidth_limit)
     {
	nuke_count++;
	if (nuke_count > wc_len)
	  {
	     int                 mlen;

	     new_line[0] = 0;
	     if (MB_CUR_MAX > 1 && wc_len > 0)
	       {		/* if multibyte locale,... */
		  mlen = mblen(text, MB_CUR_MAX);
		  if (mlen < 0)
		     mlen = 1;
	       }
	     else
		mlen = 1;

	     strncat(new_line, text, mlen);
	     strcat(new_line, "...");
	     break;
	  }
	new_line[0] = 0;
	if (MB_CUR_MAX > 1 && wc_len > 0)
	  {
	     int                 j, k, len_mb;

	     for (j = k = 0; k < (wc_len - nuke_count) / 2; k++)
	       {
		  len_mb = wctomb(new_line + j, wc_line[k]);
		  if (len_mb > 0)
		     j += len_mb;
	       }
	     new_line[j] = '\0';
	     strcat(new_line, "...");
	     j += 3;
	     len_mb = wcstombs(new_line + j,
			       wc_line + (wc_len - nuke_count) / 2 +
			       nuke_count, len + 10 - j);
	     if (len_mb > 0)
		j += len_mb;
	     new_line[j] = '\0';
	  }
	else
	  {
	     strncat(new_line, text, (len - nuke_count) / 2);
	     strcat(new_line, "...");
	     strcat(new_line, text + ((len - nuke_count) / 2) + nuke_count);
	  }
	ts->ops->TextSize(ts, new_line, 0, pw, &hh, &ascent);
     }
   Efree(text);
   *ptext = new_line;
   if (wc_line)
      Efree(wc_line);
}

const FontOps       FontOpsXfs = {
   XfsLoad, XfsUnload, XfsTextSize, XfsTextMangle, XfsTextDraw,
   XfsFdcInit, XfsFdcSetColor
};
#endif /* FONT_TYPE_XFS */

#if FONT_TYPE_XFONT
/*
 * XFontStruct - XLoadQueryFont
 */
extern const FontOps FontOpsXfont;

static int
XfontLoad(TextState * ts)
{
   if (strchr(ts->fontname, ',') == NULL)
      ts->f.xf.font = XLoadQueryFont(disp, ts->fontname);
   if (ts->f.xf.font)
      goto done;

   /* This one really should succeed! */
   ts->f.xf.font = XLoadQueryFont(disp, "fixed");
   if (ts->f.xf.font)
      goto done;

   return -1;			/* Failed */

 done:
   ts->type = FONT_TYPE_XFONT;
   ts->ops = &FontOpsXfont;
   return -1;
}

static void
XfontUnload(TextState * ts __UNUSED__)
{
   if (ts->f.xf.font)
      XFreeFont(disp, ts->f.xf.font);
}

static void
XfontTextSize(TextState * ts, const char *text, int len,
	      int *width, int *height, int *ascent)
{
   if (len == 0)
      len = strlen(text);
   if (ts->f.xf.font->min_byte1 == 0 && ts->f.xf.font->max_byte1 == 0)
      *width = XTextWidth(ts->f.xf.font, text, len);
   else
      *width = XTextWidth16(ts->f.xf.font, (XChar2b *) text, len / 2);
   *height = ts->f.xf.font->ascent + ts->f.xf.font->descent;
   *ascent = ts->f.xf.font->ascent;
}

static void
XfontTextDraw(TextState * ts, FontDrawContext * fdc, int x, int y,
	      const char *text, int len)
{
   if (ts->f.xf.font->min_byte1 == 0 && ts->f.xf.font->max_byte1 == 0)
      XDrawString(disp, fdc->draw, fdc->gc, x, y, text, len);
   else
      XDrawString16(disp, fdc->draw, fdc->gc, x, y, (XChar2b *) text, len);
}

static void
XfontFdcInit(TextState * ts __UNUSED__, FontDrawContext * fdc, Win win)
{
   fdc->win = win;
   fdc->gc = _get_gc(win);

   XSetFont(disp, fdc->gc, ts->f.xf.font->fid);
}

static void
XfontFdcSetColor(TextState * ts __UNUSED__, FontDrawContext * fdc, XColor * xc)
{
   EAllocColor(WinGetCmap(fdc->win), xc);
   XSetForeground(disp, fdc->gc, xc->pixel);
}

static void
XfontTextMangle(TextState * ts, char **ptext, int *pw, int textwidth_limit)
{
   char               *text = *ptext;
   int                 hh, ascent;
   char               *new_line;
   int                 nuke_count = 0;
   int                 len;

   if (1)
     {
	len = strlen(text);
	new_line = Emalloc(len + 10);
	if (!new_line)
	   return;

	while (*pw > textwidth_limit)
	  {
	     nuke_count++;
	     if (nuke_count > len)
	       {
		  new_line[0] = 0;
		  strncat(new_line, text, 1);
		  strcat(new_line, "...");
		  break;
	       }
	     new_line[0] = 0;
	     strncat(new_line, text, (len - nuke_count) / 2);
	     strcat(new_line, "...");
	     strcat(new_line, text + ((len - nuke_count) / 2) + nuke_count);
	     ts->ops->TextSize(ts, new_line, 0, pw, &hh, &ascent);
	  }
     }
   else
     {
	len = strlen(text);
	new_line = Emalloc(len + 20);
	if (!new_line)
	   return;

	while (*pw > textwidth_limit)
	  {
	     nuke_count += 2;
	     if (nuke_count > len)
	       {
		  new_line[0] = 0;
		  strncat(new_line, text, 1);
		  strcat(new_line, ". . . ");
		  break;
	       }
	     new_line[0] = 0;
	     strncat(new_line, text, (len - nuke_count) / 4);
	     strcat(new_line, ". . . ");
	     strcat(new_line, text + ((len - nuke_count) / 4) + nuke_count);
	     ts->ops->TextSize(ts, new_line, 0, pw, &hh, &ascent);
	  }
     }
   Efree(text);
   *ptext = new_line;
}

const FontOps       FontOpsXfont = {
   XfontLoad, XfontUnload, XfontTextSize, XfontTextMangle, XfontTextDraw,
   XfontFdcInit, XfontFdcSetColor
};
#endif /* FONT_TYPE_XFONT */

#if FONT_TYPE_IFT
static void
IftTextMangle(TextState * ts, char **ptext, int *pw, int textwidth_limit)
{
   char               *text = *ptext;
   int                 hh, ascent;
   char               *new_line;
   int                 nuke_count = 0;
   int                 len;

   len = strlen(text);
   new_line = Emalloc(len + 10);
   if (!new_line)
      return;

   while (*pw > textwidth_limit)
     {
	nuke_count++;
	if (nuke_count > len)
	  {
	     new_line[0] = 0;
	     strncat(new_line, text, 1);
	     strcat(new_line, "...");
	     break;
	  }
	new_line[0] = 0;
	strncat(new_line, text, (len - nuke_count) / 2);
	strcat(new_line, "...");
	strcat(new_line, text + ((len - nuke_count) / 2) + nuke_count);
	ts->ops->TextSize(ts, new_line, 0, pw, &hh, &ascent);
     }
   Efree(text);
   *ptext = new_line;
}
#endif /* FONT_TYPE_IFT */

static void
TsTextDraw(TextState * ts, FontDrawContext * fdc, int x, int y,
	   const char *text, int len)
{
   if (ts->style.effect == 1)
     {
	ts->ops->FdcSetColor(ts, fdc, &(ts->bg_col));
	ts->ops->TextDraw(ts, fdc, x + 1, y + 1, text, len);
     }
   else if (ts->style.effect == 2)
     {
	ts->ops->FdcSetColor(ts, fdc, &(ts->bg_col));
	ts->ops->TextDraw(ts, fdc, x - 1, y, text, len);
	ts->ops->TextDraw(ts, fdc, x + 1, y, text, len);
	ts->ops->TextDraw(ts, fdc, x, y - 1, text, len);
	ts->ops->TextDraw(ts, fdc, x, y + 1, text, len);
     }
   ts->ops->FdcSetColor(ts, fdc, &(ts->fg_col));
   ts->ops->TextDraw(ts, fdc, x, y, text, len);
}

static void
TextStateLoadFont(TextState * ts)
{
   if (!ts->fontname)
      return;

   /* Quit if already done */
   if (ts->type)
      return;

   ts->need_utf8 = Mode.locale.utf8_int;

#if FONT_TYPE_IFT
   if (!FontOpsIft.Load(ts))	/* Imlib2/FreeType */
      goto done;
#endif
#if FONT_TYPE_XFS
   if (!FontOpsXfs.Load(ts))	/* XFontSet - XCreateFontSet */
      goto done;
#endif
#if FONT_TYPE_XFONT
   if (!FontOpsXfont.Load(ts))	/* XFontStruct - XLoadQueryFont */
      goto done;
#endif

 done:
   if (EventDebug(EDBUG_TYPE_FONTS))
      Eprintf("TextStateLoadFont %s: type=%d\n", ts->fontname, ts->type);
   return;
}

void
TextSize(TextClass * tclass, int active, int sticky, int state,
	 const char *text, int *width, int *height, int fsize __UNUSED__)
{
   const char         *str;
   char              **lines;
   int                 i, num_lines, ww, hh, asc;
   TextState          *ts;

   *width = 0;
   *height = 0;

   if (!text)
      return;

   ts = TextclassGetTextState(tclass, state, active, sticky);
   if (!ts)
      return;

   TextStateLoadFont(ts);
   if (!ts->ops)
      return;

   /* Do encoding conversion, if necessary */
   str = EstrInt2Enc(text, ts->need_utf8);
   lines = StrlistFromString(str, '\n', &num_lines);
   EstrInt2EncFree(str, ts->need_utf8);
   if (!lines)
      return;

   for (i = 0; i < num_lines; i++)
     {
	ts->ops->TextSize(ts, lines[i], strlen(lines[i]), &ww, &hh, &asc);
	*height += hh;
	if (ww > *width)
	   *width = ww;
     }

   StrlistFree(lines, num_lines);
}

void
TextstateDrawText(TextState * ts, Win win, Drawable draw, const char *text,
		  int x, int y, int w, int h, const EImageBorder * pad,
		  int fsize __UNUSED__, int justification)
{
   const char         *str;
   char              **lines;
   int                 i, num_lines;
   int                 textwidth_limit, offset_x, offset_y;
   int                 xx, yy, ww, hh, ascent;
   Pixmap              drawable;
   FontDrawContext     fdc;

   if (w <= 0 || h <= 0)
      return;

   TextStateLoadFont(ts);
   if (!ts->ops)
      return;

   /* Do encoding conversion, if necessary */
   str = EstrInt2Enc(text, ts->need_utf8);
   lines = StrlistFromString(str, '\n', &num_lines);
   EstrInt2EncFree(str, ts->need_utf8);
   if (!lines)
      return;

   if (draw == None)
      draw = WinGetXwin(win);

   if (ts->style.orientation == FONT_TO_RIGHT ||
       ts->style.orientation == FONT_TO_LEFT)
     {
	if (pad)
	  {
	     x += pad->left;
	     w -= pad->left + pad->right;
	     y += pad->top;
	     h -= pad->top + pad->bottom;
	  }
	textwidth_limit = w;
     }
   else
     {
	if (pad)
	  {
	     x += pad->left;
	     h -= pad->left + pad->right;
	     y += pad->top;
	     w -= pad->top + pad->bottom;
	  }
	textwidth_limit = h;
     }

#if 0
   Eprintf("TextstateDrawText %d,%d %dx%d(%d): %s\n", x, y, w, h,
	   textwidth_limit, text);
#endif

   xx = x;
   yy = y;

#if FONT_TYPE_IFT
   if (ts->type == FONT_TYPE_IFT)
     {
	ts->ops->FdcInit(ts, &fdc, win);

	for (i = 0; i < num_lines; i++)
	  {
	     ts->ops->TextSize(ts, lines[i], 0, &ww, &hh, &ascent);
	     if (ww > textwidth_limit)
#if 0
		ts->ops->TextMangle(ts, &lines[i], &ww, textwidth_limit);
#else
		IftTextMangle(ts, &lines[i], &ww, textwidth_limit);
#endif

	     if (i == 0)
		yy += ascent;
	     xx = x + (((textwidth_limit - ww) * justification) >> 10);

	     fdc.im = TextImageGet(win, draw, xx - 1, yy - 1 - ascent,
				   ww + 2, hh + 2, ts);
	     if (!fdc.im)
		break;

	     offset_x = 1;
	     offset_y = ascent + 1;

	     TsTextDraw(ts, &fdc, offset_x, offset_y, lines[i],
			strlen(lines[i]));

	     TextImagePut(fdc.im, win, draw, xx - 1, yy - 1 - ascent,
			  ww + 2, hh + 2, ts);

	     yy += hh;
	  }
     }
   else
#endif /* FONT_TYPE_IFT */
     {
	ts->ops->FdcInit(ts, &fdc, win);

	for (i = 0; i < num_lines; i++)
	  {
	     ts->ops->TextSize(ts, lines[i], 0, &ww, &hh, &ascent);
	     if (ww > textwidth_limit)
		ts->ops->TextMangle(ts, &lines[i], &ww, textwidth_limit);

	     if (i == 0)
		yy += ascent;
	     xx = x + (((textwidth_limit - ww) * justification) >> 10);

	     if (ts->style.orientation != FONT_TO_RIGHT)
		drawable = ECreatePixmap(win, ww + 2, hh + 2, 0);
	     else
		drawable = draw;
	     fdc.draw = drawable;
	     TextDrawRotTo(win, draw, drawable, xx - 1, yy - 1 - ascent,
			   ww + 2, hh + 2, ts);

	     if (ts->style.orientation == FONT_TO_RIGHT)
	       {
		  offset_x = xx;
		  offset_y = yy;
	       }
	     else
	       {
		  offset_x = 1;
		  offset_y = ascent + 1;
	       }

	     TsTextDraw(ts, &fdc, offset_x, offset_y, lines[i],
			strlen(lines[i]));

	     TextDrawRotBack(win, draw, drawable, xx - 1, yy - 1 - ascent,
			     ww + 2, hh + 2, ts);
	     if (drawable != draw)
		EFreePixmap(drawable);

	     yy += hh;
	  }
     }

   StrlistFree(lines, num_lines);
}

void
TextDraw(TextClass * tclass, Win win, Drawable draw, int active, int sticky,
	 int state, const char *text, int x, int y, int w, int h, int fsize,
	 int justification)
{
   TextState          *ts;

   if (!tclass || !text)
      return;

   ts = TextclassGetTextState(tclass, state, active, sticky);
   if (!ts)
      return;

   TextstateDrawText(ts, win, draw, text, x, y, w, h, NULL, fsize,
		     justification);
}
