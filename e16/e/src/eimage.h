/*
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
#ifndef _EIMAGE_H_
#define _EIMAGE_H_

typedef void        EImage;
typedef void        EImageColorModifier;

typedef struct
{
   int                 left, right, top, bottom;
} EImageBorder;

void                EImageInit(Display * dpy);

EImage             *EImageCreate(int w, int h);
EImage             *EImageCreateFromData(int w, int h, unsigned int *data);
EImage             *EImageCreateScaled(EImage * im, int sx, int sy,
				       int sw, int sh, int dw, int dh);

void                EImageFree(EImage * im);
void                EImageDecache(EImage * im);

void                EImageSave(EImage * im, const char *file);
EImage             *EImageLoad(const char *file);

void                EImageCheckAlpha(EImage * im);

void                EImageSetHasAlpha(EImage * im, int has_alpha);
void                EImageSetBorder(EImage * im, EImageBorder * border);

int                 EImageHasAlpha(EImage * im);
void                EImageGetSize(EImage * im, int *pw, int *ph);

void                EImageFill(EImage * im, int x, int y, int w, int h, int r,
			       int g, int b, int a);
void                EImageOrientate(EImage * im, int orientation);

void                EImageBlend(EImage * im, EImage * src, int blend,
				int sx, int sy, int sw, int sh,
				int dx, int dy, int dw, int dh,
				int merge_alpha, int anti_alias);
void                EImageBlendCM(EImage * im, EImage * src,
				  EImageColorModifier * icm);
void                EImageTile(EImage * im, EImage * tile, int blend, int tw,
			       int th, int dx, int dy, int dw, int dh, int ox,
			       int oy);

EImage             *EImageGrabDrawable(Drawable draw, Pixmap mask,
				       int x, int y, int w, int h, int grab);
EImage             *EImageGrabDrawableScaled(Drawable draw, Pixmap mask,
					     int x, int y, int w, int h,
					     int iw, int ih, int grab,
					     int get_mask_from_shape);

void                EImageRenderOnDrawable(EImage * im, Drawable draw,
					   int x, int y, int w, int h,
					   int blend);

void                EImageRenderPixmaps(EImage * im, Drawable draw,
					Pixmap * pmap, Pixmap * mask,
					int w, int h);
void                EImagePixmapFree(Pixmap pmap);

EImageColorModifier *EImageColorModifierCreate(unsigned char *r,
					       unsigned char *g,
					       unsigned char *b,
					       unsigned char *a);

void                ScaleRect(Window src, Pixmap dst, Pixmap * pdst,
			      int sx, int sy, int sw, int sh,
			      int dx, int dy, int dw, int dh, int scale);

#endif /* _EIMAGE_H_ */
