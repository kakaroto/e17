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
#include "backgrounds.h"
#include "conf.h"
#include "desktops.h"
#include "e16-ecore_list.h"
#include "eimage.h"
#include "emodule.h"
#include "iclass.h"
#include "tclass.h"
#include "xwin.h"

#if ENABLE_COLOR_MODIFIERS
typedef struct
{
   int                 num;
   unsigned char      *px;
   unsigned char      *py;
   unsigned char       map[256];
}
ModCurve;

typedef struct
{
   char               *name;
   ModCurve            red, green, blue;
   unsigned int        ref_count;
}
ColorModifierClass;
#endif

struct _imagestate
{
   char               *im_file;
   char               *real_file;
   char                unloadable;
   char                transparent;
   EImage             *im;
   EImageBorder       *border;
   int                 pixmapfillstyle;
   XColor              bg, hi, lo, hihi, lolo;
   int                 bevelstyle;
#if ENABLE_COLOR_MODIFIERS
   ColorModifierClass *colmod;
#endif
};

typedef struct
{
   ImageState         *normal;
   ImageState         *hilited;
   ImageState         *clicked;
   ImageState         *disabled;
}
ImageStateArray;

struct _imageclass
{
   char               *name;
   ImageStateArray     norm, active, sticky, sticky_active;
   EImageBorder        padding;
#if ENABLE_COLOR_MODIFIERS
   ColorModifierClass *colmod;
#endif
   unsigned int        ref_count;
};

static Ecore_List  *iclass_list = NULL;

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

static ImageState  *
ImagestateCreate(void)
{
   ImageState         *is;

   is = Emalloc(sizeof(ImageState));
   if (!is)
      return NULL;

   is->im_file = NULL;
   is->real_file = NULL;
   is->unloadable = 0;
   is->transparent = 0;
   is->im = NULL;
   is->border = NULL;
   is->pixmapfillstyle = FILL_STRETCH;
   ESetColor(&(is->bg), 160, 160, 160);
   ESetColor(&(is->hi), 200, 200, 200);
   ESetColor(&(is->lo), 120, 120, 120);
   ESetColor(&(is->hihi), 255, 255, 255);
   ESetColor(&(is->lolo), 64, 64, 64);
   is->bevelstyle = BEVEL_NONE;
#if ENABLE_COLOR_MODIFIERS
   is->colmod = NULL;
#endif

   return is;
}

static void
FreeImageState(ImageState * i)
{

   Efree(i->im_file);
   Efree(i->real_file);

   if (i->im)
     {
	EImageFree(i->im);
	i->im = NULL;
     }

   if (i->border)
      Efree(i->border);

#if ENABLE_COLOR_MODIFIERS
   if (i->colmod)
      i->colmod->ref_count--;
#endif
}

static void
FreeImageStateArray(ImageStateArray * isa)
{
   FreeImageState(isa->normal);
   Efree(isa->normal);
   FreeImageState(isa->hilited);
   Efree(isa->hilited);
   FreeImageState(isa->clicked);
   Efree(isa->clicked);
   FreeImageState(isa->disabled);
   Efree(isa->disabled);
}

static void
ImagestatePopulate(ImageState * is)
{
   if (!is)
      return;

   /* FIXME - Should be done elsewhere (ImagestateDrawBevel?) */
   EAllocColor(VRoot.cmap, &is->bg);
   EAllocColor(VRoot.cmap, &is->hi);
   EAllocColor(VRoot.cmap, &is->lo);
   EAllocColor(VRoot.cmap, &is->hihi);
   EAllocColor(VRoot.cmap, &is->lolo);
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

   ic = Ecalloc(1, sizeof(ImageClass));
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
#if ENABLE_COLOR_MODIFIERS
   ic->colmod = NULL;
#endif
   ic->ref_count = 0;

   return ic;
}

static void
ImageclassDestroy(ImageClass * ic)
{
   if (!ic)
      return;

   if (ic->ref_count > 0)
     {
	DialogOK(_("Imageclass Error!"), _("%u references remain\n"),
		 ic->ref_count);
	return;
     }

   ecore_list_remove_node(iclass_list, ic);

   if (ic->name)
      Efree(ic->name);

   FreeImageStateArray(&(ic->norm));
   FreeImageStateArray(&(ic->active));
   FreeImageStateArray(&(ic->sticky));
   FreeImageStateArray(&(ic->sticky_active));

#if ENABLE_COLOR_MODIFIERS
   if (ic->colmod)
      ic->colmod->ref_count--;
#endif
}

void
ImageclassIncRefcount(ImageClass * ic)
{
   ic->ref_count++;
}

void
ImageclassDecRefcount(ImageClass * ic)
{
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
   return strcmp(((const ImageClass *)data)->name, match);
}

ImageClass         *
ImageclassFind(const char *name, int fallback)
{
   ImageClass         *ic;

   if (name)
     {
	ic = ecore_list_find(iclass_list, _ImageclassMatchName, name);
	if (ic || !fallback)
	   return ic;
     }

   ic = ecore_list_find(iclass_list, _ImageclassMatchName, "__FALLBACK_ICLASS");

   return ic;
}

#define ISTATE_SET_STATE(which, fallback) \
   if (ic->which) ImagestatePopulate(ic->which); \
   else ic->which = ic->fallback;

#if ENABLE_COLOR_MODIFIERS
#define ISTATE_SET_CM(which, fallback) \
   if (!ic->which->colmod) { \
      ic->which->colmod = fallback; \
      if (fallback) fallback->ref_count++; \
     }
#endif

static void
ImageclassPopulate(ImageClass * ic)
{
#if ENABLE_COLOR_MODIFIERS
   ColorModifierClass *cm;
#endif

   if (!ic)
      return;

   if (!ic->norm.normal)
      return;

   ImagestatePopulate(ic->norm.normal);
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

#if ENABLE_COLOR_MODIFIERS
   if (!ic->colmod)
     {
	cm = FindItem("ICLASS", 0, LIST_FINDBY_NAME, LIST_TYPE_COLORMODIFIER);
	if (!cm)
	   cm =
	      FindItem("DEFAULT", 0, LIST_FINDBY_NAME, LIST_TYPE_COLORMODIFIER);
	ic->colmod = cm;
     }

   cm = FindItem("NORMAL", 0, LIST_FINDBY_NAME, LIST_TYPE_COLORMODIFIER);
   if (!cm)
      cm = ic->colmod;

   ISTATE_SET_CM(norm.normal, cm);
   ISTATE_SET_CM(norm.hilited, cm);
   ISTATE_SET_CM(norm.clicked, cm);
   ISTATE_SET_CM(norm.disabled, cm);

   cm = FindItem("ACTIVE", 0, LIST_FINDBY_NAME, LIST_TYPE_COLORMODIFIER);
   if (!cm)
      cm = ic->colmod;

   ISTATE_SET_CM(active.normal, cm);
   ISTATE_SET_CM(active.hilited, cm);
   ISTATE_SET_CM(active.clicked, cm);
   ISTATE_SET_CM(active.disabled, cm);

   cm = FindItem("STICKY", 0, LIST_FINDBY_NAME, LIST_TYPE_COLORMODIFIER);
   if (!cm)
      cm = ic->colmod;

   ISTATE_SET_CM(sticky.normal, cm);
   ISTATE_SET_CM(sticky.hilited, cm);
   ISTATE_SET_CM(sticky.clicked, cm);
   ISTATE_SET_CM(sticky.disabled, cm);

   cm = FindItem("STICKY_ACTIVE", 0, LIST_FINDBY_NAME, LIST_TYPE_COLORMODIFIER);
   if (!cm)
      cm = ic->colmod;

   ISTATE_SET_CM(sticky_active.normal, cm);
   ISTATE_SET_CM(sticky_active.hilited, cm);
   ISTATE_SET_CM(sticky_active.clicked, cm);
   ISTATE_SET_CM(sticky_active.disabled, cm);
#endif
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
   int                 fields;
   int                 l, r, t, b;

#if ENABLE_COLOR_MODIFIERS
   ColorModifierClass *cm = NULL;
#endif

   while (GetLine(s, sizeof(s), fs))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	   i1 = CONFIG_INVALID;
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
		Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
	  }

	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     ImageclassPopulate(ic);
	     goto done;
	  case ICLASS_LRTB:
	     {
		ICToRead->border = Emalloc(sizeof(EImageBorder));

		l = r = t = b = 0;
		sscanf(s, "%*s %i %i %i %i", &l, &r, &t, &b);
		ICToRead->border->left = l;
		ICToRead->border->right = r;
		ICToRead->border->top = t;
		ICToRead->border->bottom = b;
		/* Hmmm... imlib2 works better with this */
		ICToRead->border->right++;
		ICToRead->border->bottom++;
	     }
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
#if ENABLE_COLOR_MODIFIERS
		ic->colmod = ICToInherit->colmod;
#endif
	     }
	     break;
	  case CONFIG_COLORMOD:
	  case ICLASS_COLORMOD:
#if ENABLE_COLOR_MODIFIERS
	     cm = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_COLORMODIFIER);
	     if (cm)
	       {
		  if (ICToRead)
		    {
		       ICToRead->colmod = cm;
		    }
		  else
		    {
		       ic->colmod = cm;
		    }
		  cm->ref_count++;
	       }
#endif
	     break;
	  case ICLASS_PADDING:
	     {
		l = r = t = b = 0;
		sscanf(s, "%*s %i %i %i %i", &l, &r, &t, &b);
		ic->padding.left = l;
		ic->padding.right = r;
		ic->padding.top = t;
		ic->padding.bottom = b;
	     }
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
	     ic->norm.normal = ImagestateCreate();
	     ic->norm.normal->im_file = Estrdup(s2);
	     ICToRead = ic->norm.normal;
	     break;
	  case ICLASS_CLICKED:
	     ic->norm.clicked = ImagestateCreate();
	     ic->norm.clicked->im_file = Estrdup(s2);
	     ICToRead = ic->norm.clicked;
	     break;
	  case ICLASS_HILITED:
	     ic->norm.hilited = ImagestateCreate();
	     ic->norm.hilited->im_file = Estrdup(s2);
	     ICToRead = ic->norm.hilited;
	     break;
	  case ICLASS_DISABLED:
	     ic->norm.disabled = ImagestateCreate();
	     ic->norm.disabled->im_file = Estrdup(s2);
	     ICToRead = ic->norm.disabled;
	     break;
	  case ICLASS_STICKY_NORMAL:
	     ic->sticky.normal = ImagestateCreate();
	     ic->sticky.normal->im_file = Estrdup(s2);
	     ICToRead = ic->sticky.normal;
	     break;
	  case ICLASS_STICKY_CLICKED:
	     ic->sticky.clicked = ImagestateCreate();
	     ic->sticky.clicked->im_file = Estrdup(s2);
	     ICToRead = ic->sticky.clicked;
	     break;
	  case ICLASS_STICKY_HILITED:
	     ic->sticky.hilited = ImagestateCreate();
	     ic->sticky.hilited->im_file = Estrdup(s2);
	     ICToRead = ic->sticky.hilited;
	     break;
	  case ICLASS_STICKY_DISABLED:
	     ic->sticky.disabled = ImagestateCreate();
	     ic->sticky.disabled->im_file = Estrdup(s2);
	     ICToRead = ic->sticky.disabled;
	     break;
	  case ICLASS_ACTIVE_NORMAL:
	     ic->active.normal = ImagestateCreate();
	     ic->active.normal->im_file = Estrdup(s2);
	     ICToRead = ic->active.normal;
	     break;
	  case ICLASS_ACTIVE_CLICKED:
	     ic->active.clicked = ImagestateCreate();
	     ic->active.clicked->im_file = Estrdup(s2);
	     ICToRead = ic->active.clicked;
	     break;
	  case ICLASS_ACTIVE_HILITED:
	     ic->active.hilited = ImagestateCreate();
	     ic->active.hilited->im_file = Estrdup(s2);
	     ICToRead = ic->active.hilited;
	     break;
	  case ICLASS_ACTIVE_DISABLED:
	     ic->active.disabled = ImagestateCreate();
	     ic->active.disabled->im_file = Estrdup(s2);
	     ICToRead = ic->active.disabled;
	     break;
	  case ICLASS_STICKY_ACTIVE_NORMAL:
	     ic->sticky_active.normal = ImagestateCreate();
	     ic->sticky_active.normal->im_file = Estrdup(s2);
	     ICToRead = ic->sticky_active.normal;
	     break;
	  case ICLASS_STICKY_ACTIVE_CLICKED:
	     ic->sticky_active.clicked = ImagestateCreate();
	     ic->sticky_active.clicked->im_file = Estrdup(s2);
	     ICToRead = ic->sticky_active.clicked;
	     break;
	  case ICLASS_STICKY_ACTIVE_HILITED:
	     ic->sticky_active.hilited = ImagestateCreate();
	     ic->sticky_active.hilited->im_file = Estrdup(s2);
	     ICToRead = ic->sticky_active.hilited;
	     break;
	  case ICLASS_STICKY_ACTIVE_DISABLED:
	     ic->sticky_active.disabled = ImagestateCreate();
	     ic->sticky_active.disabled->im_file = Estrdup(s2);
	     ICToRead = ic->sticky_active.disabled;
	     break;
	  default:
	     Alert(_("Warning: unable to determine what to do with\n"
		     "the following text in the middle of current "
		     "ImageClass definition:\n"
		     "%s\nWill ignore and continue...\n"), s);
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
   ic->norm.normal = ImagestateCreate();
   ic->norm.normal->im_file = Estrdup(image);
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

Pixmap
ImageclassApplySimple(ImageClass * ic, Win win, Drawable draw, int state,
		      int x, int y, int w, int h)
{
   Pixmap              pmap;
   EImage             *im;

   im = ImageclassGetImage(ic, 0, 0, state);
   if (!im)
      return None;

   pmap = None;
   if (draw == None)
     {
	pmap = ECreatePixmap(win, w, h, 0);
	draw = pmap;
	x = y = 0;
     }

   EImageRenderOnDrawable(im, win, draw, x, y, w, h, 0);
   EImageFree(im);

   return pmap;
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
pt_get_bg_image(Window win, int w, int h, int use_root)
{
   EImage             *ii = NULL;
   Window              cr, dummy;
   Drawable            bg;
   int                 xx, yy;

   bg = DeskGetBackgroundPixmap(DesksGetCurrent());
   if (use_root || bg == None)
     {
	cr = VRoot.xwin;
	bg = VRoot.xwin;
     }
   else
     {
	cr = EoGetXwin(DesksGetCurrent());
     }
   XTranslateCoordinates(disp, win, cr, 0, 0, &xx, &yy, &dummy);
#if 0
   Eprintf("pt_get_bg_image %#lx %d %d %d %d\n", win, xx, yy, w, h);
#endif
   if (xx < VRoot.w && yy < VRoot.h && xx + w >= 0 && yy + h >= 0)
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
	bg = pt_get_bg_image(Xwin(win), w, h, flags & ICLASS_ATTR_GLASS);
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

 done:
   EImageFree(im);

   return bg;
}

static void
ImagestateMakePmapMask(ImageState * is, Win win, PmapMask * pmm,
		       int make_mask, int w, int h, int image_type)
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
	ii = pt_get_bg_image(Xwin(win), w, h, flags & ICLASS_ATTR_GLASS);
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
	EImageRenderOnDrawable(ii, win, pmap, 0, 0, w, h, 0);

	if (make_mask && !(flags & ICLASS_ATTR_NO_CLIP))
	  {
	     if (EImageHasAlpha(is->im))
	       {
		  /* Make the scaled clip mask to be used */
		  EImageRenderPixmaps(is->im, win, &pmap, &mask, w, h);

		  /* Replace the mask with the correct one */
		  pmm->mask = EXCreatePixmapCopy(mask, w, h, 1);

		  EImagePixmapFree(pmap);
	       }
	  }
	EImageDecache(ii);
     }
   else
#else
   make_mask = 0;
   image_type = 0;
#endif /* ENABLE_TRANSPARENCY */
   if (is->pixmapfillstyle == FILL_STRETCH)
     {
	pmm->type = 1;
	pmm->pmap = pmm->mask = None;
	pmm->w = w;
	pmm->h = h;
	EImageRenderPixmaps(is->im, win, &pmm->pmap, &pmm->mask, w, h);
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
	EImageRenderPixmaps(is->im, win, &pmm->pmap, &pmm->mask, pw, ph);
     }
}

static void
ImagestateDrawBevel(ImageState * is, Drawable win, GC gc, int w, int h)
{
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
ITApply(Win win, ImageClass * ic, ImageState * is, int w, int h,
	int state, int active, int sticky, int image_type,
	TextClass * tc, TextState * ts, const char *text)
{
   if (win == NoWin || !ic)
      return;

   /* FIXME - Why? */
   if (w <= 0 || h <= 0)
      EGetGeometry(win, NULL, NULL, NULL, &w, &h, NULL, NULL);
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

	ImagestateMakePmapMask(is, win, &pmm, 1, w, h, image_type);

	if (pmm.pmap)
	  {
	     Pixmap              pmap = pmm.pmap;

	     if (ts && text)
	       {
		  if (pmm.type != 0)
		    {
		       pmap = ECreatePixmap(win, w, h, 0);
		       EXCopyArea(pmm.pmap, pmap, 0, 0, w, h, 0, 0);
		    }

		  TextstateDrawText(ts, win, pmap, text, 0, 0, w, h,
				    &(ic->padding), 0,
				    TextclassGetJustification(tc));
	       }

	     /* Set window pixmap */
	     ESetWindowBackgroundPixmap(win, pmap);
	     if (pmap != pmm.pmap)
		EFreePixmap(pmap);

	     if (pmm.w == w && pmm.h == h)
		EShapeCombineMask(win, ShapeBounding, 0, 0, pmm.mask, ShapeSet);
	     else if (pmm.mask)
		EShapeCombineMaskTiled(win, ShapeBounding, 0, 0,
				       pmm.mask, ShapeSet, w, h);
	  }

	FreePmapMask(&pmm);
	EClearWindow(win);

	if ((is->unloadable) || (Conf.memory_paranoia))
	  {
	     EImageFree(is->im);
	     is->im = NULL;
	  }
     }
   else
     {
	/* FIXME - No text */
	ESetWindowBackground(win, is->bg.pixel);
	EClearWindow(win);
     }

   if (is->bevelstyle != BEVEL_NONE)
     {
	GC                  gc;

	gc = EXCreateGC(Xwin(win), 0, NULL);
	ImagestateDrawBevel(is, Xwin(win), gc, w, h);
	EXFreeGC(gc);
     }
}

void
ImageclassApply(ImageClass * ic, Win win, int w, int h, int active,
		int sticky, int state, int image_type)
{
   ITApply(win, ic, NULL, w, h, state, active, sticky, image_type,
	   NULL, NULL, NULL);
}

void
ImageclassApplyCopy(ImageClass * ic, Win win, int w, int h,
		    int active, int sticky, int state,
		    PmapMask * pmm, int make_mask, int image_type)
{
   ImageState         *is;
   GC                  gc;

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
	ImagestateMakePmapMask(is, win, pmm, make_mask, w, h, image_type);

	if (pmm->pmap)
	  {
	     if (pmm->w != w || pmm->h != h)
	       {
		  /* Create new full sized pixmaps and fill them with the */
		  /* pmap and mask tiles                                  */
		  Pixmap              tp = 0, tm = 0;
		  XGCValues           gcv;

		  tp = ECreatePixmap(win, w, h, 0);
		  gcv.fill_style = FillTiled;
		  gcv.tile = pmm->pmap;
		  gcv.ts_x_origin = 0;
		  gcv.ts_y_origin = 0;
		  gc = EXCreateGC(tp, GCFillStyle | GCTile |
				  GCTileStipXOrigin | GCTileStipYOrigin, &gcv);
		  XFillRectangle(disp, tp, gc, 0, 0, w, h);
		  EXFreeGC(gc);
		  if (pmm->mask)
		    {
		       tm = ECreatePixmap(win, w, h, 1);
		       gcv.fill_style = FillTiled;
		       gcv.tile = pmm->mask;
		       gcv.ts_x_origin = 0;
		       gcv.ts_y_origin = 0;
		       gc = EXCreateGC(tm, GCFillStyle | GCTile |
				       GCTileStipXOrigin | GCTileStipYOrigin,
				       &gcv);
		       XFillRectangle(disp, tm, gc, 0, 0, w, h);
		       EXFreeGC(gc);
		    }
		  FreePmapMask(pmm);
		  pmm->type = 0;
		  pmm->pmap = tp;
		  pmm->mask = tm;
	       }
	  }

	if ((is->unloadable) || (Conf.memory_paranoia))
	  {
	     EImageFree(is->im);
	     is->im = NULL;
	  }
     }
   else
     {
	Pixmap              pmap;

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

static void
ImageclassSetupFallback(void)
{
   ImageClass         *ic;

   /* create a fallback imageclass in case no imageclass can be found */
   ic = ImageclassCreate("__FALLBACK_ICLASS");

   ic->norm.normal = ImagestateCreate();
   ESetColor(&(ic->norm.normal->hihi), 255, 255, 255);
   ESetColor(&(ic->norm.normal->hi), 255, 255, 255);
   ESetColor(&(ic->norm.normal->bg), 160, 160, 160);
   ESetColor(&(ic->norm.normal->lo), 0, 0, 0);
   ESetColor(&(ic->norm.normal->lolo), 0, 0, 0);
   ic->norm.normal->bevelstyle = BEVEL_AMIGA;

   ic->norm.hilited = ImagestateCreate();
   ESetColor(&(ic->norm.hilited->hihi), 255, 255, 255);
   ESetColor(&(ic->norm.hilited->hi), 255, 255, 255);
   ESetColor(&(ic->norm.hilited->bg), 192, 192, 192);
   ESetColor(&(ic->norm.hilited->lo), 0, 0, 0);
   ESetColor(&(ic->norm.hilited->lolo), 0, 0, 0);
   ic->norm.hilited->bevelstyle = BEVEL_AMIGA;

   ic->norm.clicked = ImagestateCreate();
   ESetColor(&(ic->norm.clicked->hihi), 0, 0, 0);
   ESetColor(&(ic->norm.clicked->hi), 0, 0, 0);
   ESetColor(&(ic->norm.clicked->bg), 192, 192, 192);
   ESetColor(&(ic->norm.clicked->lo), 255, 255, 255);
   ESetColor(&(ic->norm.clicked->lolo), 255, 255, 255);
   ic->norm.clicked->bevelstyle = BEVEL_AMIGA;

   ic->active.normal = ImagestateCreate();
   ESetColor(&(ic->active.normal->hihi), 255, 255, 255);
   ESetColor(&(ic->active.normal->hi), 255, 255, 255);
   ESetColor(&(ic->active.normal->bg), 180, 140, 160);
   ESetColor(&(ic->active.normal->lo), 0, 0, 0);
   ESetColor(&(ic->active.normal->lolo), 0, 0, 0);
   ic->active.normal->bevelstyle = BEVEL_AMIGA;

   ic->active.hilited = ImagestateCreate();
   ESetColor(&(ic->active.hilited->hihi), 255, 255, 255);
   ESetColor(&(ic->active.hilited->hi), 255, 255, 255);
   ESetColor(&(ic->active.hilited->bg), 230, 190, 210);
   ESetColor(&(ic->active.hilited->lo), 0, 0, 0);
   ESetColor(&(ic->active.hilited->lolo), 0, 0, 0);
   ic->active.hilited->bevelstyle = BEVEL_AMIGA;

   ic->active.clicked = ImagestateCreate();
   ESetColor(&(ic->active.clicked->hihi), 0, 0, 0);
   ESetColor(&(ic->active.clicked->hi), 0, 0, 0);
   ESetColor(&(ic->active.clicked->bg), 230, 190, 210);
   ESetColor(&(ic->active.clicked->lo), 255, 255, 255);
   ESetColor(&(ic->active.clicked->lolo), 255, 255, 255);
   ic->active.clicked->bevelstyle = BEVEL_AMIGA;

   ic->padding.left = 8;
   ic->padding.right = 8;
   ic->padding.top = 8;
   ic->padding.bottom = 8;

   ImageclassPopulate(ic);

   /* Create all black image class for filler borders */
   ic = ImageclassCreate("__BLACK");

   ic->norm.normal = ImagestateCreate();
   ESetColor(&(ic->norm.normal->hihi), 0, 0, 0);
   ESetColor(&(ic->norm.normal->hi), 0, 0, 0);
   ESetColor(&(ic->norm.normal->bg), 0, 0, 0);
   ESetColor(&(ic->norm.normal->lo), 0, 0, 0);
   ESetColor(&(ic->norm.normal->lolo), 0, 0, 0);

   ImageclassPopulate(ic);
}

/*
 * Imageclass Module
 */

static void
ImageclassSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_INIT:
	ImageclassSetupFallback();
	break;
     }
}

static void
ImageclassIpc(const char *params, Client * c __UNUSED__)
{
   char                param1[FILEPATH_LEN_MAX];
   char                param2[FILEPATH_LEN_MAX];
   char                param3[FILEPATH_LEN_MAX];
   ImageClass         *ic;

   if (!params)
     {
	IpcPrintf("Please specify...\n");
	return;
     }

   param1[0] = 0;
   param2[0] = 0;
   param3[0] = 0;

   word(params, 1, param1);
   word(params, 2, param2);

   if (!strncmp(param1, "list", 2))
     {
	ECORE_LIST_FOR_EACH(iclass_list, ic) IpcPrintf("%s\n", ic->name);
	return;
     }

   if (!strcmp(param2, "create"))
     {
     }
   else if (!strcmp(param2, "delete"))
     {
	ic = ImageclassFind(param1, 0);
	if (ic)
	   ImageclassDestroy(ic);
     }
   else if (!strcmp(param2, "modify"))
     {
     }
   else if (!strcmp(param2, "free_pixmap"))
     {
	Pixmap              p;

	word(params, 3, param3);
	p = (Pixmap) strtol(param3, (char **)NULL, 0);
	EImagePixmapFree(p);
     }
   else if (!strcmp(param2, "get_padding"))
     {
	ic = ImageclassFind(param1, 0);
	if (ic)
	   IpcPrintf("%i %i %i %i\n",
		     ic->padding.left, ic->padding.right,
		     ic->padding.top, ic->padding.bottom);
	else
	   IpcPrintf("Error: Imageclass does not exist\n");
     }
   else if (!strcmp(param2, "get_image_size"))
     {
	ic = ImageclassFind(param1, 0);
	if (ic)
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
	else
	   IpcPrintf("Error: Imageclass does not exist\n");
     }
   else if (!strcmp(param2, "apply"))
     {
	ic = ImageclassFind(param1, 0);
	if (ic)
	  {
	     Window              xwin;
	     Win                 win;
	     char                state[20];
	     const char         *winptr, *hptr;
	     int                 st, w = -1, h = -1;

	     winptr = atword(params, 3);
	     xwin = (Window) strtoul(winptr, NULL, 0);
	     win = ECreateWinFromXwin(xwin);
	     if (!win)
		return;

	     word(params, 4, state);
	     if (!strcmp(state, "hilited"))
		st = STATE_HILITED;
	     else if (!strcmp(state, "clicked"))
		st = STATE_CLICKED;
	     else if (!strcmp(state, "disabled"))
		st = STATE_DISABLED;
	     else
		st = STATE_NORMAL;

	     hptr = atword(params, 6);
	     if (hptr)
	       {
		  w = (int)strtol(atword(params, 5), NULL, 0);
		  h = (int)strtol(hptr, NULL, 0);
	       }

	     ImageclassApply(ic, win, w, h, 0, 0, st, ST_SOLID);
	     EDestroyWin(win);
	  }
     }
   else if (!strcmp(param2, "apply_copy"))
     {
	ic = ImageclassFind(param1, 0);
	if (ic)
	  {
	     Window              xwin;
	     Win                 win;
	     char                state[20];
	     const char         *winptr, *hptr;
	     int                 st, w = -1, h = -1;
	     PmapMask            pmm;

	     winptr = atword(params, 3);
	     xwin = (Window) strtoul(winptr, NULL, 0);

	     word(params, 4, state);
	     if (!strcmp(state, "hilited"))
		st = STATE_HILITED;
	     else if (!strcmp(state, "clicked"))
		st = STATE_CLICKED;
	     else if (!strcmp(state, "disabled"))
		st = STATE_DISABLED;
	     else
		st = STATE_NORMAL;

	     hptr = atword(params, 6);
	     if (!hptr)
	       {
		  IpcPrintf("Error:  missing width and/or height\n");
		  return;
	       }

	     w = (int)strtol(atword(params, 5), NULL, 0);
	     h = (int)strtol(hptr, NULL, 0);

	     win = ECreateWinFromXwin(xwin);
	     if (!win)
		return;

	     ImageclassApplyCopy(ic, win, w, h, 0, 0, st, &pmm, 1, ST_SOLID);
	     IpcPrintf("0x%08lx 0x%08lx\n", pmm.pmap, pmm.mask);
	     EDestroyWin(win);
	  }
     }
   else if (!strcmp(param2, "ref_count"))
     {
	ic = ImageclassFind(param1, 0);
	if (ic)
	   IpcPrintf("%u references remain\n", ic->ref_count);
     }
   else if (!strcmp(param2, "query"))
     {
	ic = ImageclassFind(param1, 0);
	if (ic)
	   IpcPrintf("ImageClass %s found\n", ic->name);
	else
	   IpcPrintf("ImageClass %s not found\n", param1);
     }
   else
     {
	IpcPrintf("Error: unknown operation specified\n");
     }
}

static const IpcItem ImageclassIpcArray[] = {
   {
    ImageclassIpc,
    "imageclass", NULL,
    "List imageclasses, create/delete/modify/apply an imageclass",
    NULL}
   ,
};
#define N_IPC_FUNCS (sizeof(ImageclassIpcArray)/sizeof(IpcItem))

/*
 * Module descriptor
 */
const EModule       ModImageclass = {
   "imageclass", "ic",
   ImageclassSighan,
   {N_IPC_FUNCS, ImageclassIpcArray}
   ,
   {0, NULL}
};
