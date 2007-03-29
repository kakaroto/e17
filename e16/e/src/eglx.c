/*
 * Copyright (C) 2007 Kim Woelders
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
#define GLX_GLXEXT_PROTOTYPES 1
#include "E.h"
#include "eglx.h"
#include "eimage.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xrender.h>

#define ENABLE_DEBUG   1
#if ENABLE_DEBUG
#define Dprintf(fmt...)  do { if(EDebug(EDBUG_TYPE_GLX))Eprintf(fmt); } while(0)
#define D2printf(fmt...) do { if(EDebug(EDBUG_TYPE_GLX)>1)Eprintf(fmt); } while(0)
#else
#define Dprintf(fmt...)
#define D2printf(fmt...)
#endif /* ENABLE_DEBUG */

#define ETEX_TYPE_IMAGE      1
#define ETEX_TYPE_PIXMAP     2

#if 1
#define TEXTURE_TARGET GL_TEXTURE_2D
#else
#define TEXTURE_TARGET GLX_TEXTURE_RECTANGLE_EXT
#endif

static void         EobjTexturesFree(void);

typedef struct
{
   XVisualInfo        *vi;
   GLXContext          ctx;
   GLXFBConfig         fbc;
   unsigned            ctx_initialised:1;
} EGlContext;

static EGlContext   egl;

int
EGlInit(void)
{
/* From NV's README.txt (AddARGBGLXVisuals) */
   static const int    attrs[] = {
      GLX_RENDER_TYPE, GLX_RGBA_BIT,
      GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
      GLX_RED_SIZE, 1,
      GLX_GREEN_SIZE, 1,
      GLX_BLUE_SIZE, 1,
      GLX_ALPHA_SIZE, 1,
      GLX_DOUBLEBUFFER, True,
      GLX_DEPTH_SIZE, 1,
      GLX_VISUAL_CAVEAT_EXT, GLX_NONE_EXT,
      0
   };
   int                 screen = DefaultScreen(disp);
   XVisualInfo        *vi;

   memset(&egl, 0, sizeof(EGlContext));

   Dprintf("EGlInit\n");

   /* Create a GLX context */
   GLXFBConfig        *fbc;
   int                 i, ix, num;
   int                 value;
   char               *s;
   XID                 vid = None;
   XRenderPictFormat  *pictFormat;

   s = getenv("EVISUAL");
   if (s)
     {
	vid = strtoul(s, NULL, 0);
	Eprintf("Want Visual Id=%#lx\n", vid);
     }

   fbc = glXChooseFBConfig(disp, screen, attrs, &num);
   if (!fbc)
     {
	Eprintf("No FB configs\n");
	return -1;
     }

   D2printf("Visuals found: %d\n", num);
   ix = -1;
   for (i = 0; i < num; i++)
     {
	vi = glXGetVisualFromFBConfig(disp, fbc[i]);
	if (!vi)
	   continue;

	D2printf("Checking Visual ID=%#lx depth=%d\n", vi->visualid, vi->depth);
	if (vid && vi->visualid != vid)
	   continue;

#if 0
	glXGetFBConfigAttrib(disp, fbc[i], GLX_VISUAL_CAVEAT_EXT, &value);
	D2printf("  GLX_VISUAL_CAVEAT_EXT=%#x (want %#x)\n", value,
		 GLX_NONE_EXT);
	if (value != GLX_NONE_EXT)
	   continue;
#endif

	glXGetFBConfigAttrib(disp, fbc[i], GLX_DRAWABLE_TYPE, &value);
	D2printf("  GLX_DRAWABLE_TYPE=%#x (want %#x)\n", value, GLX_PIXMAP_BIT);
#if 0
	if (!(value & GLX_PIXMAP_BIT))
	   continue;
#endif

	glXGetFBConfigAttrib(disp, fbc[i], GLX_TEXTURE_FORMAT_EXT, &value);
	D2printf("  GLX_TEXTURE_FORMAT_EXT=%#x\n", value);

	glXGetFBConfigAttrib(disp, fbc[i], GLX_BIND_TO_TEXTURE_TARGETS_EXT,
			     &value);
	D2printf("  GLX_BIND_TO_TEXTURE_TARGETS_EXT=%#x (want %#x)\n", value,
		 GLX_TEXTURE_2D_BIT_EXT);
#if 0
	if (!(value & GLX_TEXTURE_2D_BIT_EXT))
	   continue;
#endif

	glXGetFBConfigAttrib(disp, fbc[i], GLX_BIND_TO_TEXTURE_RGBA_EXT,
			     &value);
	D2printf("  GLX_BIND_TO_TEXTURE_RGBA_EXT=%x (want %x)\n", value, 1);
	glXGetFBConfigAttrib(disp, fbc[i], GLX_BIND_TO_TEXTURE_RGB_EXT, &value);
	D2printf("  GLX_BIND_TO_TEXTURE_RGB_EXT=%x (want %x)\n", value, 1);
	if (!value)
	   continue;

	glXGetFBConfigAttrib(disp, fbc[i], GLX_BIND_TO_MIPMAP_TEXTURE_EXT,
			     &value);
	D2printf("  GLX_BIND_TO_MIPMAP_TEXTURE_EXT=%x\n", value);

	glXGetFBConfigAttrib(disp, fbc[i], GLX_Y_INVERTED_EXT, &value);
	D2printf("  GLX_Y_INVERTED_EXT=%x\n", value);

#if 1
	/* We want an ARGB visual */
	pictFormat = XRenderFindVisualFormat(disp, vi->visual);
	if (!pictFormat)
	   continue;
	if (pictFormat->direct.alphaMask == 0)
	   continue;
#endif

	D2printf(" - passed\n");
	if (ix < 0)
	   ix = i;

	XFree(vi);
     }

   if (ix >= 0)
      egl.fbc = fbc[ix];
   XFree(fbc);

   if (ix < 0)
     {
	Eprintf("No FB config match\n");
	return -1;
     }

   egl.vi = glXGetVisualFromFBConfig(disp, egl.fbc);

   egl.ctx = glXCreateNewContext(disp, egl.fbc, GLX_RGBA_TYPE, NULL, True);

   Dprintf("Direct Rendering %s\n",
	   glXIsDirect(disp, egl.ctx) ? "enabled" : "not available");
   Dprintf("Visual ID=%#lx  depth %d\n", egl.vi->visualid, egl.vi->depth);

   return 0;
}

void
EGlExit(void)
{
   EobjTexturesFree();

   Dprintf("EGlExit\n");

   if (egl.vi)
     {
	XFree(egl.vi);
	egl.vi = NULL;
     }

   if (egl.ctx)
     {
	EGlWindowDisconnect();
	glXDestroyContext(disp, egl.ctx);
	egl.ctx = NULL;
     }
}

XVisualInfo        *
EGlGetVI(void)
{
   return egl.vi;
}

GLXContext
EGlGetContext(void)
{
   return egl.ctx;
}

Win
EGlWindowCreate(Win parent, int x, int y, unsigned int width,
		unsigned int height)
{
   return ECreateWindowVD(parent, x, y, width, height, egl.vi->visual,
			  egl.vi->depth);
}

void
EGlWindowConnect(Window xwin)
{
   glXMakeContextCurrent(disp, xwin, xwin, egl.ctx);

   if (egl.ctx_initialised)
      return;

   /* First time */
   glEnable(GL_TEXTURE_2D);	/* ??? */
   glShadeModel(GL_SMOOTH);
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glClearDepth(1.0f);
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

   egl.ctx_initialised = 1;
}

void
EGlWindowDisconnect(void)
{
   if (!glXMakeContextCurrent(disp, None, None, NULL))
     {
	Eprintf("Failed to release GL context.\n");
     }
}

ETexture           *
EGlTextureFromImage(EImage * im, int mode)
{
   ETexture           *et;
   int                 w, h;
   unsigned char      *data;

   if (!im)
      return NULL;

   et = Ecalloc(1, sizeof(ETexture));
   if (!et)
      return NULL;

   et->target = TEXTURE_TARGET;
   glGenTextures(1, &et->texture);
   glBindTexture(et->target, et->texture);
   et->type = ETEX_TYPE_IMAGE;

   EImageGetSize(im, &w, &h);
   data = EImageGetData(im);

   switch (mode)
     {
     case 0:			/* No filtering */
	glTexImage2D(et->target, 0, GL_RGB8, w, h, 0, GL_BGRA,
		     GL_UNSIGNED_BYTE, data);
	glTexParameteri(et->target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(et->target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	break;
     case 1:			/* Linear filtering */
	glTexImage2D(et->target, 0, GL_RGB8, w, h, 0, GL_BGRA,
		     GL_UNSIGNED_BYTE, data);
	glTexParameteri(et->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(et->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	break;
     case 2:			/* Mipmapping */
	gluBuild2DMipmaps(et->target, GL_RGB8, w, h, GL_BGRA,
			  GL_UNSIGNED_BYTE, data);
	glTexParameteri(et->target, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(et->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	break;
     }

   return et;
}

static              GLXPixmap
GetGlPixmap(Window xwin, Drawable draw)
{
   static const int    attrs[] = {
      GLX_TEXTURE_FORMAT_EXT, GLX_TEXTURE_FORMAT_RGB_EXT,
      0
   };
   Pixmap              pixmap;
   GLXPixmap           glxpixmap;

   if (xwin == None && draw == None)
      return 0;

   pixmap = (draw) ? draw : XCompositeNameWindowPixmap(disp, xwin);
   glxpixmap = glXCreatePixmap(disp, egl.fbc, pixmap, attrs);
   Dprintf("GetGlPixmap: Window=%#lx Drawable=%#lx glxpixmap=%#lx\n",
	   xwin, draw, glxpixmap);

   return glxpixmap;
}

ETexture           *
EGlTextureFromDrawable(Drawable draw, int mode)
{
   ETexture           *et;
   GLXPixmap           glxpixmap;

   if (draw == None)
      return NULL;

   et = Ecalloc(1, sizeof(ETexture));
   if (!et)
      return NULL;

   et->target = TEXTURE_TARGET;
   glGenTextures(1, &et->texture);
   glEnable(et->target);	/* Why ??? */
   glBindTexture(et->target, et->texture);
   et->type = ETEX_TYPE_PIXMAP;

   glxpixmap = GetGlPixmap(draw, (mode & 0x100) ? None : draw);
   if (glxpixmap == None)
      return NULL;
   et->glxpmap = glxpixmap;

#if 0
   unsigned int        trg;

   glXQueryDrawable(disp, et->glxpmap, GLX_TEXTURE_TARGET_EXT, &trg);
   Dprintf("TARGET: %#x\n", trg);
#endif

   switch (mode & 0xff)
     {
     case 0:
	glXBindTexImageEXT(disp, glxpixmap, GLX_FRONT_LEFT_EXT, NULL);
	glTexParameteri(et->target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(et->target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	break;
     case 1:
	glXBindTexImageEXT(disp, glxpixmap, GLX_FRONT_LEFT_EXT, NULL);
	glTexParameteri(et->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(et->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	break;
     }
#if 0				/* No! */
   glXDestroyPixmap(disp, glxpixmap);
#endif
   glBindTexture(et->target, 0);

   return et;
}

void
EGlTextureDestroy(ETexture * et)
{
   if (!et)
      return;

   Dprintf("EGlTextureDestroy %d type=%u pmap=%#x\n", et->texture, et->type,
	   et->glxpmap);

   glEnable(et->target);	/* Why ??? */
   glBindTexture(et->target, et->texture);

   switch (et->type)
     {
     case ETEX_TYPE_IMAGE:
	break;
     case ETEX_TYPE_PIXMAP:
	if (!et->glxpmap)
	   break;
	glXReleaseTexImageEXT(disp, et->glxpmap, GLX_FRONT_LEFT_EXT);
	glBindTexture(et->target, 0);
	glDisable(et->target);
	glXDestroyPixmap(disp, et->glxpmap);
	break;
     }

   glBindTexture(et->target, 0);
   glDeleteTextures(1, &et->texture);

   Efree(et);
}

#include "eobj.h"

static void
EobjTexturesFree(void)
{
   int                 i, num;
   EObj               *const *eol, *eo;

   eol = EobjListStackGet(&num);
   for (i = 0; i < num; i++)
     {
	eo = eol[i];
	EGlTextureDestroy(eo->glhook);
	eo->glhook = NULL;
     }
}

ETexture           *
EobjGetTexture(EObj * eo)
{
   Pixmap              pmap;

   if (eo->glhook)
      return eo->glhook;

   pmap = EobjGetPixmap(eo);
   if (pmap == None)
      return NULL;

   eo->glhook = EGlTextureFromDrawable(pmap, 0);

   return eo->glhook;
}
