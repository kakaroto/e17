#ifndef __RGBADRAW
#define __RGBADRAW 1
typedef struct _imlib_filter ImlibFilter;

struct _imlib_filter
{
   short *r, *g, *b, *a;
   int w, h, x, y;
};
ImlibFilter *__imlib_CreateFilter(int w, int h, int x, int y);
void __imlib_FreeFilter(ImlibFilter *fil);
void __imlib_FilterImage(ImlibImage *im, ImlibFilter *fil);

void __imlib_FlipImageHoriz(ImlibImage *im);
void __imlib_FlipImageVert(ImlibImage *im);
void __imlib_FlipImageBoth(ImlibImage *im);
void __imlib_FlipImageDiagonal(ImlibImage *im, int direction);
void __imlib_BlurImage(ImlibImage *im, int rad);
void __imlib_SharpenImage(ImlibImage *im, int rad);
void __imlib_TileImageHoriz(ImlibImage *im);
void __imlib_TileImageVert(ImlibImage *im);
ImlibUpdate * __imlib_draw_line(ImlibImage *im, int x1, int y1, int x2, int y2, DATA8 r, DATA8 g, DATA8 b, DATA8 a, ImlibOp op, char make_updates);
void __imlib_draw_box(ImlibImage *im, int x, int y, int w, int h, DATA8 r, DATA8 g, DATA8 b, DATA8 a, ImlibOp op);
void __imlib_draw_filled_box(ImlibImage *im, int x, int y, int w, int h, DATA8 r, DATA8 g, DATA8 b, DATA8 a, ImlibOp op);
void __imlib_draw_filled_box(ImlibImage *im, int x, int y, int w, int h, DATA8 r, DATA8 g, DATA8 b, DATA8 a, ImlibOp op);
void __imlib_copy_image_data(ImlibImage *im, int x, int y, int w, int h, int nx, int ny);
void __imlib_copy_alpha_data(ImlibImage *src, ImlibImage *dst, int x, int y, int w, int h, int nx, int ny);
#endif
