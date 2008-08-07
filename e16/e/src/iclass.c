/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2008 Kim Woelders
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
#include "backgrounds.h"
#include "conf.h"
#include "desktops.h"
#include "e16-ecore_list.h"
#include "eimage.h"
#include "emodule.h"
#include "iclass.h"
#include "tclass.h"
#include "xwin.h"

#define ENABLE_DESTROY 0	/* Broken */

struct _imagestate {
   char               *im_file;
   char               *real_file;
   char                got_colors;
   char                unloadable;
   char                transparent;
   EImage             *im;
   EImageBorder       *border;
   int                 pixmapfillstyle;
   EColor              bg, hi, lo, hihi, lolo;
   int                 bevelstyle;
};

typedef struct {
   ImageState         *normal;
   ImageState         *hilited;
   ImageState         *clicked;
   ImageState         *disabled;
} ImageStateArray;

struct _imageclass {
   char               *name;
   ImageStateArray     norm, active, sticky, sticky_active;
   EImageBorder        padding;
   unsigned int        ref_count;
};

static Ecore_List  *iclass_list = NULL;

static ImageClass  *ImageclassGetFallback(void);

#ifdef ENABLE_THEME_TRANSPARENCY

static EImageColorModifier *icm = NULL;
static unsigned char gray[256];
static unsigned char alpha[256];

static int          prev_alpha = -1;

int
TransparencyEnabled(void)
{
   return Conf.trans.alpha;
}

int
TransparencyUpdateNeeded(void)
{
   return Conf.trans.alpha || prev_alpha;
}

static void
TransparencyMakeColorModifier(void)
{
   int                 i;

   for (i = 0; i < 256; i++)
     {
	gray[i] = i;
	alpha[i] = 255 - Conf.trans.alpha;
     }

   if (icm == NULL)
      icm = EImageColorModifierCreate();
   EImageColorModifierSetTables(icm, gray, gray, gray, alpha);
}

void
TransparencySet(int transparency)
{
   int                 changed;

   if (transparency < 0)
      transparency = 0;
   else if (transparency > 255)
      transparency = 255;

   /*  This will render the initial startup stuff correctly since !changed  */
   if (prev_alpha == -1)
     {
	prev_alpha = Conf.trans.alpha = transparency;
	changed = -1;
     }
   else
     {
	changed = Conf.trans.alpha != transparency;
	prev_alpha = Conf.trans.alpha;
	Conf.trans.alpha = transparency;
     }

   if (!changed)
      return;

   /* Generate the color modifier tables */
   TransparencyMakeColorModifier();

   if (changed < 0)
      return;

   if (prev_alpha == 0)
     {
	/* Hack to get tiled backgrounds regenerated at full size */
	BackgroundsInvalidate(1);
     }
   ModulesSignal(ESIGNAL_THEME_TRANS_CHANGE, NULL);
}

#endif /* ENABLE_THEME_TRANSPARENCY */

EImage             *
ThemeImageLoad(const char *file)
{
   EImage             *im;
   char               *f;

   if (!file)
      return NULL;

   if (file[0] == '/')
     {
	im = EImageLoad(file);
	return im;
     }

   f = ThemeFileFind(file, 0);
   if (f)
     {
	im = EImageLoad(f);
	Efree(f);
	return im;
     }

   return NULL;
}

static void
ImagestateColorsSetGray(ImageState * is,
			unsigned int hihi, unsigned int hi,
			unsigned int bg, unsigned int lo, unsigned int lolo)
{
   SET_COLOR(&(is->hihi), hihi, hihi, hihi);
   SET_COLOR(&(is->hi), hi, hi, hi);
   SET_COLOR(&(is->bg), bg, bg, bg);
   SET_COLOR(&(is->lo), lo, lo, lo);
   SET_COLOR(&(is->lolo), lolo, lolo, lolo);
}

static ImageState  *
ImagestateCreate(const char *file)
{
   ImageState         *is;

   is = ECALLOC(ImageState, 1);
   if (!is)
      return NULL;

   is->pixmapfillstyle = FILL_STRETCH;
   ImagestateColorsSetGray(is, 255, 200, 160, 120, 64);
   is->bevelstyle = BEVEL_NONE;
   is->im_file = Estrdup(file);

   return is;
}

#if ENABLE_DESTROY
static void
ImagestateDestroy(ImageState * is)
{
   if (!is)
      return;

   Efree(is->im_file);
   Efree(is->real_file);

   if (is->im)
      EImageFree(is->im);

   Efree(is->border);

   Efree(is);
}

static void
FreeImageStateArray(ImageStateArray * isa)
{
   ImagestateDestroy(isa->normal);
   ImagestateDestroy(isa->hilited);
   ImagestateDestroy(isa->clicked);
   ImagestateDestroy(isa->disabled);
}
#endif /* ENABLE_DESTROY */

static void
ImagestateColorsAlloc(ImageState * is)
{
   if (!is || is->got_colors)
      return;

   EAllocColor(WinGetCmap(VROOT), &is->bg);
   EAllocColor(WinGetCmap(VROOT), &is->hi);
   EAllocColor(WinGetCmap(VROOT), &is->lo);
   EAllocColor(WinGetCmap(VROOT), &is->hihi);
   EAllocColor(WinGetCmap(VROOT), &is->lolo);

   is->got_colors = 1;
}

static void
ImagestateRealize(ImageState * is)
{
   if (is == NULL || is->im_file == NULL)
      return;

   /* has bg pixmap */
   if (is->im)
      return;

   /* not loaded, load and setup */
   if (!is->real_file)
      is->real_file = ThemeFileFind(is->im_file, 0);
   if (is->real_file)
      is->im = EImageLoad(is->real_file);
   if (!is->im)
     {
	Eprintf
	   ("ImagestateRealize: Hmmm... is->im is NULL (im_file=%s real_file=%s)\n",
	    is->im_file, is->real_file);
	return;
     }

   EImageCheckAlpha(is->im);

   if (is->border)
      EImageSetBorder(is->im, is->border);

#if 0				/* To be implemented? */
   if (is->colmod)
     {
	Imlib_set_image_red_curve(pImlib_Context, is->im, is->colmod->red.map);
	Imlib_set_image_green_curve(pImlib_Context, is->im,
				    is->colmod->green.map);
	Imlib_set_image_blue_curve(pImlib_Context, is->im,
				   is->colmod->blue.map);
     }
#endif
}

static ImageClass  *
ImageclassCreate(const char *name)
{
   ImageClass         *ic;

   ic = ECALLOC(ImageClass, 1);
   if (!ic)
      return NULL;

   if (!iclass_list)
      iclass_list = ecore_list_new();
   ecore_list_prepend(iclass_list, ic);

   ic->name = Estrdup(name);
   ic->norm.normal = ic->norm.hilited = ic->norm.clicked = ic->norm.disabled =
      NULL;
   ic->active.normal = ic->active.hilited = ic->active.clicked =
      ic->active.disabled = NULL;
   ic->sticky.normal = ic->sticky.hilited = ic->sticky.clicked =
      ic->sticky.disabled = NULL;
   ic->sticky_active.normal = ic->sticky_active.hilited =
      ic->sticky_active.clicked = ic->sticky_active.disabled = NULL;
   ic->padding.left = 0;
   ic->padding.right = 0;
   ic->padding.top = 0;
   ic->padding.bottom = 0;
   ic->ref_count = 0;

   return ic;
}

#if ENABLE_DESTROY
static void
ImageclassDestroy(ImageClass * ic)
{
   if (!ic)
      return;

   if (ic->ref_count > 0)
     {
	DialogOK("ImageClass Error!", _("%u references remain\n"),
		 ic->ref_count);
	return;
     }

   ecore_list_node_remove(iclass_list, ic);

   Efree(ic->name);

   FreeImageStateArray(&(ic->norm));
   FreeImageStateArray(&(ic->active));
   FreeImageStateArray(&(ic->sticky));
   FreeImageStateArray(&(ic->sticky_active));

   Efree(ic);
}
#endif /* ENABLE_DESTROY */

ImageClass         *
ImageclassAlloc(const char *name, int fallback)
{
   ImageClass         *ic;

   if (!name || !name[0])
      return NULL;

   ic = ImageclassFind(name, fallback);
   if (ic)
      ic->ref_count++;

   return ic;
}

void
ImageclassFree(ImageClass * ic)
{
   if (ic)
      ic->ref_count--;
}

const char         *
ImageclassGetName(ImageClass * ic)
{
   return (ic) ? ic->name : NULL;
}

EImageBorder       *
ImageclassGetPadding(ImageClass * ic)
{
   return (ic) ? &(ic->padding) : NULL;
}

static int
_ImageclassMatchName(const void *data, const void *match)
{
   return strcmp(((const ImageClass *)data)->name, (const char *)match);
}

ImageClass         *
ImageclassFind(const char *name, int fallback)
{
   ImageClass         *ic = NULL;

   if (name)
      ic = (ImageClass *) ecore_list_find(iclass_list, _ImageclassMatchName,
					  name);
   if (ic || !fallback)
      return ic;

#if 0
   Eprintf("%s: Get fallback (%s)\n", __func__, name);
#endif
   return ImageclassGetFallback();
}

#define ISTATE_SET_STATE(which, fallback) \
   if (!ic->which) ic->which = ic->fallback;

static void
ImageclassPopulate(ImageClass * ic)
{
   if (!ic)
      return;

   if (!ic->norm.normal)
      return;

   ISTATE_SET_STATE(norm.hilited, norm.normal);
   ISTATE_SET_STATE(norm.clicked, norm.normal);
   ISTATE_SET_STATE(norm.disabled, norm.normal);

   ISTATE_SET_STATE(active.normal, norm.normal);
   ISTATE_SET_STATE(active.hilited, active.normal);
   ISTATE_SET_STATE(active.clicked, active.normal);
   ISTATE_SET_STATE(active.disabled, active.normal);

   ISTATE_SET_STATE(sticky.normal, norm.normal);
   ISTATE_SET_STATE(sticky.hilited, sticky.normal);
   ISTATE_SET_STATE(sticky.clicked, sticky.normal);
   ISTATE_SET_STATE(sticky.disabled, sticky.normal);

   ISTATE_SET_STATE(sticky_active.normal, norm.normal);
   ISTATE_SET_STATE(sticky_active.hilited, sticky_active.normal);
   ISTATE_SET_STATE(sticky_active.clicked, sticky_active.normal);
   ISTATE_SET_STATE(sticky_active.disabled, sticky_active.normal);
}

int
ImageclassConfigLoad(FILE * fs)
{
   int                 err = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   ImageClass         *ic = NULL;
   ImageState         *ICToRead = NULL;
   int                 l, r, t, b;

   while (GetLine(s, sizeof(s), fs))
     {
	i1 = ConfigParseline1(s, s2, NULL, NULL);
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     ImageclassPopulate(ic);
	     goto done;
	  case ICLASS_LRTB:
	     ICToRead->border = EMALLOC(EImageBorder, 1);

	     l = r = t = b = 0;
	     sscanf(s, "%*s %i %i %i %i", &l, &r, &t, &b);
	     ICToRead->border->left = l;
	     ICToRead->border->right = r;
	     ICToRead->border->top = t;
	     ICToRead->border->bottom = b;
	     /* Hmmm... imlib2 works better with this */
	     ICToRead->border->right++;
	     ICToRead->border->bottom++;
	     break;
	  case ICLASS_FILLRULE:
	     ICToRead->pixmapfillstyle = atoi(s2);
	     break;
	  case ICLASS_TRANSPARENT:
	     ICToRead->transparent = strtoul(s2, NULL, 0);
	     break;
	  case CONFIG_INHERIT:
	     {
		ImageClass         *ICToInherit;

		ICToInherit = ImageclassFind(s2, 0);
		ic->norm = ICToInherit->norm;
		ic->active = ICToInherit->active;
		ic->sticky = ICToInherit->sticky;
		ic->sticky_active = ICToInherit->sticky_active;
		ic->padding = ICToInherit->padding;
	     }
	     break;
	  case CONFIG_COLORMOD:
	  case ICLASS_COLORMOD:
	     break;
	  case ICLASS_PADDING:
	     l = r = t = b = 0;
	     sscanf(s, "%*s %i %i %i %i", &l, &r, &t, &b);
	     ic->padding.left = l;
	     ic->padding.right = r;
	     ic->padding.top = t;
	     ic->padding.bottom = b;
	     break;
	  case CONFIG_CLASSNAME:
	  case ICLASS_NAME:
	     if (ImageclassFind(s2, 0))
	       {
		  SkipTillEnd(fs);
		  goto done;
	       }
	     ic = ImageclassCreate(s2);
	     break;
	  case CONFIG_DESKTOP:
	     /* don't ask... --mandrake */
	  case ICLASS_NORMAL:
	     ic->norm.normal = ICToRead = ImagestateCreate(s2);
	     break;
	  case ICLASS_CLICKED:
	     ic->norm.clicked = ICToRead = ImagestateCreate(s2);
	     break;
	  case ICLASS_HILITED:
	     ic->norm.hilited = ICToRead = ImagestateCreate(s2);
	     break;
	  case ICLASS_DISABLED:
	     ic->norm.disabled = ICToRead = ImagestateCreate(s2);
	     break;
	  case ICLASS_STICKY_NORMAL:
	     ic->sticky.normal = ICToRead = ImagestateCreate(s2);
	     break;
	  case ICLASS_STICKY_CLICKED:
	     ic->sticky.clicked = ICToRead = ImagestateCreate(s2);
	     break;
	  case ICLASS_STICKY_HILITED:
	     ic->sticky.hilited = ICToRead = ImagestateCreate(s2);
	     break;
	  case ICLASS_STICKY_DISABLED:
	     ic->sticky.disabled = ICToRead = ImagestateCreate(s2);
	     break;
	  case ICLASS_ACTIVE_NORMAL:
	     ic->active.normal = ICToRead = ImagestateCreate(s2);
	     break;
	  case ICLASS_ACTIVE_CLICKED:
	     ic->active.clicked = ICToRead = ImagestateCreate(s2);
	     break;
	  case ICLASS_ACTIVE_HILITED:
	     ic->active.hilited = ICToRead = ImagestateCreate(s2);
	     break;
	  case ICLASS_ACTIVE_DISABLED:
	     ic->active.disabled = ICToRead = ImagestateCreate(s2);
	     break;
	  case ICLASS_STICKY_ACTIVE_NORMAL:
	     ic->sticky_active.normal = ICToRead = ImagestateCreate(s2);
	     break;
	  case ICLASS_STICKY_ACTIVE_CLICKED:
	     ic->sticky_active.clicked = ICToRead = ImagestateCreate(s2);
	     break;
	  case ICLASS_STICKY_ACTIVE_HILITED:
	     ic->sticky_active.hilited = ICToRead = ImagestateCreate(s2);
	     break;
	  case ICLASS_STICKY_ACTIVE_DISABLED:
	     ic->sticky_active.disabled = ICToRead = ImagestateCreate(s2);
	     break;
	  default:
	     ConfigParseError("ImageClass", s);
	     break;
	  }
     }
   err = -1;

 done:
   return err;
}

ImageClass         *
ImageclassCreateSimple(const char *name, const char *image)
{
   ImageClass         *ic;

   ic = ImageclassCreate(name);
   if (!ic)
      return NULL;

   ic->norm.normal = ImagestateCreate(image);
   ic->norm.normal->unloadable = 1;
   ImageclassPopulate(ic);

   ImagestateRealize(ic->norm.normal);

   return ic;
}

#ifdef ENABLE_THEME_TRANSPARENCY
int
ImageclassIsTransparent(ImageClass * ic)
{
   return ic && ic->norm.normal && ic->norm.normal->transparent;
}
#endif

static ImageState  *
ImageclassGetImageState1(ImageStateArray * pisa, int state)
{
   ImageState         *is;

   switch (state)
     {
     case STATE_NORMAL:
	is = pisa->normal;
	break;
     case STATE_HILITED:
	is = pisa->hilited;
	break;
     case STATE_CLICKED:
	is = pisa->clicked;
	break;
     case STATE_DISABLED:
	is = pisa->disabled;
	break;
     default:
	is = NULL;
	break;
     }

   return is;
}

ImageState         *
ImageclassGetImageState(ImageClass * ic, int state, int active, int sticky)
{
   ImageState         *is;

   if (active)
     {
	if (sticky)
	   is = ImageclassGetImageState1(&ic->sticky_active, state);
	else
	   is = ImageclassGetImageState1(&ic->active, state);
     }
   else
     {
	if (sticky)
	   is = ImageclassGetImageState1(&ic->sticky, state);
	else
	   is = ImageclassGetImageState1(&ic->norm, state);
     }

   return is;
}

EImage             *
ImageclassGetImage(ImageClass * ic, int active, int sticky, int state)
{
   EImage             *im;
   ImageState         *is;

   if (!ic)
      return NULL;

   is = ImageclassGetImageState(ic, state, active, sticky);
   if (!is)
      return NULL;

   if (is->im == NULL && is->im_file)
      ImagestateRealize(is);

   im = is->im;
   if (!im)
      return NULL;
   is->im = NULL;

   return im;
}

#ifdef ENABLE_TRANSPARENCY
static int
pt_type_to_flags(int image_type)
{
   int                 flags;

   if (Conf.trans.alpha == 0)
      return ICLASS_ATTR_OPAQUE;

   switch (image_type)
     {
     default:
     case ST_SOLID:
     case ST_BUTTON:
	flags = ICLASS_ATTR_OPAQUE;
	break;
     case ST_BORDER:
	flags = Conf.trans.border;
	break;
     case ST_WIDGET:
	flags = Conf.trans.widget;
	break;
     case ST_ICONBOX:
	flags = Conf.trans.iconbox;
	break;
     case ST_MENU:
	flags = Conf.trans.menu;
	break;
     case ST_MENU_ITEM:
	flags = Conf.trans.menu_item;
	break;
     case ST_TOOLTIP:
	flags = Conf.trans.tooltip;
	break;
     case ST_DIALOG:
	flags = Conf.trans.dialog;
	break;
     case ST_HILIGHT:
	flags = Conf.trans.hilight;
	break;
     case ST_PAGER:
	flags = Conf.trans.pager;
	break;
     case ST_WARPLIST:
	flags = Conf.trans.warplist;
	break;
     }
   if (flags != ICLASS_ATTR_OPAQUE)
      flags |= ICLASS_ATTR_USE_CM;

   return flags;
}

static EImage      *
pt_get_bg_image(Win win, int w, int h, int use_root)
{
   EImage             *ii = NULL;
   Win                 cr;
   Drawable            bg;
   int                 xx, yy;

   bg = DeskGetBackgroundPixmap(DesksGetCurrent());
   if (use_root || bg == None)
     {
	cr = VROOT;
	bg = WinGetXwin(VROOT);
     }
   else
     {
	cr = EoGetWin(DesksGetCurrent());
     }
   ETranslateCoordinates(win, cr, 0, 0, &xx, &yy, NULL);
#if 0
   Eprintf("pt_get_bg_image %#lx %d %d %d %d\n", win, xx, yy, w, h);
#endif
   if (xx < WinGetW(VROOT) && yy < WinGetH(VROOT) && xx + w >= 0 && yy + h >= 0)
     {
	/* Create the background base image */
	ii = EImageGrabDrawable(bg, None, xx, yy, w, h, !EServerIsGrabbed());
     }

   return ii;
}

#endif

EImage             *
ImageclassGetImageBlended(ImageClass * ic, Win win, int w, int h, int active,
			  int sticky, int state, int image_type)
{
   EImage             *im, *bg;
   int                 flags;

   if (!ic)
      return NULL;

   im = ImageclassGetImage(ic, active, sticky, state);
   if (!im)
      return NULL;

#ifdef ENABLE_TRANSPARENCY
   flags = pt_type_to_flags(image_type);
   if (flags != ICLASS_ATTR_OPAQUE)
     {
	bg = pt_get_bg_image(win, w, h, flags & ICLASS_ATTR_GLASS);
	if (bg)
	  {
	     EImageBlendCM(bg, im, (flags & ICLASS_ATTR_USE_CM) ? icm : NULL);
	     goto done;
	  }
     }
#else
   flags = image_type;
   win = None;
#endif

   bg = EImageCreateScaled(im, 0, 0, 0, 0, w, h);

#ifdef ENABLE_TRANSPARENCY
 done:
#endif
   EImageFree(im);

   return bg;
}

static void
ImagestateMakePmapMask(ImageState * is, Win win, PmapMask * pmm,
		       int pmapflags, int w, int h, int image_type)
{
#ifdef ENABLE_TRANSPARENCY
   EImage             *ii = NULL;
   int                 flags;
   Pixmap              pmap, mask;

   flags = pt_type_to_flags(image_type);

   /*
    * is->transparent flags:
    *   0x01: Use desktop background pixmap as base
    *   0x02: Use root window as base (use only for transients, if at all)
    *   0x04: Don't apply image mask to result
    */
   if (is->transparent && EImageHasAlpha(is->im))
      flags = is->transparent;

   if (flags != ICLASS_ATTR_OPAQUE)
     {
	ii = pt_get_bg_image(win, w, h, flags & ICLASS_ATTR_GLASS);
     }
   else
     {
#if 0
	Eprintf("ImagestateMakePmapMask %#lx %d %d\n", win, w, h);
#endif
     }

   if (ii)
     {
	EImageBlendCM(ii, is->im, (flags & ICLASS_ATTR_USE_CM) ? icm : NULL);

	pmm->type = 0;
	pmm->pmap = pmap = ECreatePixmap(win, w, h, 0);
	pmm->mask = None;
	pmm->w = w;
	pmm->h = h;
	EImageRenderOnDrawable(ii, win, pmap, 0, 0, 0, w, h);

	if ((pmapflags & IC_FLAG_MAKE_MASK) && !(flags & ICLASS_ATTR_NO_CLIP))
	  {
	     if (EImageHasAlpha(is->im))
	       {
		  /* Make the scaled clip mask to be used */
		  EImageRenderPixmaps(is->im, win, EIMAGE_ANTI_ALIAS, &pmap,
				      &mask, w, h);

		  /* Replace the mask with the correct one */
		  pmm->mask = EXCreatePixmapCopy(mask, w, h, 1);

		  EImagePixmapsFree(pmap, mask);
	       }
	  }
	EImageDecache(ii);
     }
   else
#else
   pmapflags = 0;
   image_type = 0;
#endif /* ENABLE_TRANSPARENCY */
   if (is->pixmapfillstyle == FILL_STRETCH)
     {
	pmm->type = 1;
	pmm->pmap = pmm->mask = None;
	pmm->w = w;
	pmm->h = h;
	EImageRenderPixmaps(is->im, win, EIMAGE_ANTI_ALIAS, &pmm->pmap,
			    &pmm->mask, w, h);
     }
   else
     {
	int                 ww, hh, cw, ch, pw, ph;

	EImageGetSize(is->im, &ww, &hh);

	pw = w;
	ph = h;
	if (is->pixmapfillstyle & FILL_TILE_H)
	   pw = ww;
	if (is->pixmapfillstyle & FILL_TILE_V)
	   ph = hh;
	if (is->pixmapfillstyle & FILL_INT_TILE_H)
	  {
	     cw = w / ww;
	     if (cw * ww < w)
		cw++;
	     if (cw < 1)
		cw = 1;
	     pw = w / cw;
	  }
	if (is->pixmapfillstyle & FILL_INT_TILE_V)
	  {
	     ch = h / hh;
	     if (ch * hh < h)
		ch++;
	     if (ch < 1)
		ch = 1;
	     ph = h / ch;
	  }
	pmm->type = 1;
	pmm->pmap = pmm->mask = None;
	pmm->w = pw;
	pmm->h = ph;
	EImageRenderPixmaps(is->im, win, EIMAGE_ANTI_ALIAS, &pmm->pmap,
			    &pmm->mask, pw, ph);
     }
}

static void
ImagestateDrawBevel(ImageState * is, Drawable win, GC gc, int w, int h)
{
   ImagestateColorsAlloc(is);

   switch (is->bevelstyle)
     {
     case BEVEL_AMIGA:
	XSetForeground(disp, gc, is->hihi.pixel);
	XDrawLine(disp, win, gc, 0, 0, w - 2, 0);
	XDrawLine(disp, win, gc, 0, 0, 0, h - 2);
	XSetForeground(disp, gc, is->lolo.pixel);
	XDrawLine(disp, win, gc, 1, h - 1, w - 1, h - 1);
	XDrawLine(disp, win, gc, w - 1, 1, w - 1, h - 1);
	break;
     case BEVEL_MOTIF:
	XSetForeground(disp, gc, is->hi.pixel);
	XDrawLine(disp, win, gc, 0, 0, w - 1, 0);
	XDrawLine(disp, win, gc, 0, 0, 0, h - 1);
	XDrawLine(disp, win, gc, 1, 1, w - 2, 1);
	XDrawLine(disp, win, gc, 1, 1, 1, h - 2);
	XSetForeground(disp, gc, is->lo.pixel);
	XDrawLine(disp, win, gc, 0, h - 1, w - 1, h - 1);
	XDrawLine(disp, win, gc, w - 1, 1, w - 1, h - 1);
	XDrawLine(disp, win, gc, 1, h - 2, w - 2, h - 2);
	XDrawLine(disp, win, gc, w - 2, 2, w - 2, h - 2);
	break;
     case BEVEL_NEXT:
	XSetForeground(disp, gc, is->hihi.pixel);
	XDrawLine(disp, win, gc, 0, 0, w - 1, 0);
	XDrawLine(disp, win, gc, 0, 0, 0, h - 1);
	XSetForeground(disp, gc, is->hi.pixel);
	XDrawLine(disp, win, gc, 1, 1, w - 2, 1);
	XDrawLine(disp, win, gc, 1, 1, 1, h - 2);
	XSetForeground(disp, gc, is->lolo.pixel);
	XDrawLine(disp, win, gc, 1, h - 1, w - 1, h - 1);
	XDrawLine(disp, win, gc, w - 1, 1, w - 1, h - 1);
	XSetForeground(disp, gc, is->lo.pixel);
	XDrawLine(disp, win, gc, 2, h - 2, w - 2, h - 2);
	XDrawLine(disp, win, gc, w - 2, 2, w - 2, h - 2);
	break;
     case BEVEL_DOUBLE:
	XSetForeground(disp, gc, is->hi.pixel);
	XDrawLine(disp, win, gc, 0, 0, w - 2, 0);
	XDrawLine(disp, win, gc, 0, 0, 0, h - 2);
	XSetForeground(disp, gc, is->lo.pixel);
	XDrawLine(disp, win, gc, 1, 1, w - 3, 1);
	XDrawLine(disp, win, gc, 1, 1, 1, h - 3);
	XSetForeground(disp, gc, is->lo.pixel);
	XDrawLine(disp, win, gc, 1, h - 1, w - 1, h - 1);
	XDrawLine(disp, win, gc, w - 1, 1, w - 1, h - 1);
	XSetForeground(disp, gc, is->hi.pixel);
	XDrawLine(disp, win, gc, 2, h - 2, w - 2, h - 2);
	XDrawLine(disp, win, gc, w - 2, 2, w - 2, h - 2);
	break;
     case BEVEL_WIDEDOUBLE:
	XSetForeground(disp, gc, is->hihi.pixel);
	XDrawLine(disp, win, gc, 0, 0, w - 1, 0);
	XDrawLine(disp, win, gc, 0, 0, 0, h - 1);
	XSetForeground(disp, gc, is->hi.pixel);
	XDrawLine(disp, win, gc, 1, 1, w - 2, 1);
	XDrawLine(disp, win, gc, 1, 1, 1, h - 2);
	XDrawLine(disp, win, gc, 3, h - 4, w - 4, h - 4);
	XDrawLine(disp, win, gc, w - 4, 3, w - 4, h - 4);
	XSetForeground(disp, gc, is->lolo.pixel);
	XDrawLine(disp, win, gc, 1, h - 1, w - 1, h - 1);
	XDrawLine(disp, win, gc, w - 1, 1, w - 1, h - 1);
	XSetForeground(disp, gc, is->lo.pixel);
	XDrawLine(disp, win, gc, 2, h - 2, w - 2, h - 2);
	XDrawLine(disp, win, gc, w - 2, 2, w - 2, h - 2);
	XDrawLine(disp, win, gc, 3, 3, w - 4, 3);
	XDrawLine(disp, win, gc, 3, 3, 3, h - 4);
	break;
     case BEVEL_THINPOINT:
	XSetForeground(disp, gc, is->hi.pixel);
	XDrawLine(disp, win, gc, 0, 0, w - 2, 0);
	XDrawLine(disp, win, gc, 0, 0, 0, h - 2);
	XSetForeground(disp, gc, is->lo.pixel);
	XDrawLine(disp, win, gc, 1, h - 1, w - 1, h - 1);
	XDrawLine(disp, win, gc, w - 1, 1, w - 1, h - 1);
	XSetForeground(disp, gc, is->hihi.pixel);
	XDrawLine(disp, win, gc, 0, 0, 1, 0);
	XDrawLine(disp, win, gc, 0, 0, 0, 1);
	XSetForeground(disp, gc, is->lolo.pixel);
	XDrawLine(disp, win, gc, w - 2, h - 1, w - 1, h - 1);
	XDrawLine(disp, win, gc, w - 1, h - 2, w - 1, h - 1);
	break;
     case BEVEL_THICKPOINT:
	XSetForeground(disp, gc, is->hi.pixel);
	XDrawRectangle(disp, win, gc, 0, 0, w - 1, h - 1);
	break;
     default:
	break;
     }
}

void
ITApply(Win win, ImageClass * ic, ImageState * is,
	int state, int active, int sticky, int image_type,
	TextClass * tc, TextState * ts, const char *text, int flags)
{
   int                 w, h;

   if (win == NULL || !ic)
      return;

   w = WinGetW(win);
   h = WinGetH(win);
   if (w <= 0 || h <= 0)
      return;

   if (!is)
      is = ImageclassGetImageState(ic, state, active, sticky);
   if (!is)
      return;

   if (tc && text)
     {
	if (!ts)
	   ts = TextclassGetTextState(tc, state, active, sticky);
     }

   if (is->im == NULL && is->im_file)
      ImagestateRealize(is);

   /* Imlib2 will not render pixmaps with dimensions > 8192 */
   if (is->im && w <= 8192 && h <= 8192)
     {
	PmapMask            pmm;

	ImagestateMakePmapMask(is, win, &pmm, IC_FLAG_MAKE_MASK, w, h,
			       image_type);

	if (pmm.pmap)
	  {
	     Pixmap              pmap = pmm.pmap;

	     if (ts && text)
	       {
		  if (pmm.type != 0)
		    {
		       pmap = EGetWindowBackgroundPixmap(win);
		       EXCopyArea(pmm.pmap, pmap, 0, 0, w, h, 0, 0);
		    }

		  TextstateTextDraw(ts, win, pmap, text, 0, 0, w, h,
				    &(ic->padding), 0,
				    TextclassGetJustification(tc), flags);
	       }

	     /* Set window pixmap */
	     if (pmap == pmm.pmap)
	       {
		  ESetWindowBackgroundPixmap(win, pmap);
		  EFreeWindowBackgroundPixmap(win);
	       }

	     if (pmm.w == w && pmm.h == h)
		EShapeSetMask(win, 0, 0, pmm.mask);
	     else if (pmm.mask)
		EShapeSetMaskTiled(win, 0, 0, pmm.mask, w, h);
	  }

	FreePmapMask(&pmm);

	if ((is->unloadable) || (Conf.memory_paranoia))
	  {
	     EImageFree(is->im);
	     is->im = NULL;
	  }
     }
   else
     {
	ImagestateColorsAlloc(is);

	if (is->bevelstyle == BEVEL_NONE && !text)
	  {
	     ESetWindowBackground(win, is->bg.pixel);
	  }
	else
	  {
	     Pixmap              pmap;
	     GC                  gc;

	     pmap = EGetWindowBackgroundPixmap(win);
	     gc = EXCreateGC(WinGetXwin(win), 0, NULL);
	     XSetFillStyle(disp, gc, FillSolid);
	     XSetForeground(disp, gc, is->bg.pixel);
	     XFillRectangle(disp, pmap, gc, 0, 0, w, h);
	     if (is->bevelstyle != BEVEL_NONE)
		ImagestateDrawBevel(is, pmap, gc, w, h);
	     if (ts && text)
		TextstateTextDraw(ts, win, pmap, text, 0, 0, w, h,
				  &(ic->padding), 0,
				  TextclassGetJustification(tc), flags);
	     EXFreeGC(gc);
	  }
     }
   EClearWindow(win);
}

void
ImageclassApply(ImageClass * ic, Win win, int active, int sticky, int state,
		int image_type)
{
   ITApply(win, ic, NULL, state, active, sticky, image_type, NULL, NULL, NULL,
	   0);
}

static void
PmapMaskTile(PmapMask * pmm, Win win, unsigned int w, unsigned int h)
{
   Pixmap              pmap, mask;

   pmap = ECreatePixmap(win, w, h, 0);
   if (pmap == None)
      return;
   EXCopyAreaTiled(pmm->pmap, None, pmap, 0, 0, w, h, 0, 0);

   mask = None;
   if (pmm->mask != None)
      mask = ECreatePixmap(win, w, h, 1);
   if (mask != None)
      EXCopyAreaTiled(pmm->mask, None, mask, 0, 0, w, h, 0, 0);

   FreePmapMask(pmm);
   pmm->type = 0;
   pmm->w = w;
   pmm->h = h;
   pmm->pmap = pmap;
   pmm->mask = mask;
}

void
ImageclassApplyCopy(ImageClass * ic, Win win, int w, int h,
		    int active, int sticky, int state,
		    PmapMask * pmm, int pmapflags, int image_type)
{
   ImageState         *is;

   if (pmm == NULL)
      return;

   pmm->type = 0;
   pmm->pmap = pmm->mask = 0;

   if ((!ic) || (!win) || (w <= 0) || (h <= 0))
      return;

   is = ImageclassGetImageState(ic, state, active, sticky);
   if (!is)
      return;

   if (is->im == NULL && is->im_file)
      ImagestateRealize(is);

   /* Imlib2 will not render pixmaps with dimensions > 8192 */
   if (is->im && w <= 8192 && h <= 8192)
     {
	ImagestateMakePmapMask(is, win, pmm, pmapflags, w, h, image_type);

	if ((pmapflags & IC_FLAG_FULL_SIZE) && pmm->pmap &&
	    (pmm->w != w || pmm->h != h))
	  {
	     /* Create new full sized pixmaps and fill them with the */
	     /* pmap and mask tiles                                  */
	     PmapMaskTile(pmm, win, w, h);
	  }

	if ((is->unloadable) || (Conf.memory_paranoia))
	  {
	     EImageFree(is->im);
	     is->im = NULL;
	  }
     }
   else
     {
	GC                  gc;
	Pixmap              pmap;

	ImagestateColorsAlloc(is);

	if (pmm->pmap)
	   Eprintf("ImageclassApplyCopy: Hmm... pmm->pmap already set\n");

	pmap = ECreatePixmap(win, w, h, 0);
	pmm->type = 0;
	pmm->pmap = pmap;
	pmm->mask = 0;

	gc = EXCreateGC(pmap, 0, NULL);
	/* bg color */
	XSetForeground(disp, gc, is->bg.pixel);
	XFillRectangle(disp, pmap, gc, 0, 0, w, h);
	/* if there is a bevel to draw, draw it */
	if (is->bevelstyle != BEVEL_NONE)
	   ImagestateDrawBevel(is, pmap, gc, w, h);
	EXFreeGC(gc);
	/* FIXME - No text */
     }
}

void
ImageclassApplySimple(ImageClass * ic, Win win, Drawable draw, int state,
		      int x, int y, int w, int h)
{
   EImage             *im;

   im = ImageclassGetImage(ic, 0, 0, state);
   if (!im)
      return;

   EImageRenderOnDrawable(im, win, draw, 0, x, y, w, h);
   EImageFree(im);
}

static ImageClass  *
ImageclassGetFallback(void)
{
   ImageClass         *ic;

   ic = ImageclassFind("__fb_ic", 0);
   if (ic)
      return ic;

   /* Create fallback imageclass */
   ic = ImageclassCreate("__fb_ic");
   if (!ic)
      return ic;

   ic->norm.normal = ImagestateCreate(NULL);
   ImagestateColorsSetGray(ic->norm.normal, 255, 255, 160, 0, 0);
   ic->norm.normal->bevelstyle = BEVEL_AMIGA;

   ic->norm.hilited = ImagestateCreate(NULL);
   ImagestateColorsSetGray(ic->norm.hilited, 255, 255, 192, 0, 0);
   ic->norm.hilited->bevelstyle = BEVEL_AMIGA;

   ic->norm.clicked = ImagestateCreate(NULL);
   ImagestateColorsSetGray(ic->norm.clicked, 0, 0, 192, 255, 255);
   ic->norm.clicked->bevelstyle = BEVEL_AMIGA;

   ic->active.normal = ImagestateCreate(NULL);
   ImagestateColorsSetGray(ic->active.normal, 255, 255, 0, 0, 0);
   SET_COLOR(&(ic->active.normal->bg), 180, 140, 160);
   ic->active.normal->bevelstyle = BEVEL_AMIGA;

   ic->active.hilited = ImagestateCreate(NULL);
   ImagestateColorsSetGray(ic->active.hilited, 255, 255, 0, 0, 0);
   SET_COLOR(&(ic->active.hilited->bg), 230, 190, 210);
   ic->active.hilited->bevelstyle = BEVEL_AMIGA;

   ic->active.clicked = ImagestateCreate(NULL);
   ImagestateColorsSetGray(ic->active.clicked, 0, 0, 0, 255, 255);
   SET_COLOR(&(ic->active.clicked->bg), 230, 190, 210);
   ic->active.clicked->bevelstyle = BEVEL_AMIGA;

   ic->padding.left = 4;
   ic->padding.right = 4;
   ic->padding.top = 4;
   ic->padding.bottom = 4;

   ImageclassPopulate(ic);

   return ic;
}

ImageClass         *
ImageclassGetBlack(void)
{
   ImageClass         *ic;

   ic = ImageclassFind("__BLACK", 0);
   if (ic)
      return ic;

   /* Create all black image class for filler borders */
   ic = ImageclassCreate("__BLACK");
   if (!ic)
      return ic;

   ic->norm.normal = ImagestateCreate(NULL);
   ImagestateColorsSetGray(ic->norm.normal, 0, 0, 0, 0, 0);

   ImageclassPopulate(ic);

   return ic;
}

/*
 * Imageclass Module
 */

static void
ImageclassIpc(const char *params)
{
   char                param1[1024];
   char                param2[1024];
   int                 l;
   const char         *p;
   ImageClass         *ic;

   if (!params)
     {
	IpcPrintf("Please specify...\n");
	return;
     }

   p = params;
   l = 0;
   param1[0] = param2[0] = '\0';
   sscanf(p, "%1000s %1000s %n", param1, param2, &l);
   p += l;

   if (!strncmp(param1, "list", 2))
     {
	ECORE_LIST_FOR_EACH(iclass_list, ic) IpcPrintf("%s\n", ic->name);
	return;
     }

   if (!param1[0])
     {
	IpcPrintf("ImageClass not specified\n");
	return;
     }

   if (!strcmp(param2, "create"))
     {
	/* Not implemented */
	return;
     }
   else if (!strcmp(param2, "free_pixmap"))
     {
	Pixmap              pmap;

	pmap = (Pixmap) strtol(p, NULL, 0);
	EImagePixmapsFree(pmap, None);
	return;
     }

   ic = ImageclassFind(param1, 0);
   if (!ic)
     {
	IpcPrintf("ImageClass not found: %s\n", param1);
	return;
     }

   if (!strcmp(param2, "delete"))
     {
#if ENABLE_DESTROY
	ImageclassDestroy(ic);
#endif
     }
   else if (!strcmp(param2, "modify"))
     {
	/* Not implemented */
     }
   else if (!strcmp(param2, "get_padding"))
     {
	IpcPrintf("%i %i %i %i\n",
		  ic->padding.left, ic->padding.right,
		  ic->padding.top, ic->padding.bottom);
     }
   else if (!strcmp(param2, "get_image_size"))
     {
	ImagestateRealize(ic->norm.normal);
	if (ic->norm.normal->im)
	  {
	     int                 w, h;

	     EImageGetSize(ic->norm.normal->im, &w, &h);
	     EImageFree(ic->norm.normal->im);
	     IpcPrintf("%i %i\n", w, h);
	  }
     }
   else if (!strcmp(param2, "apply"))
     {
	Window              xwin;
	Win                 win;
	char                state[20];
	int                 st, w, h;

	/* 3:xwin 4:state 5:w 6:h */
	xwin = None;
	state[0] = '\0';
	w = h = -1;
	sscanf(p, "%lx %16s %d %d", &xwin, state, &w, &h);

	win = ECreateWinFromXwin(xwin);
	if (!win)
	   return;

	if (!strcmp(state, "normal"))
	   st = STATE_NORMAL;
	else if (!strcmp(state, "hilited"))
	   st = STATE_HILITED;
	else if (!strcmp(state, "clicked"))
	   st = STATE_CLICKED;
	else if (!strcmp(state, "disabled"))
	   st = STATE_DISABLED;
	else
	   st = STATE_NORMAL;

	ImageclassApply(ic, win, 0, 0, st, ST_SOLID);
	EDestroyWin(win);
     }
   else if (!strcmp(param2, "apply_copy"))
     {
	Window              xwin;
	Win                 win;
	char                state[20];
	int                 st, w, h;
	PmapMask            pmm;

	/* 3:xwin 4:state 5:w 6:h */
	xwin = None;
	state[0] = '\0';
	w = h = -1;
	sscanf(p, "%lx %16s %d %d", &xwin, state, &w, &h);

	win = ECreateWinFromXwin(xwin);
	if (!win)
	   return;

	if (!strcmp(state, "normal"))
	   st = STATE_NORMAL;
	else if (!strcmp(state, "hilited"))
	   st = STATE_HILITED;
	else if (!strcmp(state, "clicked"))
	   st = STATE_CLICKED;
	else if (!strcmp(state, "disabled"))
	   st = STATE_DISABLED;
	else
	   st = STATE_NORMAL;

	if (w < 0 || h < 0)
	  {
	     IpcPrintf("Error:  missing width and/or height\n");
	     return;
	  }

	ImageclassApplyCopy(ic, win, w, h, 0, 0, st, &pmm,
			    IC_FLAG_MAKE_MASK | IC_FLAG_FULL_SIZE, ST_SOLID);
	IpcPrintf("0x%08lx 0x%08lx\n", pmm.pmap, pmm.mask);
	EDestroyWin(win);
     }
   else if (!strcmp(param2, "query"))
     {
	IpcPrintf("ImageClass %s found\n", ic->name);
     }
   else if (!strcmp(param2, "ref_count"))
     {
	IpcPrintf("%u references remain\n", ic->ref_count);
     }
   else
     {
	IpcPrintf("Error: unknown operation specified\n");
     }
}

static const IpcItem ImageclassIpcArray[] = {
   {
    ImageclassIpc,
    "imageclass", "ic",
    "List imageclasses, apply an imageclass",
    NULL}
   ,
};
#define N_IPC_FUNCS (sizeof(ImageclassIpcArray)/sizeof(IpcItem))

/*
 * Module descriptor
 */
extern const EModule ModImageclass;
const EModule       ModImageclass = {
   "imageclass", "ic",
   NULL,
   {N_IPC_FUNCS, ImageclassIpcArray}
   ,
   {0, NULL}
};
