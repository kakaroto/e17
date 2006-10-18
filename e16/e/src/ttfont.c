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
#include <Imlib2.h>

#if FONT_TYPE_IFT

static void
EFonts_Init(void)
{
#if !TEST_TTFONT
   char                s[4096];
   char              **lst;
   int                 i, num;

   Esnprintf(s, sizeof(s), "%s/ttfonts", Mode.theme.path);
   imlib_add_path_to_font_path(s);
   lst = StrlistFromString(Conf.theme.ttfont_path, ':', &num);
   for (i = 0; i < num; i++)
      imlib_add_path_to_font_path(lst[i]);
   StrlistFree(lst, num);
#endif
}

static EFont       *
Efont_load(const char *file, int size)
{
   static char         ttfont_path_set = 0;
   char                s[4096];
   EFont              *f;

   if (!ttfont_path_set)
     {
	EFonts_Init();
	ttfont_path_set = 1;
     }

   Esnprintf(s, sizeof(s), "%s/%d", file, size);
   f = imlib_load_font(s);

   return f;
}

static void
Efont_free(EFont * f)
{
   imlib_context_set_font(f);
   imlib_free_font();
}

static void
Efont_extents(EFont * f, const char *text, int len __UNUSED__,
	      int *width, int *height, int *ascent)
{
   int                 h, asc, dsc;

   imlib_context_set_font(f);
   imlib_get_text_advance(text, width, &h);
   asc = imlib_get_font_ascent();
   dsc = imlib_get_font_descent();
   *height = asc + dsc;
   *ascent = asc;
}

static void
EFont_draw_string(EImage * im, EFont * f, int x, int y,
		  int r, int g, int b, const char *text)
{
   imlib_context_set_image(im);
   imlib_context_set_color(r, g, b, 255);
   imlib_context_set_font(f);
   imlib_text_draw(x, y - imlib_get_font_ascent(), text);
}

extern const FontOps FontOpsIft;

/*
 * Imlib2/FreeType
 */
extern const FontOps FontOpsIft;

static int
IftLoad(TextState * ts)
{
   char                s[4096], *s2, *ss;

   s2 = Estrdup(ts->fontname);
   if (!s2)
      return -1;
   ss = strchr(s2, '/');
   if (ss)
     {
	*ss++ = '\0';
	Esnprintf(s, sizeof(s), "%s.ttf", s2);
	ts->f.ift.font = Efont_load(s2, atoi(ss));
     }
   Efree(s2);

   if (!ts->f.ift.font)
      return -1;

   ts->need_utf8 = 1;
   ts->type = FONT_TYPE_IFT;
   ts->ops = &FontOpsIft;
   return 0;
}

static void
IftUnload(TextState * ts)
{
   Efont_free(ts->f.ift.font);
}

static void
IftTextSize(TextState * ts, const char *text, int len,
	    int *width, int *height, int *ascent)
{
   Efont_extents(ts->f.ift.font, text, len, width, height, ascent);
}

static void
IftTextDraw(TextState * ts, FontDrawContext * fdc, int x, int y,
	    const char *text, int len __UNUSED__)
{
   EFont_draw_string(fdc->im, ts->f.ift.font, x, y, fdc->r, fdc->g, fdc->b,
		     text);
}

static void
IftFdcInit(TextState * ts __UNUSED__, FontDrawContext * fdc __UNUSED__,
	   Win win __UNUSED__)
{
}

static void
IftFdcSetColor(TextState * ts __UNUSED__, FontDrawContext * fdc, XColor * xc)
{
   EGetColor(xc, &(fdc->r), &(fdc->g), &(fdc->b));
}

#if 0				/* Well... */
static void
IftTextMangle(TextState * ts, char **ptext, int *pw, int textwidth_limit)
{
   ts = NULL;
   ptext = NULL;
   pw = NULL;
   textwidth_limit = 0;
}
#endif

const FontOps       FontOpsIft = {
   IftLoad, IftUnload, IftTextSize, NULL, IftTextDraw,
   IftFdcInit, IftFdcSetColor
};

#if TEST_TTFONT
#include <time.h>

Display            *disp;

int
main(int argc, char **argv)
{
   Efont              *f;
   GC                  gc;
   XGCValues           gcv;
   Window              win;
   int                 i, j;

   disp = XOpenDisplay(NULL);

   imlib_context_set_display(disp);
   imlib_context_set_visual(DefaultVisual(disp, DefaultScreen(disp)));
   imlib_context_set_colormap(DefaultColormap(disp, DefaultScreen(disp)));

   srand(time(NULL));
   win = XCreateSimpleWindow(disp, DefaultRootWindow(disp), 0, 0, 640, 480, 0,
			     0, 0);
   XMapWindow(disp, win);
   XSync(disp, False);

   gcv.subwindow_mode = IncludeInferiors;
   gc = XCreateGC(disp, win, GCSubwindowMode, &gcv);
   for (;; j++)
     {
	for (i = 3; i < argc; i++)
	  {
	     XSetForeground(disp, gc, (rand() << 16 | rand()) & 0xffffff);
	     f = Efont_load(argv[i], atoi(argv[1]));
	     if (f)
		EFont_draw_string(win, gc, 20, atoi(argv[1]) * (i - 2),
				  argv[2], f,
				  DefaultVisual(disp, DefaultScreen(disp)),
				  DefaultColormap(disp, DefaultScreen(disp)));
	     Efont_free(f);
	     f = NULL;
	  }
     }
   return 0;
}

void
EGetColor(const XColor * pxc, int *pr, int *pg, int *pb)
{
   *pr = pxc->red >> 8;
   *pg = pxc->green >> 8;
   *pb = pxc->blue >> 8;
}

#endif

#endif /* FONT_TYPE_IFT */
