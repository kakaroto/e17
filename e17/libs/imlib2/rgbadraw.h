#ifndef __RGBADRAW
#define __RGBADRAW 1
void __imlib_FlipImageHoriz(ImlibImage *im);
void __imlib_FlipImageVert(ImlibImage *im);
void __imlib_FlipImageDiagonal(ImlibImage *im);
void __imlib_BlurImage(ImlibImage *im, int rad);
void __imlib_SharpenImage(ImlibImage *im, int rad);
void __imlib_TileImageHoriz(ImlibImage *im);
void __imlib_TileImageVert(ImlibImage *im);
#endif
