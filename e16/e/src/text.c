/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
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

static void         TextDrawRotTo(Window win, Drawable * drawable, int x, int y,
				  int w, int h, TextState * ts);

static void         TextDrawRotBack(Window win, Drawable drawable, int x, int y,
				    int w, int h, TextState * ts);

TextState          *
TextGetState(TextClass * tclass, int active, int sticky, int state)
{
   EDBUG(5, "TextGetState");
   if (active)
     {
	if (!sticky)
	  {
	     switch (state)
	       {
	       case STATE_NORMAL:
		  EDBUG_RETURN(tclass->active.normal);
	       case STATE_HILITED:
		  EDBUG_RETURN(tclass->active.hilited);
	       case STATE_CLICKED:
		  EDBUG_RETURN(tclass->active.clicked);
	       case STATE_DISABLED:
		  EDBUG_RETURN(tclass->active.disabled);
	       default:
		  break;
	       }
	  }
	else
	  {
	     switch (state)
	       {
	       case STATE_NORMAL:
		  EDBUG_RETURN(tclass->sticky_active.normal);
	       case STATE_HILITED:
		  EDBUG_RETURN(tclass->sticky_active.hilited);
	       case STATE_CLICKED:
		  EDBUG_RETURN(tclass->sticky_active.clicked);
	       case STATE_DISABLED:
		  EDBUG_RETURN(tclass->sticky_active.disabled);
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
	     EDBUG_RETURN(tclass->sticky.normal);
	  case STATE_HILITED:
	     EDBUG_RETURN(tclass->sticky.hilited);
	  case STATE_CLICKED:
	     EDBUG_RETURN(tclass->sticky.clicked);
	  case STATE_DISABLED:
	     EDBUG_RETURN(tclass->sticky.disabled);
	  default:
	     break;
	  }
     }
   else
     {
	switch (state)
	  {
	  case STATE_NORMAL:
	     EDBUG_RETURN(tclass->norm.normal);
	  case STATE_HILITED:
	     EDBUG_RETURN(tclass->norm.hilited);
	  case STATE_CLICKED:
	     EDBUG_RETURN(tclass->norm.clicked);
	  case STATE_DISABLED:
	     EDBUG_RETURN(tclass->norm.disabled);
	  default:
	     break;
	  }
     }
   EDBUG_RETURN(NULL);
}

char              **
TextGetLines(const char *text, int *count)
{
   int                 i, j, k;
   char              **list = NULL;

   EDBUG(5, "TextGetLines");
   *count = 0;
   i = 0;
   k = 0;
   if (!text)
      EDBUG_RETURN(NULL);
   *count = 1;
   while (text[i])
     {
	j = i;
	while ((text[j]) && (text[j] != '\n'))
	   j++;
	k++;
	list = Erealloc(list, sizeof(char *) * k);
	list[k - 1] = Emalloc(sizeof(char) * (j - i + 1));

	strncpy(list[k - 1], &(text[i]), (j - i));
	list[k - 1][j - i] = 0;
	i = j;
	if (text[i] == '\n')
	   i++;
     }
   *count = k;
   EDBUG_RETURN(list);
}

void
TextStateLoadFont(TextState * ts)
{
   EDBUG(5, "TextStateLoadFont");
#if USE_FNLIB
   if ((ts->font) || (ts->efont) || (ts->xfont) || (ts->xfontset))
#else
   if ((ts->efont) || (ts->xfont) || (ts->xfontset))
#endif
      EDBUG_RETURN_;
   if (!ts->fontname)
      EDBUG_RETURN_;
#if USE_FNLIB
   if ((!ts->font) && (!ts->efont))
      ts->font = Fnlib_load_font(pFnlibData, ts->fontname);
#endif
#if USE_FNLIB
   if ((!ts->font) && (!ts->efont))
#else
   if (!ts->efont)
#endif
     {
	char                s[4096], w[4046], *dup, *ss;

	dup = NULL;
	dup = Estrdup(ts->fontname);
	ss = strchr(dup, '/');
	if (ss)
	  {
	     *ss = ' ';
	     word(dup, 1, w);
	     Esnprintf(s, sizeof(s), "ttfonts/%s.ttf", w);
	     word(dup, 2, w);
	     ss = FindFile(s);
	     if (ss)
	       {
		  ts->efont = Efont_load(ss, atoi(w));
		  Efree(ss);
	       }
	  }
	if (dup)
	   Efree(dup);
     }
#if USE_FNLIB
   if ((!ts->font) && (!ts->efont))
#else
   if (!ts->efont)
#endif
     {
	if ((!ts->xfont) && (strchr(ts->fontname, ',') == NULL))
	  {
	     ts->xfont = XLoadQueryFont(disp, ts->fontname);
	  }
	else if (!ts->xfontset)
	  {
	     int                 i, missing_cnt, font_cnt;
	     char              **missing_list, *def_str, **fn;
	     XFontStruct       **fs;

	     ts->xfontset =
		XCreateFontSet(disp, ts->fontname, &missing_list, &missing_cnt,
			       &def_str);
	     if (missing_cnt)
	       {
		  XFreeStringList(missing_list);
		  /* EDBUG_RETURN_; */
	       }

	     if (!ts->xfontset)
	       {
		  ts->xfontset =
		     XCreateFontSet(disp, "fixed", &missing_list, &missing_cnt,
				    &def_str);
		  if (missing_cnt)
		     XFreeStringList(missing_list);
	       }

	     if (ts->xfontset)
	       {
		  ts->xfontset_ascent = 0;
		  font_cnt = XFontsOfFontSet(ts->xfontset, &fs, &fn);
		  for (i = 0; i < font_cnt; i++)
		     ts->xfontset_ascent =
			MAX(fs[i]->ascent, ts->xfontset_ascent);
	       }
	  }
	if (!ts->xfont)
	   ts->xfont = XLoadQueryFont(disp, "fixed");
     }
   EDBUG_RETURN_;
}

void
TextSize(TextClass * tclass, int active, int sticky, int state,
	 const char *text, int *width, int *height, int fsize)
{
   char              **lines;
   int                 i, num_lines;
   TextState          *ts;

   EDBUG(4, "TextSize");
   *width = 0;
   *height = 0;
   lines = TextGetLines(text, &num_lines);
   if (!lines)
      EDBUG_RETURN_;
   ts = TextGetState(tclass, active, sticky, state);
   if (!ts)
      EDBUG_RETURN_;
   TextStateLoadFont(ts);
#if USE_FNLIB
   if (ts->font)
     {
	for (i = 0; i < num_lines; i++)
	  {
	     int                 high, wid, dummy;

	     Fnlib_measure(pFnlibData, ts->font, 0, 0, 999999, 999999, 0, 0,
			   fsize, &ts->style, (unsigned char *)lines[i], 0, 0,
			   &dummy, &dummy, &wid, &high, &dummy, &dummy, &dummy,
			   &dummy);
	     *height += high;
	     if (wid > *width)
		*width = wid;
	  }
     }
   else
#endif
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

	     XmbTextExtents(ts->xfontset, lines[i], strlen(lines[i]), &ret1,
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

	     wid =
		XTextWidth16(ts->xfont, (XChar2b *) lines[i],
			     strlen(lines[i]) / 2);
	     *height += ts->xfont->ascent + ts->xfont->descent;
	     if (wid > *width)
		*width = wid;
	  }
     }
   freestrlist(lines, num_lines);
   EDBUG_RETURN_;
   fsize = 0;
}

void
TextDraw(TextClass * tclass, Window win, int active, int sticky, int state,
	 const char *text, int x, int y, int w, int h, int fsize,
	 int justification)
{
   char              **lines;
   int                 i, num_lines;
   TextState          *ts;
   int                 xx, yy;
   XGCValues           gcv;
   static GC           gc = 0;

   int                 textwidth_limit, offset_x, offset_y;
   Pixmap              drawable;

   EDBUG(4, "TextDraw");
   lines = TextGetLines(text, &num_lines);
   if (!lines)
      EDBUG_RETURN_;
   ts = TextGetState(tclass, active, sticky, state);
   if (!ts)
      EDBUG_RETURN_;
   TextStateLoadFont(ts);
   xx = x;
   yy = y;
   if (!gc)
      gc = XCreateGC(disp, win, 0, &gcv);

   if (ts->style.orientation == FONT_TO_RIGHT
       || ts->style.orientation == FONT_TO_LEFT)
      textwidth_limit = w;
   else
      textwidth_limit = h;

#if USE_FNLIB
   if (ts->font)
     {
	for (i = 0; i < num_lines; i++)
	  {
	     int                 high, wid, dummy;

	     Fnlib_measure(pFnlibData, ts->font, 0, 0, 999999, 999999, 0, 0,
			   fsize, &ts->style, (unsigned char *)lines[i], 0, 0,
			   &dummy, &dummy, &wid, &high, &dummy, &dummy, &dummy,
			   &dummy);
	     if ((ts->style.orientation == FONT_TO_UP)
		 || (ts->style.orientation == FONT_TO_DOWN))
		fsize = w;
	     xx = x + (((w - wid) * justification) >> 10);
	     Fnlib_draw(pFnlibData, ts->font, win, 0, xx, yy, w, h, 0, 0, fsize,
			&ts->style, (unsigned char *)lines[i]);
	     yy += high;
	  }
     }
   else
#endif
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
		drawable =
		   ECreatePixmap(disp, root.win, wid + 2, ascent + descent + 2,
				 GetWinDepth(win));
	     else
		drawable = win;
	     TextDrawRotTo(win, &drawable, xx - 1, yy - 1 - ascent, wid + 2,
			   ascent + descent + 2, ts);
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
		  EFont_draw_string(disp, drawable, gc, offset_x + 1,
				    offset_y + 1, lines[i], ts->efont,
				    root.vis, root.cmap);
	       }
	     else if (ts->effect == 2)
	       {
		  EAllocColor(&ts->bg_col);
		  XSetForeground(disp, gc, ts->bg_col.pixel);
		  EFont_draw_string(disp, drawable, gc, offset_x - 1, offset_y,
				    lines[i], ts->efont, root.vis, root.cmap);
		  EFont_draw_string(disp, drawable, gc, offset_x + 1, offset_y,
				    lines[i], ts->efont, root.vis, root.cmap);
		  EFont_draw_string(disp, drawable, gc, offset_x, offset_y - 1,
				    lines[i], ts->efont, root.vis, root.cmap);
		  EFont_draw_string(disp, drawable, gc, offset_x, offset_y + 1,
				    lines[i], ts->efont, root.vis, root.cmap);
	       }
	     EAllocColor(&ts->fg_col);
	     XSetForeground(disp, gc, ts->fg_col.pixel);
	     EFont_draw_string(disp, drawable, gc, offset_x, offset_y, lines[i],
			       ts->efont, root.vis, root.cmap);

	     TextDrawRotBack(win, drawable, xx - 1, yy - 1 - ascent, wid + 2,
			     ascent + descent + 2, ts);
	     if (drawable != win)
		EFreePixmap(disp, drawable);
	     yy += ascent + descent;
	  }
     }
   else if (ts->xfontset)
     {
	for (i = 0; i < num_lines; i++)
	  {
	     XRectangle          ret1, ret2;

	     XmbTextExtents(ts->xfontset, lines[i], strlen(lines[i]), &ret1,
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
		       if (nuke_count > len)
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
		       XmbTextExtents(ts->xfontset, new_line, strlen(new_line),
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
		   ECreatePixmap(disp, root.win, ret2.width + 2,
				 ret2.height + 2, GetWinDepth(win));
	     else
		drawable = win;
	     TextDrawRotTo(win, &drawable, xx - 1,
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
		  XmbDrawString(disp, drawable, ts->xfontset, gc, offset_x + 1,
				offset_y + 1, lines[i], strlen(lines[i]));
	       }
	     else if (ts->effect == 2)
	       {
		  EAllocColor(&ts->bg_col);
		  XSetForeground(disp, gc, ts->bg_col.pixel);
		  XmbDrawString(disp, drawable, ts->xfontset, gc, offset_x - 1,
				offset_y, lines[i], strlen(lines[i]));
		  XmbDrawString(disp, drawable, ts->xfontset, gc, offset_x + 1,
				offset_y, lines[i], strlen(lines[i]));
		  XmbDrawString(disp, drawable, ts->xfontset, gc, offset_x,
				offset_y - 1, lines[i], strlen(lines[i]));
		  XmbDrawString(disp, drawable, ts->xfontset, gc, offset_x,
				offset_y + 1, lines[i], strlen(lines[i]));
	       }
	     EAllocColor(&ts->fg_col);
	     XSetForeground(disp, gc, ts->fg_col.pixel);
	     XmbDrawString(disp, drawable, ts->xfontset, gc, offset_x, offset_y,
			   lines[i], strlen(lines[i]));

	     TextDrawRotBack(win, drawable, xx - 1,
			     yy - (ts->xfontset_ascent) - 1, ret2.width + 2,
			     ret2.height + 2, ts);
	     if (drawable != win)
		EFreePixmap(disp, drawable);
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
		drawable =
		   ECreatePixmap(disp, root.win, wid + 2, ascent + descent + 2,
				 GetWinDepth(win));
	     else
		drawable = win;
	     TextDrawRotTo(win, &drawable, xx - 1, yy - ascent - 1, wid + 2,
			   ascent + descent + 2, ts);
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

	     TextDrawRotBack(win, drawable, xx - 1, yy - 1 - ascent, wid + 2,
			     ascent + descent + 2, ts);
	     if (drawable != win)
		EFreePixmap(disp, drawable);
	     yy += ts->xfont->ascent + ts->xfont->descent;
	  }
     }
   else if ((ts->xfont))
     {
	XSetFont(disp, gc, ts->xfont->fid);
	for (i = 0; i < num_lines; i++)
	  {
	     int                 wid, ascent, descent;

	     wid =
		XTextWidth16(ts->xfont, (XChar2b *) lines[i],
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
		drawable =
		   ECreatePixmap(disp, root.win, wid + 2, ascent + descent + 2,
				 GetWinDepth(win));
	     else
		drawable = win;
	     TextDrawRotTo(win, &drawable, xx - 1, yy - ascent - 1, wid + 2,
			   ascent + descent + 2, ts);
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

	     TextDrawRotBack(win, drawable, xx - 1, yy - 1 - ascent, wid + 2,
			     ascent + descent + 2, ts);
	     if (drawable != win)
		EFreePixmap(disp, drawable);
	     yy += ts->xfont->ascent + ts->xfont->descent;
	  }
     }
   freestrlist(lines, num_lines);
   EDBUG_RETURN_;
   fsize = 0;
}

void
TextDrawRotTo(Window win, Drawable * drawable, int x, int y, int w, int h,
	      TextState * ts)
{
   Imlib_Image        *ii = NULL;
   int                 win_x, win_y;
   unsigned int        win_w, win_h, win_b, win_d;

   switch (ts->style.orientation)
     {
     case FONT_TO_UP:
	imlib_context_set_drawable(win);
	ii = imlib_create_image_from_drawable(0, y, x, h, w, 0);
	imlib_context_set_image(ii);
	imlib_image_orientate(1);
	imlib_context_set_drawable(*drawable);
	imlib_render_image_on_drawable_at_size(0, 0, w, h);
	break;
     case FONT_TO_DOWN:
	EGetGeometry(disp, win, &(root.win), &win_x, &win_y, &win_w, &win_h,
		     &win_b, &win_d);
	imlib_context_set_drawable(win);
	ii = imlib_create_image_from_drawable(0, win_w - y - h, x, h, w, 0);
	imlib_context_set_image(ii);
	imlib_image_orientate(3);
	imlib_context_set_drawable(*drawable);
	imlib_render_image_on_drawable_at_size(0, 0, w, h);
	break;
     case FONT_TO_LEFT:	/* Holy carumba! That's for yoga addicts, maybe .... */
	imlib_context_set_drawable(win);
	ii = imlib_create_image_from_drawable(0, x, y, w, h, 0);
	imlib_context_set_image(ii);
	imlib_image_orientate(2);
	imlib_context_set_drawable(*drawable);
	imlib_render_image_on_drawable_at_size(0, 0, w, h);
	break;
     default:
	break;
     }
   if (ii)
      imlib_free_image();
}

void
TextDrawRotBack(Window win, Pixmap drawable, int x, int y, int w, int h,
		TextState * ts)
{
   Imlib_Image        *ii = NULL;
   int                 win_x, win_y;
   unsigned int        win_w, win_h, win_b, win_d;

   switch (ts->style.orientation)
     {
     case FONT_TO_UP:
	imlib_context_set_drawable(drawable);
	ii = imlib_create_image_from_drawable(0, 0, 0, w, h, 0);
	imlib_context_set_image(ii);
	imlib_image_orientate(3);
	imlib_context_set_drawable(win);
	imlib_render_image_on_drawable_at_size(y, x, h, w);
	break;
     case FONT_TO_DOWN:
	imlib_context_set_drawable(drawable);
	EGetGeometry(disp, win, &(root.win), &win_x, &win_y, &win_w, &win_h,
		     &win_b, &win_d);
	ii = imlib_create_image_from_drawable(0, 0, 0, w, h, 0);
	imlib_context_set_image(ii);
	imlib_image_orientate(1);
	imlib_context_set_drawable(win);
	imlib_render_image_on_drawable_at_size(win_w - y - h, x, h, w);
	break;
     case FONT_TO_LEFT:	/* Holy carumba! That's for yoga addicts, maybe .... */
	imlib_context_set_drawable(drawable);
	ii = imlib_create_image_from_drawable(0, 0, 0, w, h, 0);
	imlib_context_set_image(ii);
	imlib_image_orientate(2);
	imlib_context_set_drawable(win);
	imlib_render_image_on_drawable_at_size(x, y, w, h);
	break;
     default:
	break;
     }
   if (ii)
      imlib_free_image();
}
