#ifndef __RGBADRAW
#define __RGBADRAW 1

typedef struct _imlib_point    ImlibPoint;

struct _imlib_point
{  
   int x,y;
};

typedef struct _imlib_rectangle    Imlib_Rectangle;

struct _imlib_rectangle
{
   int x,y,w,h;
};


enum __imlib_polytype
{
   P_OPEN,
   P_CLOSED,
   P_FILLED
};

typedef struct _imlib_polygon _ImlibPoly;
typedef _ImlibPoly * ImlibPoly;

struct _imlib_polygon
{
   unsigned char closed, filled;
   ImlibPoint *points;
   int pointcount;
   DATA8 r,g,b,a;
};

void __imlib_polygon_free(ImlibPoly poly);
void __imlib_polygon_add_point(ImlibPoly poly, int x, int y);
ImlibPoly __imlib_polygon_new(int type);

typedef unsigned int ImlibOutCode;
enum
{ TOP = 0x1, BOTTOM = 0x2, RIGHT = 0x4, LEFT = 0x8 };
#define TRUE 1
#define FALSE 0

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
ImlibOutCode __imlib_comp_outcode(double x, double y, double xmin, double xmax,
                                  double ymin, double ymax);
ImlibUpdate        *
__imlib_draw_line_clipped(ImlibImage * im, int x1, int y1, int x2, int y2,
                          int clip_xmin, int clip_xmax, int clip_ymin,
                          int clip_ymax, DATA8 r, DATA8 g, DATA8 b, DATA8 a,
                          ImlibOp op, char make_updates);
void
__imlib_draw_box_clipped(ImlibImage * im, int x, int y, int w, int h,
                         int clip_xmin, int clip_xmax, int clip_ymin,
                         int clip_ymax, DATA8 r, DATA8 g, DATA8 b, DATA8 a,
                         ImlibOp op);
#endif
