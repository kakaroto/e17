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
#if FONT_TYPE_XFT
#include <X11/extensions/Xrender.h>
#include <X11/Xft/Xft.h>
#endif

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

static void
TextstateTextFit1(TextState * ts, char **ptext, int *pw, int textwidth_limit)
{
   char               *text = *ptext;
   int                 hh, ascent;
   char               *new_line;
   int                 nuke_count = 0, nc2;
   int                 len;

   len = strlen(text);
   if (len <= 1)
      return;
   new_line = Emalloc(len + 10);
   if (!new_line)
      return;

   while (*pw > textwidth_limit)
     {
	nuke_count++;

	if (nuke_count >= len - 1)
	  {
	     new_line[0] = text[0];
	     memcpy(new_line + 1, "...", 4);
	     break;
	  }

	nc2 = (len - nuke_count) / 2;
	memcpy(new_line, text, nc2);
	memcpy(new_line + nc2, "...", 3);
	strcpy(new_line + nc2 + 3, text + nc2 + nuke_count);

	ts->ops->TextSize(ts, new_line, 0, pw, &hh, &ascent);
     }

   Efree(text);
   *ptext = new_line;
}

#if FONT_TYPE_XFONT
static void
TextstateTextFit2(TextState * ts, char **ptext, int *pw, int textwidth_limit)
{
   char               *text = *ptext;
   int                 hh, ascent;
   char               *new_line;
   int                 nuke_count = 0;
   int                 len;

   len = strlen(text);
   new_line = Emalloc(len + 20);
   if (!new_line)
      return;

   while (*pw > textwidth_limit)
     {
	nuke_count += 2;

	if (nuke_count > len)
	  {
	     memcpy(new_line, text, 2);
	     memcpy(new_line + 2, ". . . ", 7);
	     break;
	  }

	new_line[0] = 0;
	strncat(new_line, text, (len - nuke_count) / 4);
	strcat(new_line, ". . . ");
	strcat(new_line, text + ((len - nuke_count) / 4) + nuke_count);

	ts->ops->TextSize(ts, new_line, 0, pw, &hh, &ascent);
     }

   Efree(text);
   *ptext = new_line;
}
#endif /* FONT_TYPE_XFONT */

static void
TextstateTextFitMB(TextState * ts, char **ptext, int *pw, int textwidth_limit)
{
   char               *text = *ptext;
   int                 hh, ascent;
   char               *new_line;
   int                 nuke_count = 0, nc2;
   int                 len;
   wchar_t            *wc_line = NULL;
   int                 wc_len;

   if (EwcOpen(ts->need_utf8 || Mode.locale.utf8_int))
      return;

   len = strlen(text);
   wc_len = EwcStrToWcs(text, len, NULL, 0);
   if (wc_len <= 1)
      goto done;

   wc_line = (wchar_t *) Emalloc((wc_len + 1) * sizeof(wchar_t));
   if (!wc_line)
      goto done;

   if (EwcStrToWcs(text, len, wc_line, wc_len) <= 0)
      goto done;

   new_line = Emalloc(len + 10);
   if (!new_line)
      goto done;

   while (*pw > textwidth_limit)
     {
	nuke_count++;
	int                 len_mb;

	if (nuke_count >= wc_len - 1)
	  {
	     int                 mlen;

	     mlen = EwcWcsToStr(wc_line, 1, new_line, MB_CUR_MAX);
	     if (mlen < 0)
		mlen = 1;

	     strcpy(new_line + mlen, "...");
	     break;
	  }

	nc2 = (wc_len - nuke_count) / 2;
	len_mb = EwcWcsToStr(wc_line, nc2, new_line, len + 10);
	memcpy(new_line + len_mb, "...", 3);
	len_mb += 3;
	len_mb += EwcWcsToStr(wc_line + nc2 + nuke_count,
			      wc_len - nc2 - nuke_count,
			      new_line + len_mb, len + 10 - len_mb);
	new_line[len_mb] = '\0';

	ts->ops->TextSize(ts, new_line, 0, pw, &hh, &ascent);
     }
   Efree(text);
   *ptext = new_line;
 done:
   if (wc_line)
      Efree(wc_line);
   EwcClose();
}

#if FONT_TYPE_XFT
/*
 * Xft
 */
extern const FontOps FontOpsXft;

typedef struct
{
   XftFont            *font;
   Win                 win;
   Drawable            draw;
   XftDraw            *xftd;
   XftColor            xftc;
} FontCtxXft;

static int
_xft_Load(TextState * ts, int fallback __UNUSED__)
{
   XftFont            *font;
   FontCtxXft         *fdc;

   if (strchr(ts->fontname, '/'))
      return -1;

   if (ts->fontname[0] == '-')
      font = XftFontOpenXlfd(disp, VRoot.scr, ts->fontname);
   else
      font = XftFontOpenName(disp, VRoot.scr, ts->fontname);

   if (!font)
      return -1;

#if 0				/* Debug */
   {
      FT_Face             ftf = XftLockFace(font);

      if (ftf == NULL)
	 return -1;
      Eprintf("Font %s family_name=%s style_name=%s\n", ts->fontname,
	      ftf->family_name, ftf->style_name);
      XftUnlockFace(font);
   }
#endif

   fdc = Emalloc(sizeof(FontCtxXft));
   if (!fdc)
      return -1;
   fdc->font = font;
   ts->fdc = fdc;
   ts->need_utf8 = 1;
   ts->type = FONT_TYPE_XFT;
   ts->ops = &FontOpsXft;
   return 0;
}

static void
_xft_Unload(TextState * ts)
{
   FontCtxXft         *fdc = (FontCtxXft *) ts->fdc;

   XftFontClose(disp, fdc->font);
}

static void
_xft_TextSize(TextState * ts, const char *text, int len,
	      int *width, int *height, int *ascent)
{
   FontCtxXft         *fdc = (FontCtxXft *) ts->fdc;
   XGlyphInfo          gi;

   if (len == 0)
      len = strlen(text);
   XftTextExtentsUtf8(disp, fdc->font, (const XftChar8 *)text, len, &gi);
   *width = gi.xOff;
   *height = fdc->font->height;
   *ascent = fdc->font->ascent;
#if 0
   Eprintf("asc/dsc/h=%d/%d/%d x,y=%2d,%d wxh=%dx%d ox,y=%3d,%d: (%d)%s\n",
	   fdc->font->ascent, fdc->font->descent, fdc->font->height, gi.x, gi.y,
	   gi.width, gi.height, gi.xOff, gi.yOff, len, text);
#endif
}

static void
_xft_TextDraw(TextState * ts, int x, int y, const char *text, int len)
{
   FontCtxXft         *fdc = (FontCtxXft *) ts->fdc;

   XftDrawStringUtf8(fdc->xftd, &(fdc->xftc), fdc->font, x, y,
		     (const XftChar8 *)text, len);
}

static int
_xft_FdcInit(TextState * ts, Win win, Drawable draw)
{
   FontCtxXft         *fdc = (FontCtxXft *) ts->fdc;

   fdc->win = win;

   fdc->xftd = XftDrawCreate(disp, draw, WinGetVisual(win), WinGetCmap(win));
   if (!fdc->xftd)
      return -1;
   return 0;
}

static void
_xft_FdcFini(TextState * ts)
{
   FontCtxXft         *fdc = (FontCtxXft *) ts->fdc;

   XftDrawDestroy(fdc->xftd);
}

static void
_xft_FdcSetDrawable(TextState * ts, unsigned long draw)
{
   FontCtxXft         *fdc = (FontCtxXft *) ts->fdc;

   if (fdc->draw == draw)
      return;
   XftDrawChange(fdc->xftd, draw);
}

static void
_xft_FdcSetColor(TextState * ts, XColor * xc)
{
   FontCtxXft         *fdc = (FontCtxXft *) ts->fdc;
   XRenderColor        xrc;

   xrc.red = xc->red * 256;
   xrc.green = xc->green * 256;
   xrc.blue = xc->blue * 256;
   xrc.alpha = 65535;

   XftColorAllocValue(disp, WinGetVisual(fdc->win), WinGetCmap(fdc->win),
		      &xrc, &(fdc->xftc));
}

const FontOps       FontOpsXft = {
   _xft_Load, _xft_Unload, _xft_TextSize, TextstateTextFit, _xft_TextDraw,
   _xft_FdcInit, _xft_FdcFini, _xft_FdcSetDrawable, _xft_FdcSetColor
};
#endif /* FONT_TYPE_XFT */

#if FONT_TYPE_XFS
/*
 * XFontSet - XCreateFontSet
 */
extern const FontOps FontOpsXfs;

typedef struct
{
   XFontSet            font;
   int                 ascent;
   Win                 win;
   Drawable            draw;
   GC                  gc;
} FontCtxXfs;

static int
_xfs_Load(TextState * ts, int fallback)
{
   XFontSet            font;
   FontCtxXfs         *fdc;
   int                 i, missing_cnt, font_cnt;
   char              **missing_list, *def_str, **fn;
   XFontStruct       **fs;

   font = XCreateFontSet(disp, ts->fontname, &missing_list,
			 &missing_cnt, &def_str);
   if (missing_cnt)
      XFreeStringList(missing_list);

   if (!font && fallback)
     {
	font = XCreateFontSet(disp, "fixed", &missing_list,
			      &missing_cnt, &def_str);
	if (missing_cnt)
	   XFreeStringList(missing_list);
     }

   if (!font)
      return -1;

   if (EventDebug(EDBUG_TYPE_FONTS))
     {
	Eprintf("- XBaseFontNameListOfFontSet %s\n",
		XBaseFontNameListOfFontSet(font));
	font_cnt = XFontsOfFontSet(font, &fs, &fn);
	for (i = 0; i < font_cnt; i++)
	   Eprintf("- XFontsOfFontSet %d: %s\n", i, fn[i]);
     }

   fdc = Emalloc(sizeof(FontCtxXfs));
   if (!fdc)
      return -1;
   fdc->font = font;
   ts->fdc = fdc;
   fdc->ascent = 0;
   font_cnt = XFontsOfFontSet(font, &fs, &fn);
   for (i = 0; i < font_cnt; i++)
      fdc->ascent = MAX(fs[i]->ascent, fdc->ascent);
   ts->type = FONT_TYPE_XFS;
   ts->ops = &FontOpsXfs;
   return 0;
}

static void
_xfs_Unload(TextState * ts)
{
   FontCtxXfs         *fdc = (FontCtxXfs *) ts->fdc;

   XFreeFontSet(disp, fdc->font);
}

static void
_xfs_TextSize(TextState * ts, const char *text, int len,
	      int *width, int *height, int *ascent)
{
   FontCtxXfs         *fdc = (FontCtxXfs *) ts->fdc;
   XRectangle          ret2;

   if (len == 0)
      len = strlen(text);
   XmbTextExtents(fdc->font, text, len, NULL, &ret2);
   *height = ret2.height;
   *width = ret2.width;
   *ascent = fdc->ascent;
}

static void
_xfs_TextDraw(TextState * ts, int x, int y, const char *text, int len)
{
   FontCtxXfs         *fdc = (FontCtxXfs *) ts->fdc;

   XmbDrawString(disp, fdc->draw, fdc->font, fdc->gc, x, y, text, len);
}

static int
_xfs_FdcInit(TextState * ts, Win win, Drawable draw)
{
   FontCtxXfs         *fdc = (FontCtxXfs *) ts->fdc;

   fdc->win = win;
   fdc->draw = draw;
   fdc->gc = _get_gc(win);
   return 0;
}

static void
_xfs_FdcSetDrawable(TextState * ts, unsigned long draw)
{
   FontCtxXfs         *fdc = (FontCtxXfs *) ts->fdc;

   fdc->draw = draw;
}

static void
_xfs_FdcSetColor(TextState * ts, XColor * xc)
{
   FontCtxXfs         *fdc = (FontCtxXfs *) ts->fdc;

   EAllocColor(WinGetCmap(fdc->win), xc);
   XSetForeground(disp, fdc->gc, xc->pixel);
}

const FontOps       FontOpsXfs = {
   _xfs_Load, _xfs_Unload, _xfs_TextSize, TextstateTextFit, _xfs_TextDraw,
   _xfs_FdcInit, NULL, _xfs_FdcSetDrawable, _xfs_FdcSetColor
};
#endif /* FONT_TYPE_XFS */

#if FONT_TYPE_XFONT
/*
 * XFontStruct - XLoadQueryFont
 */
extern const FontOps FontOpsXfont;

typedef struct
{
   XFontStruct        *font;
   Win                 win;
   Drawable            draw;
   GC                  gc;
} FontCtxXfont;

static int
_xfont_Load(TextState * ts, int fallback __UNUSED__)
{
   XFontStruct        *font = NULL;
   FontCtxXfont       *fdc;

   if (strchr(ts->fontname, ',') == NULL)
      font = XLoadQueryFont(disp, ts->fontname);
   if (font)
      goto done;

   /* This one really should succeed! */
   font = XLoadQueryFont(disp, "fixed");
   if (font)
      goto done;

   return -1;			/* Failed */

 done:
   fdc = Emalloc(sizeof(FontCtxXfont));
   if (!fdc)
      return -1;
   fdc->font = font;
   ts->fdc = fdc;
   ts->type = FONT_TYPE_XFONT;
   ts->ops = &FontOpsXfont;
   return -1;
}

static void
_xfont_Unload(TextState * ts __UNUSED__)
{
   FontCtxXfont       *fdc = (FontCtxXfont *) ts->fdc;

   XFreeFont(disp, fdc->font);
}

static void
_xfont_TextSize(TextState * ts, const char *text, int len,
		int *width, int *height, int *ascent)
{
   FontCtxXfont       *fdc = (FontCtxXfont *) ts->fdc;

   if (len == 0)
      len = strlen(text);
   if (fdc->font->min_byte1 == 0 && fdc->font->max_byte1 == 0)
      *width = XTextWidth(fdc->font, text, len);
   else
      *width = XTextWidth16(fdc->font, (XChar2b *) text, len / 2);
   *height = fdc->font->ascent + fdc->font->descent;
   *ascent = fdc->font->ascent;
}

static void
_xfont_TextDraw(TextState * ts, int x, int y, const char *text, int len)
{
   FontCtxXfont       *fdc = (FontCtxXfont *) ts->fdc;

   if (fdc->font->min_byte1 == 0 && fdc->font->max_byte1 == 0)
      XDrawString(disp, fdc->draw, fdc->gc, x, y, text, len);
   else
      XDrawString16(disp, fdc->draw, fdc->gc, x, y, (XChar2b *) text, len);
}

static int
_xfont_FdcInit(TextState * ts, Win win, Drawable draw)
{
   FontCtxXfont       *fdc = (FontCtxXfont *) ts->fdc;

   fdc->win = win;
   fdc->draw = draw;
   fdc->gc = _get_gc(win);

   XSetFont(disp, fdc->gc, fdc->font->fid);
   return 0;
}

static void
_xfont_FdcSetDrawable(TextState * ts, unsigned long draw)
{
   FontCtxXfont       *fdc = (FontCtxXfont *) ts->fdc;

   fdc->draw = draw;
}

static void
_xfont_FdcSetColor(TextState * ts, XColor * xc)
{
   FontCtxXfont       *fdc = (FontCtxXfont *) ts->fdc;

   EAllocColor(WinGetCmap(fdc->win), xc);
   XSetForeground(disp, fdc->gc, xc->pixel);
}

static void
_xfont_TextFit(TextState * ts, char **ptext, int *pw, int textwidth_limit)
{
   FontCtxXfont       *fdc = (FontCtxXfont *) ts->fdc;

   if (fdc->font->min_byte1 == 0 && fdc->font->max_byte1 == 0)
      TextstateTextFit1(ts, ptext, pw, textwidth_limit);
   else
      TextstateTextFit2(ts, ptext, pw, textwidth_limit);
}

const FontOps       FontOpsXfont = {
   _xfont_Load, _xfont_Unload, _xfont_TextSize, _xfont_TextFit, _xfont_TextDraw,
   _xfont_FdcInit, NULL, _xfont_FdcSetDrawable, _xfont_FdcSetColor
};
#endif /* FONT_TYPE_XFONT */

static void
TsTextDraw(TextState * ts, int x, int y, const char *text, int len)
{
   if (ts->style.effect == 1)
     {
	ts->ops->FdcSetColor(ts, &(ts->bg_col));
	ts->ops->TextDraw(ts, x + 1, y + 1, text, len);
     }
   else if (ts->style.effect == 2)
     {
	ts->ops->FdcSetColor(ts, &(ts->bg_col));
	ts->ops->TextDraw(ts, x - 1, y, text, len);
	ts->ops->TextDraw(ts, x + 1, y, text, len);
	ts->ops->TextDraw(ts, x, y - 1, text, len);
	ts->ops->TextDraw(ts, x, y + 1, text, len);
     }
   ts->ops->FdcSetColor(ts, &(ts->fg_col));
   ts->ops->TextDraw(ts, x, y, text, len);
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
   if (!FontOpsIft.Load(ts, 0))	/* Imlib2/FreeType */
      goto done;
#endif
#if FONT_TYPE_XFS
   if (!FontOpsXfs.Load(ts, 0))	/* XFontSet - XCreateFontSet */
      goto done;
#endif
#if FONT_TYPE_XFT
   if (!FontOpsXft.Load(ts, 0))	/* Xft */
      goto done;
#endif
#if FONT_TYPE_XFS
   if (!FontOpsXfs.Load(ts, 1))	/* XFontSet - XCreateFontSet */
      goto done;
#endif
#if FONT_TYPE_XFONT
   if (!FontOpsXfont.Load(ts, 1))	/* XFontStruct - XLoadQueryFont */
      goto done;
#endif

 done:
   if (!ts->ops)
      Eprintf("*** Unable to load font \"%s\"\n", ts->fontname);
   else if (EventDebug(EDBUG_TYPE_FONTS))
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
TextstateTextFit(TextState * ts, char **ptext, int *pw, int textwidth_limit)
{
   if (ts->need_utf8 || MB_CUR_MAX > 1)
      TextstateTextFitMB(ts, ptext, pw, textwidth_limit);
   else
      TextstateTextFit1(ts, ptext, pw, textwidth_limit);
}

void
TextstateTextDraw(TextState * ts, Win win, Drawable draw, const char *text,
		  int x, int y, int w, int h, const EImageBorder * pad,
		  int fsize __UNUSED__, int justification)
{
   const char         *str;
   char              **lines;
   int                 i, num_lines;
   int                 textwidth_limit, offset_x, offset_y;
   int                 xx, yy, ww, hh, ascent;
   Pixmap              drawable;

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
   Eprintf("TextstateTextDraw %d,%d %dx%d(%d): %s\n", x, y, w, h,
	   textwidth_limit, text);
#endif

   xx = x;
   yy = y;

   if (ts->ops->FdcInit(ts, win, draw))
      return;

#if FONT_TYPE_IFT
   if (ts->type == FONT_TYPE_IFT)
     {
	for (i = 0; i < num_lines; i++)
	  {
	     EImage             *im;

	     ts->ops->TextSize(ts, lines[i], 0, &ww, &hh, &ascent);
	     if (ww > textwidth_limit)
		ts->ops->TextFit(ts, &lines[i], &ww, textwidth_limit);

	     if (i == 0)
		yy += ascent;
	     xx = x + (((textwidth_limit - ww) * justification) >> 10);

	     im = TextImageGet(win, draw, xx - 1, yy - 1 - ascent,
			       ww + 2, hh + 2, ts);
	     if (!im)
		break;

	     offset_x = 1;
	     offset_y = ascent + 1;

	     ts->ops->FdcSetDrawable(ts, (unsigned long)im);

	     TsTextDraw(ts, offset_x, offset_y, lines[i], strlen(lines[i]));

	     TextImagePut(im, win, draw, xx - 1, yy - 1 - ascent,
			  ww + 2, hh + 2, ts);

	     yy += hh;
	  }
     }
   else
#endif /* FONT_TYPE_IFT */
     {
	for (i = 0; i < num_lines; i++)
	  {
	     ts->ops->TextSize(ts, lines[i], 0, &ww, &hh, &ascent);
	     if (ww > textwidth_limit)
		ts->ops->TextFit(ts, &lines[i], &ww, textwidth_limit);

	     if (i == 0)
		yy += ascent;
	     xx = x + (((textwidth_limit - ww) * justification) >> 10);

	     if (ts->style.orientation != FONT_TO_RIGHT)
		drawable = ECreatePixmap(win, ww + 2, hh + 2, 0);
	     else
		drawable = draw;
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

	     if (drawable != draw)
		ts->ops->FdcSetDrawable(ts, drawable);

	     TsTextDraw(ts, offset_x, offset_y, lines[i], strlen(lines[i]));

	     TextDrawRotBack(win, draw, drawable, xx - 1, yy - 1 - ascent,
			     ww + 2, hh + 2, ts);
	     if (drawable != draw)
		EFreePixmap(drawable);

	     yy += hh;
	  }
     }

   if (ts->ops->FdcFini)
      ts->ops->FdcFini(ts);

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

   TextstateTextDraw(ts, win, draw, text, x, y, w, h, NULL, fsize,
		     justification);
}
