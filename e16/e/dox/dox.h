/*****************************************************************************/
/* Enlightenment - The Window Manager that dares to do what others don't     */
/*****************************************************************************/
/*
  Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to
  deal in the Software without restriction, including without limitation the
  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
  sell copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
 
  The above copyright notice and this permission notice shall be included in
  all copies of the Software, its documentation and marketing & publicity
  materials, and acknowledgment shall be given in the documentation, materials
  and software packages that this Software was used.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  */

#include "config.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/Xlocale.h>
#include <X11/cursorfont.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/XShm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define ESetColor(pxc, r, g, b) \
	({ (pxc)->red = ((r)<<8)|r; (pxc)->green = ((g)<<8)|g; (pxc)->blue = ((b)<<8)|b; })
#define EGetColor(pxc, pr, pg, pb) \
	({ *(pr) = ((pxc)->red)>>8; *(pg) = ((pxc)->green)>>8; *(pb) = ((pxc)->blue)>>8; })

#if USE_IMLIB2
#include <Imlib2.h>

#define IMLIB1_SET_CONTEXT(root_ctx)

#define EAllocColor(pxc) \
	XAllocColor(disp, root.cmap, pxc)

#define IMLIB_FREE_PIXMAP_AND_MASK(pmap, mask) \
	imlib_free_pixmap_and_mask(pmap)

#define IC_RenderDepth() DefaultDepth(disp, root.scr)

#else

#include <Imlib.h>

extern ImlibData   *pI1Ctx;
extern ImlibImage  *pIcImg;
extern Drawable     vIcDrw;

#define IMLIB1_SET_CONTEXT(root_ctx) \
	pI1Ctx = ((root_ctx) && prImlib_Context) ? prImlib_Context : pImlib_Context

#define imlib_context_set_image(im_img) \
	pIcImg = im_img
#define imlib_context_set_drawable(im_drw) \
	vIcDrw = im_drw

#define imlib_context_set_dither(onoff) \
	Imlib_set_render_type(pI1Ctx, RT_DITHER_TRUECOL)
#define imlib_context_get_dither() \
	Imlib_get_render_type(pI1Ctx)

#define imlib_image_get_width() \
	pIcImg->rgb_width
#define imlib_image_get_height() \
	pIcImg->rgb_height

#define imlib_load_image(file) \
	Imlib_load_image(pI1Ctx, file)
#define imlib_create_image_from_drawable(mask, x, y, w, h, grab) \
	Imlib_create_image_from_drawable(pI1Ctx, vIcDrw, mask, x, y, w, h)

#define imlib_image_set_format(fmt)
#define imlib_save_image(file) \
	Imlib_save_image_to_ppm(pI1Ctx, pIcImg, file)

#define imlib_render_pixmaps_for_whole_image(p, m) \
	Imlib_render(pI1Ctx, pIcImg, imlib_image_get_width(), imlib_image_get_height()); \
	if (p) *p = Imlib_copy_image(pI1Ctx, pIcImg); \
	if (m) *m = Imlib_copy_mask(pI1Ctx, pIcImg)
#define imlib_render_pixmaps_for_whole_image_at_size(p, m, w, h) \
	Imlib_render(pI1Ctx, pIcImg, w, h); \
	if (p) *p = Imlib_copy_image(pI1Ctx, pIcImg); \
	if (m) *m = Imlib_copy_mask(pI1Ctx, pIcImg)
#define imlib_render_image_on_drawable(x, y) \
	Imlib_apply_image(pI1Ctx, pIcImg, vIcDrw)
#define imlib_render_image_on_drawable_at_size(x, y, w, h) \
	Imlib_paste_image(pI1Ctx, pIcImg, vIcDrw, x, y, w, h)

#define imlib_create_cropped_scaled_image(x, y, w, h, w2, h2) \
	Imlib_clone_scaled_image(pI1Ctx, pIcImg, w2, h2)

#define imlib_image_orientate(rot) \
	switch (rot) { \
	case 1: \
		Imlib_rotate_image(pI1Ctx, pIcImg, 1); \
		Imlib_flip_image_horizontal(pI1Ctx, pIcImg); \
		break; \
	case 2: \
        	Imlib_flip_image_vertical(pI1Ctx, pIcImg); \
        	Imlib_flip_image_horizontal(pI1Ctx, pIcImg); \
	case 3: \
		Imlib_rotate_image(pI1Ctx, pIcImg, -1); \
		Imlib_flip_image_vertical(pI1Ctx, pIcImg); \
		break; \
	}

#define imlib_free_image() \
	({ Imlib_destroy_image(pI1Ctx, pIcImg); pIcImg = NULL; })
#define imlib_free_image_and_decache() \
	({ Imlib_kill_image(pI1Ctx, pIcImg); pIcImg = NULL; })
#define imlib_free_pixmap_and_mask(pmap) \
	Imlib_free_pixmap(pI1Ctx, pmap)

#define IMLIB_FREE_PIXMAP_AND_MASK(pmap, mask) \
	({ Imlib_free_pixmap(pI1Ctx, pmap); Imlib_free_pixmap(pI1Ctx, mask); })

#define imlib_image_set_border(im_bdr) \
	Imlib_set_image_border(pI1Ctx, pIcImg, im_bdr)

#define EAllocColor(pxc) \
	({ int r = ((pxc)->red)>>8, g = ((pxc)->green)>>8, b = ((pxc)->blue)>>8; \
		(pxc)->pixel = Imlib_best_color_match(pI1Ctx, &r, &g, &b); })

#define Imlib_Context ImlibData
#define Imlib_Image ImlibImage
#define Imlib_Color ImlibColor
#define Imlib_Border ImlibBorder
#define IC_RenderDepth() (pImlib_Context->x.render_depth)
#endif

#if USE_FNLIB
#include <Fnlib.h>
#endif

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif

#define FILEPATH_LEN_MAX 4096

#define DEFAULT_LINKCOLOR_R 30
#define DEFAULT_LINKCOLOR_G 50
#define DEFAULT_LINKCOLOR_B 160

#ifndef MAX
#define MAX(a,b)  ((a)>(b)?(a):(b))
#endif

/* ISO encodings */
#define ENCOING_ISO_8859_1 0
#define ENCOING_ISO_8859_2 1
#define ENCOING_ISO_8859_3 2
#define ENCOING_ISO_8859_4 3

typedef struct _efont Efont;

typedef struct _root
{
   Window              win;
   Visual             *vis;
   int                 depth;
   Colormap            cmap;
   int                 scr;
   int                 w, h;
}
Root;

typedef struct _textstate
{
   char               *fontname;
#if USE_FNLIB
   FnlibStyle          style;
   FnlibFont          *font;
#endif
   XColor              fg_col;
   XColor              bg_col;
   int                 effect;
   Efont              *efont;
   XFontStruct        *xfont;
   XFontSet            xfontset;
   int                 xfontset_ascent;
   int                 height;
}
TextState;

typedef enum _type
{
   IMG,
   BR,
   FONT,
   P,
   TEXT,
   PAGE
}
Type;

typedef struct _img
{
   char               *src;
   char               *src2;
   char               *src3;
   int                 x, y;
   char               *link;
   int                 w, h;
}
Img_;

typedef struct _font
{
   char               *face;
   int                 r, g, b;
}
Font_;

typedef struct _p
{
   float               align;
}
P_;

typedef struct _object
{
   Type                type;
   void               *object;
}
Object;

typedef struct _page
{
   char               *name;
   int                 count;
   Object             *obj;
   int                 columns;
   int                 padding;
   int                 linkr, linkg, linkb;
   char               *background;
}
Page;

typedef struct _link
{
   char               *name;
   int                 x, y, w, h;
   struct _link       *next;
}
Link;

void                Efont_extents(Efont * f, const char *text,
				  int *font_ascent_return,
				  int *font_descent_return, int *width_return,
				  int *max_ascent_return,
				  int *max_descent_return,
				  int *lbearing_return, int *rbearing_return);
Efont              *Efont_load(char *file, int size);
void                Efont_free(Efont * f);
void                EFont_draw_string(Display * disp, Drawable win, GC gc,
				      int x, int y, char *text,
				      Efont * font, Visual * vis, Colormap cm);

char              **TextGetLines(const char *text, int *count);
void                TextStateLoadFont(TextState * ts);
void                TextSize(TextState * ts, const char *text,
			     int *width, int *height, int fsize);
void                TextDraw(TextState * ts, Window win, char *text,
			     int x, int y, int w, int h, int fsize,
			     int justification);

void                freestrlist(char **l, int num);
void                word(char *s, int num, char *wd);
void                word_mb(char *s, int num, char *wd, int *spaceflag);
int                 findLocalizedFile(char *fname);

void                AddPage(Object * obj);
void                AddObject(Object * obj);
void                BuildObj(Object * obj, char *var, char *param);
int                 GetNextTag(Object * obj);
char               *GetTextUntilTag(void);
int                 GetObjects(FILE * f);
int                 FixPage(int p);
int                 GetPage(char *name);
void                GetLinkColors(int page_num, int *r, int *g, int *b);
Link               *RenderPage(Window win, int page_num, int w, int h);

extern Display     *disp;
extern Root         root;

#if USE_FNLIB
extern FnlibData   *pFnlibData;
#endif
extern char        *docdir;

#define Emalloc malloc
#define Efree   free

#define Esnprintf snprintf
