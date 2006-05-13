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

#define ExTextExtents XmbTextExtents
#define ExDrawString XmbDrawString

static void         TextDrawRotTo(Win win, Drawable src, Drawable dst,
				  int x, int y, int w, int h, TextState * ts);
static void         TextDrawRotBack(Win win, Drawable dst, Drawable src,
				    int x, int y, int w, int h, TextState * ts);

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
TextStateLoadFont(TextState * ts)
{
   if (!ts->fontname)
      return;

   /* Quit if already done */
   if ((ts->efont) || (ts->xfont) || (ts->xfontset))
      return;

   ts->need_utf8 = Mode.locale.utf8_int;

   /* Try FreeType */
   {
      char                s[4096], *s2, *ss;

      s2 = Estrdup(ts->fontname);
      if (!s2)
	 return;
      ss = strchr(s2, '/');
      if (ss)
	{
	   *ss++ = '\0';
	   Esnprintf(s, sizeof(s), "%s.ttf", s2);
	   ts->efont = Efont_load(s2, atoi(ss));
	}
      Efree(s2);

      if (ts->efont)
	 ts->need_utf8 = 1;
   }
   if (ts->efont)
      goto done;

   /* Try X11 XCreateFontSet */
   {
      int                 i, missing_cnt, font_cnt;
      char              **missing_list, *def_str, **fn;
      XFontStruct       **fs;

      ts->xfontset = XCreateFontSet(disp, ts->fontname, &missing_list,
				    &missing_cnt, &def_str);
      if (missing_cnt)
	 XFreeStringList(missing_list);

      if (!ts->xfontset)
	{
	   ts->xfontset = XCreateFontSet(disp, "fixed", &missing_list,
					 &missing_cnt, &def_str);
	   if (missing_cnt)
	      XFreeStringList(missing_list);
	}

      if (ts->xfontset)
	{
	   ts->xfontset_ascent = 0;
	   font_cnt = XFontsOfFontSet(ts->xfontset, &fs, &fn);
	   for (i = 0; i < font_cnt; i++)
	      ts->xfontset_ascent = MAX(fs[i]->ascent, ts->xfontset_ascent);
	}
   }
   if (ts->xfontset)
      goto done;

   /* Try X11 XLoadQueryFont */
   if (strchr(ts->fontname, ',') == NULL)
      ts->xfont = XLoadQueryFont(disp, ts->fontname);
   if (ts->xfont)
      goto done;

   /* This one really should succeed! */
   ts->xfont = XLoadQueryFont(disp, "fixed");

 done:
   return;
}

void
TextSize(TextClass * tclass, int active, int sticky, int state,
	 const char *text, int *width, int *height, int fsize __UNUSED__)
{
   const char         *str;
   char              **lines;
   int                 i, num_lines;
   TextState          *ts;

   *width = 0;
   *height = 0;

   if (!text)
      return;

   ts = TextclassGetTextState(tclass, state, active, sticky);
   if (!ts)
      return;

   TextStateLoadFont(ts);

   /* Do encoding conversion, if necessary */
   str = EstrInt2Enc(text, ts->need_utf8);
   lines = StrlistFromString(str, '\n', &num_lines);
   EstrInt2EncFree(str, ts->need_utf8);
   if (!lines)
      return;

   if (ts->efont)
     {
	for (i = 0; i < num_lines; i++)
	  {
	     int                 ascent, descent, wid;

	     Efont_extents(ts->efont, lines[i], &ascent, &descent, &wid, NULL,
			   NULL, NULL, NULL);
	     *height += ascent + descent;
	     if (wid > *width)
		*width = wid;
	  }
     }
   else if (ts->xfontset)
     {
	for (i = 0; i < num_lines; i++)
	  {
	     XRectangle          ret1, ret2;

	     ExTextExtents(ts->xfontset, lines[i], strlen(lines[i]), &ret1,
			   &ret2);
	     *height += ret2.height;
	     if (ret2.width > *width)
		*width = ret2.width;
	  }
     }
   else if ((ts->xfont) && (ts->xfont->min_byte1 == 0)
	    && (ts->xfont->max_byte1 == 0))
     {
	for (i = 0; i < num_lines; i++)
	  {
	     int                 wid;

	     wid = XTextWidth(ts->xfont, lines[i], strlen(lines[i]));
	     *height += ts->xfont->ascent + ts->xfont->descent;
	     if (wid > *width)
		*width = wid;
	  }
     }
   else if ((ts->xfont))
     {
	for (i = 0; i < num_lines; i++)
	  {
	     int                 wid;

	     wid = XTextWidth16(ts->xfont, (XChar2b *) lines[i],
				strlen(lines[i]) / 2);
	     *height += ts->xfont->ascent + ts->xfont->descent;
	     if (wid > *width)
		*width = wid;
	  }
     }
   StrlistFree(lines, num_lines);
}

void
TextstateDrawText(TextState * ts, Win win, Drawable draw, const char *text,
		  int x, int y, int w, int h, int fsize __UNUSED__,
		  int justification)
{
   const char         *str;
   char              **lines;
   int                 i, num_lines;
   int                 xx, yy;
   static GC           gc = 0;
   int                 textwidth_limit, offset_x, offset_y;
   Pixmap              drawable;

   if (w <= 0 || h <= 0)
      return;

   TextStateLoadFont(ts);

   /* Do encoding conversion, if necessary */
   str = EstrInt2Enc(text, ts->need_utf8);
   lines = StrlistFromString(str, '\n', &num_lines);
   EstrInt2EncFree(str, ts->need_utf8);
   if (!lines)
      return;

   if (draw == None)
      draw = WinGetXwin(win);

   if (!gc)
      gc = EXCreateGC(draw, 0, NULL);

   if (ts->style.orientation == FONT_TO_RIGHT ||
       ts->style.orientation == FONT_TO_LEFT)
      textwidth_limit = w;
   else
      textwidth_limit = h;

   xx = x;
   yy = y;

   if (ts->efont)
     {
	for (i = 0; i < num_lines; i++)
	  {
	     int                 ascent, descent, wid;

	     Efont_extents(ts->efont, lines[i], &ascent, &descent, &wid, NULL,
			   NULL, NULL, NULL);
	     if (wid > textwidth_limit)
	       {
		  char               *new_line;
		  int                 nuke_count = 0;
		  int                 len;

		  len = strlen(lines[i]);
		  new_line = Emalloc(len + 10);
		  if (!new_line)
		     goto done;
		  while (wid > textwidth_limit)
		    {
		       nuke_count++;
		       if (nuke_count > len)
			 {
			    new_line[0] = 0;
			    strncat(new_line, lines[i], 1);
			    strcat(new_line, "...");
			    break;
			 }
		       new_line[0] = 0;
		       strncat(new_line, lines[i], (len - nuke_count) / 2);
		       strcat(new_line, "...");
		       strcat(new_line,
			      lines[i] + ((len - nuke_count) / 2) + nuke_count);
		       Efont_extents(ts->efont, new_line, &ascent, &descent,
				     &wid, NULL, NULL, NULL, NULL);
		    }
		  Efree(lines[i]);
		  lines[i] = new_line;
	       }
	     if (i == 0)
		yy += ascent;
	     xx = x + (((textwidth_limit - wid) * justification) >> 10);

	     if (ts->style.orientation != FONT_TO_RIGHT)
		drawable = ECreatePixmap(win, wid + 2, ascent + descent + 2, 0);
	     else
		drawable = draw;
	     TextDrawRotTo(win, draw, drawable, xx - 1, yy - 1 - ascent,
			   wid + 2, ascent + descent + 2, ts);
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

	     if (ts->effect == 1)
	       {
		  EAllocColor(&ts->bg_col);
		  XSetForeground(disp, gc, ts->bg_col.pixel);
		  EFont_draw_string(drawable, gc, offset_x + 1, offset_y + 1,
				    lines[i], ts->efont, VRoot.vis, VRoot.cmap);
	       }
	     else if (ts->effect == 2)
	       {
		  EAllocColor(&ts->bg_col);
		  XSetForeground(disp, gc, ts->bg_col.pixel);
		  EFont_draw_string(drawable, gc, offset_x - 1, offset_y,
				    lines[i], ts->efont, VRoot.vis, VRoot.cmap);
		  EFont_draw_string(drawable, gc, offset_x + 1, offset_y,
				    lines[i], ts->efont, VRoot.vis, VRoot.cmap);
		  EFont_draw_string(drawable, gc, offset_x, offset_y - 1,
				    lines[i], ts->efont, VRoot.vis, VRoot.cmap);
		  EFont_draw_string(drawable, gc, offset_x, offset_y + 1,
				    lines[i], ts->efont, VRoot.vis, VRoot.cmap);
	       }
	     EAllocColor(&ts->fg_col);
	     XSetForeground(disp, gc, ts->fg_col.pixel);
	     EFont_draw_string(drawable, gc, offset_x, offset_y, lines[i],
			       ts->efont, VRoot.vis, VRoot.cmap);

	     TextDrawRotBack(win, draw, drawable, xx - 1, yy - 1 - ascent,
			     wid + 2, ascent + descent + 2, ts);
	     if (drawable != draw)
		EFreePixmap(drawable);
	     yy += ascent + descent;
	  }
     }
   else if (ts->xfontset)
     {
	for (i = 0; i < num_lines; i++)
	  {
	     XRectangle          ret1, ret2;

	     ExTextExtents(ts->xfontset, lines[i], strlen(lines[i]), &ret1,
			   &ret2);
	     if (ret2.width > textwidth_limit)
	       {
		  char               *new_line;
		  int                 nuke_count = 0;
		  int                 len;
		  wchar_t            *wc_line = NULL;
		  int                 wc_len;

		  len = strlen(lines[i]);
		  new_line = Emalloc(len + 10);
		  if (!new_line)
		     goto done;
		  wc_len = mbstowcs(NULL, lines[i], 0);
		  if (wc_len > 0)
		    {
		       wc_line =
			  (wchar_t *) Emalloc((wc_len + 1) * sizeof(wchar_t));
		       mbstowcs(wc_line, lines[i], len);
		       wc_line[wc_len] = (wchar_t) '\0';
		    }

		  while (ret2.width > textwidth_limit)
		    {
		       nuke_count++;
		       if (nuke_count > wc_len)
			 {
			    int                 mlen;

			    new_line[0] = 0;
			    if (MB_CUR_MAX > 1 && wc_len > 0)
			      {	/* if multibyte locale,... */
				 mlen = mblen(lines[i], MB_CUR_MAX);
				 if (mlen < 0)
				    mlen = 1;
			      }
			    else
			       mlen = 1;

			    strncat(new_line, lines[i], mlen);
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
			    len_mb =
			       wcstombs(new_line + j,
					wc_line + (wc_len - nuke_count) / 2 +
					nuke_count, len + 10 - j);
			    if (len_mb > 0)
			       j += len_mb;
			    new_line[j] = '\0';
			 }
		       else
			 {
			    strncat(new_line, lines[i], (len - nuke_count) / 2);
			    strcat(new_line, "...");
			    strcat(new_line,
				   lines[i] + ((len - nuke_count) / 2) +
				   nuke_count);
			 }
		       ExTextExtents(ts->xfontset, new_line, strlen(new_line),
				     &ret1, &ret2);
		    }
		  Efree(lines[i]);
		  lines[i] = new_line;
		  if (wc_len > 0)
		     Efree(wc_line);
	       }
	     if (i == 0)
		yy += ts->xfontset_ascent;
	     xx = x + (((textwidth_limit - ret2.width) * justification) >> 10);

	     if (ts->style.orientation != FONT_TO_RIGHT)
		drawable =
		   ECreatePixmap(win, ret2.width + 2, ret2.height + 2, 0);
	     else
		drawable = draw;
	     TextDrawRotTo(win, draw, drawable, xx - 1,
			   yy - (ts->xfontset_ascent) - 1, ret2.width + 2,
			   ret2.height + 2, ts);
	     if (ts->style.orientation == FONT_TO_RIGHT)
	       {
		  offset_x = xx;
		  offset_y = yy;
	       }
	     else
	       {
		  offset_x = 1;
		  offset_y = ts->xfontset_ascent + 1;
	       }

	     if (ts->effect == 1)
	       {
		  EAllocColor(&ts->bg_col);
		  XSetForeground(disp, gc, ts->bg_col.pixel);
		  ExDrawString(disp, drawable, ts->xfontset, gc, offset_x + 1,
			       offset_y + 1, lines[i], strlen(lines[i]));
	       }
	     else if (ts->effect == 2)
	       {
		  EAllocColor(&ts->bg_col);
		  XSetForeground(disp, gc, ts->bg_col.pixel);
		  ExDrawString(disp, drawable, ts->xfontset, gc, offset_x - 1,
			       offset_y, lines[i], strlen(lines[i]));
		  ExDrawString(disp, drawable, ts->xfontset, gc, offset_x + 1,
			       offset_y, lines[i], strlen(lines[i]));
		  ExDrawString(disp, drawable, ts->xfontset, gc, offset_x,
			       offset_y - 1, lines[i], strlen(lines[i]));
		  ExDrawString(disp, drawable, ts->xfontset, gc, offset_x,
			       offset_y + 1, lines[i], strlen(lines[i]));
	       }
	     EAllocColor(&ts->fg_col);
	     XSetForeground(disp, gc, ts->fg_col.pixel);
	     ExDrawString(disp, drawable, ts->xfontset, gc, offset_x, offset_y,
			  lines[i], strlen(lines[i]));

	     TextDrawRotBack(win, draw, drawable, xx - 1,
			     yy - (ts->xfontset_ascent) - 1, ret2.width + 2,
			     ret2.height + 2, ts);
	     if (drawable != draw)
		EFreePixmap(drawable);
	     yy += ret2.height;
	  }
     }
   else if ((ts->xfont) && (ts->xfont->min_byte1 == 0)
	    && (ts->xfont->max_byte1 == 0))
     {
	XSetFont(disp, gc, ts->xfont->fid);
	for (i = 0; i < num_lines; i++)
	  {
	     int                 wid, ascent, descent;

	     wid = XTextWidth(ts->xfont, lines[i], strlen(lines[i]));
	     ascent = ts->xfont->ascent;
	     descent = ts->xfont->descent;
	     if (wid > textwidth_limit)
	       {
		  char               *new_line;
		  int                 nuke_count = 0;
		  int                 len;

		  len = strlen(lines[i]);
		  new_line = Emalloc(len + 10);
		  if (!new_line)
		     goto done;
		  while (wid > textwidth_limit)
		    {
		       nuke_count++;
		       if (nuke_count > len)
			 {
			    new_line[0] = 0;
			    strncat(new_line, lines[i], 1);
			    strcat(new_line, "...");
			    break;
			 }
		       new_line[0] = 0;
		       strncat(new_line, lines[i], (len - nuke_count) / 2);
		       strcat(new_line, "...");
		       strcat(new_line,
			      lines[i] + ((len - nuke_count) / 2) + nuke_count);
		       wid = XTextWidth(ts->xfont, new_line, strlen(new_line));
		    }
		  Efree(lines[i]);
		  lines[i] = new_line;
	       }
	     if (i == 0)
		yy += ts->xfont->ascent;
	     xx = x + (((textwidth_limit - wid) * justification) >> 10);

	     if (ts->style.orientation != FONT_TO_RIGHT)
		drawable = ECreatePixmap(win, wid + 2, ascent + descent + 2, 0);
	     else
		drawable = draw;
	     TextDrawRotTo(win, draw, drawable, xx - 1, yy - ascent - 1,
			   wid + 2, ascent + descent + 2, ts);
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

	     if (ts->effect == 1)
	       {
		  EAllocColor(&ts->bg_col);
		  XSetForeground(disp, gc, ts->bg_col.pixel);
		  XDrawString(disp, drawable, gc, offset_x + 1, offset_y + 1,
			      lines[i], strlen(lines[i]));
	       }
	     else if (ts->effect == 2)
	       {
		  EAllocColor(&ts->bg_col);
		  XSetForeground(disp, gc, ts->bg_col.pixel);
		  XDrawString(disp, drawable, gc, offset_x - 1, offset_y,
			      lines[i], strlen(lines[i]));
		  XDrawString(disp, drawable, gc, offset_x + 1, offset_y,
			      lines[i], strlen(lines[i]));
		  XDrawString(disp, drawable, gc, offset_x, offset_y - 1,
			      lines[i], strlen(lines[i]));
		  XDrawString(disp, drawable, gc, offset_x, offset_y + 1,
			      lines[i], strlen(lines[i]));
	       }
	     EAllocColor(&ts->fg_col);
	     XSetForeground(disp, gc, ts->fg_col.pixel);
	     XDrawString(disp, drawable, gc, offset_x, offset_y, lines[i],
			 strlen(lines[i]));

	     TextDrawRotBack(win, draw, drawable, xx - 1, yy - 1 - ascent,
			     wid + 2, ascent + descent + 2, ts);
	     if (drawable != draw)
		EFreePixmap(drawable);
	     yy += ts->xfont->ascent + ts->xfont->descent;
	  }
     }
   else if ((ts->xfont))
     {
	XSetFont(disp, gc, ts->xfont->fid);
	for (i = 0; i < num_lines; i++)
	  {
	     int                 wid, ascent, descent;

	     wid = XTextWidth16(ts->xfont, (XChar2b *) lines[i],
				strlen(lines[i]) / 2);
	     ascent = ts->xfont->ascent;
	     descent = ts->xfont->descent;
	     if (wid > textwidth_limit)
	       {
		  char               *new_line;
		  int                 nuke_count = 0;
		  int                 len;

		  len = strlen(lines[i]);
		  new_line = Emalloc(len + 20);
		  if (!new_line)
		     goto done;
		  while (wid > textwidth_limit)
		    {
		       nuke_count += 2;
		       if (nuke_count > len)
			 {
			    new_line[0] = 0;
			    strncat(new_line, lines[i], 1);
			    strcat(new_line, ". . . ");
			    break;
			 }
		       new_line[0] = 0;
		       strncat(new_line, lines[i], (len - nuke_count) / 4);
		       strcat(new_line, ". . . ");
		       strcat(new_line,
			      lines[i] + ((len - nuke_count) / 4) + nuke_count);
		       wid =
			  XTextWidth16(ts->xfont, (XChar2b *) new_line,
				       strlen(new_line) / 2);
		    }
		  Efree(lines[i]);
		  lines[i] = new_line;
	       }
	     if (i == 0)
		yy += ts->xfont->ascent;
	     xx = x + (((textwidth_limit - wid) * justification) >> 10);

	     if (ts->style.orientation != FONT_TO_RIGHT)
		drawable = ECreatePixmap(win, wid + 2, ascent + descent + 2, 0);
	     else
		drawable = draw;
	     TextDrawRotTo(win, draw, drawable, xx - 1, yy - ascent - 1,
			   wid + 2, ascent + descent + 2, ts);
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

	     if (ts->effect == 1)
	       {
		  EAllocColor(&ts->bg_col);
		  XSetForeground(disp, gc, ts->bg_col.pixel);
		  XDrawString16(disp, drawable, gc, offset_x + 1, offset_y + 1,
				(XChar2b *) lines[i], strlen(lines[i]) / 2);
	       }
	     else if (ts->effect == 2)
	       {
		  EAllocColor(&ts->bg_col);
		  XSetForeground(disp, gc, ts->bg_col.pixel);
		  XDrawString16(disp, drawable, gc, offset_x - 1, offset_y,
				(XChar2b *) lines[i], strlen(lines[i]) / 2);
		  XDrawString16(disp, drawable, gc, offset_y + 1, offset_y,
				(XChar2b *) lines[i], strlen(lines[i]) / 2);
		  XDrawString16(disp, drawable, gc, offset_x, offset_y - 1,
				(XChar2b *) lines[i], strlen(lines[i]) / 2);
		  XDrawString16(disp, drawable, gc, offset_x, offset_y + 1,
				(XChar2b *) lines[i], strlen(lines[i]) / 2);
	       }
	     EAllocColor(&ts->fg_col);
	     XSetForeground(disp, gc, ts->fg_col.pixel);
	     XDrawString16(disp, drawable, gc, offset_x, offset_y,
			   (XChar2b *) lines[i], strlen(lines[i]) / 2);

	     TextDrawRotBack(win, draw, drawable, xx - 1, yy - 1 - ascent,
			     wid + 2, ascent + descent + 2, ts);
	     if (drawable != draw)
		EFreePixmap(drawable);
	     yy += ts->xfont->ascent + ts->xfont->descent;
	  }
     }
 done:
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

   TextstateDrawText(ts, win, draw, text, x, y, w, h, fsize, justification);
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
