#ifndef __RGBADRAW
#define __RGBADRAW 1


#define GROW_BOUNDS(px1, py1, px2, py2, x, y) { \
    if (x < px1)      \
        px1 = x;      \
                      \
    if (x > px2)      \
        px2 = x;      \
                      \
    if (y < py1)      \
        py1 = y;      \
                      \
    if (y > py2)      \
        py2 = y;      \
}

#define exchange(type, a, b)    \
{               \
    type _t_;   \
    _t_ = a;    \
    a = b;      \
    b = _t_;    \
}

/* Check if p lies on segment [ s1, s2 ] given that 
      it lies on the line defined by s1 and s2. */
#define __imlib_point_inside_segment(p_x, p_y, s1_x, s1_y, s2_x,s2_y) \
(s1_y != s2_y) ? (p_y <= MAX(s1_y, s2_y) && p_y >= MIN(s1_y, s2_y)) : (p_x <= MAX(s1_x, s2_x) && p_x >= MIN(s1_x, s2_x))

#define SWAP(a,b) {int _tmp_; _tmp_ = a; a = b; b = _tmp_;}
   
#define __imlib_point_on_segment(p_x, p_y, s1_x, s1_y, s2_x, s2_y) \
__imlib_segments_intersect(p_x, p_y, p_x, p_y, s1_x, s1_y, s2_x, s2_y)

typedef struct _edgerec
{
   int x;
}
edgeRec;

typedef struct _imlib_point ImlibPoint;

struct _imlib_point
{
   int x, y;
};

typedef struct _imlib_rectangle Imlib_Rectangle;

struct _imlib_rectangle
{
   int x, y, w, h;
};


typedef struct _imlib_polygon _ImlibPoly;
typedef _ImlibPoly *ImlibPoly;

struct _imlib_polygon
{
   ImlibPoint *points;
   int pointcount;
};

void __imlib_polygon_free(ImlibPoly poly);
void __imlib_polygon_add_point(ImlibPoly poly, int x, int y);
ImlibPoly __imlib_polygon_new(void);

typedef unsigned int ImlibOutCode;
enum
{ TOP = 0x1, BOTTOM = 0x2, RIGHT = 0x4, LEFT = 0x8 };

#define TRUE 1
#define FALSE 0

void __imlib_FlipImageHoriz(ImlibImage * im);
void __imlib_FlipImageVert(ImlibImage * im);
void __imlib_FlipImageBoth(ImlibImage * im);
void __imlib_FlipImageDiagonal(ImlibImage * im, int direction);
void __imlib_BlurImage(ImlibImage * im, int rad);
void __imlib_SharpenImage(ImlibImage * im, int rad);
void __imlib_TileImageHoriz(ImlibImage * im);
void __imlib_TileImageVert(ImlibImage * im);
ImlibUpdate *__imlib_draw_line(ImlibImage * im, int x1, int y1, int x2,
                               int y2, DATA8 r, DATA8 g, DATA8 b, DATA8 a,
                               ImlibOp op, char make_updates);
void __imlib_draw_box(ImlibImage * im, int x, int y, int w, int h, DATA8 r,
                      DATA8 g, DATA8 b, DATA8 a, ImlibOp op);
void __imlib_draw_filled_box(ImlibImage * im, int x, int y, int w, int h,
                             DATA8 r, DATA8 g, DATA8 b, DATA8 a, ImlibOp op);
void __imlib_draw_filled_box(ImlibImage * im, int x, int y, int w, int h,
                             DATA8 r, DATA8 g, DATA8 b, DATA8 a, ImlibOp op);
void __imlib_copy_image_data(ImlibImage * im, int x, int y, int w, int h,
                             int nx, int ny);
void __imlib_copy_alpha_data(ImlibImage * src, ImlibImage * dst, int x, int y,
                             int w, int h, int nx, int ny);
ImlibOutCode __imlib_comp_outcode(double x, double y, double xmin,
                                  double xmax, double ymin, double ymax);
void __imlib_draw_polygon(ImlibImage * im, ImlibPoly poly,
                          unsigned char closed, DATA8 r, DATA8 g, DATA8 b,
                          DATA8 a, ImlibOp op);
ImlibUpdate *__imlib_draw_line_clipped(ImlibImage * im, int x1, int y1,
                                       int x2, int y2, int clip_xmin,
                                       int clip_xmax, int clip_ymin,
                                       int clip_ymax, DATA8 r, DATA8 g,
                                       DATA8 b, DATA8 a, ImlibOp op,

                                       char make_updates);
void __imlib_draw_box_clipped(ImlibImage * im, int x, int y, int w, int h,
                              int clip_xmin, int clip_xmax, int clip_ymin,
                              int clip_ymax, DATA8 r, DATA8 g, DATA8 b,
                              DATA8 a, ImlibOp op);
void __imlib_draw_polygon_clipped(ImlibImage * im, ImlibPoly poly,
                                  unsigned char closed, int clip_xmin,
                                  int clip_xmax, int clip_ymin, int clip_ymax,
                                  DATA8 r, DATA8 g, DATA8 b, DATA8 a,

                                  ImlibOp op);
void __imlib_polygon_get_bounds(ImlibPoly poly, int *px1, int *py1, int *px2,

                                int *py2);
void __imlib_draw_set_point(ImlibImage * im, int x, int y, DATA8 r, DATA8 g,
                            DATA8 b, DATA8 a, ImlibOp op);
void __imlib_draw_set_point_clipped(ImlibImage * im, int x, int y,
                                    int clip_xmin, int clip_xmax,
                                    int clip_ymin, int clip_ymax, DATA8 r,
                                    DATA8 g, DATA8 b, DATA8 a, ImlibOp op);
void __imlib_draw_ellipse(ImlibImage * im, int xc, int yc, int aa, int bb,
                          DATA8 r, DATA8 g, DATA8 b, DATA8 a, ImlibOp op);
void __imlib_draw_ellipse_clipped(ImlibImage * im, int xc, int yc, int aa,
                                  int bb, int clip_xmin, int clip_xmax,
                                  int clip_ymin, int clip_ymax, DATA8 r,
                                  DATA8 g, DATA8 b, DATA8 a, ImlibOp op);
void __imlib_draw_polygon_filled(ImlibImage * im, ImlibPoly poly,
                                 int clip_xmin, int clip_xmax, int clip_ymin,
                                 int clip_ymax, DATA8 r, DATA8 g, DATA8 b,
                                 DATA8 a, ImlibOp op);
void __imlib_fill_ellipse(ImlibImage * im, int xc, int yc, int aa,
                                  int bb, int clip_xmin, int clip_xmax,
                                  int clip_ymin, int clip_ymax, DATA8 r,
                                  DATA8 g, DATA8 b, DATA8 a, ImlibOp op);
unsigned char __imlib_polygon_contains_point(ImlibPoly poly, int x, int y);
unsigned char __imlib_segments_intersect(int r1_x, int r1_y, int r2_x,
                                         int r2_y, int s1_x, int s1_y,
                                         int s2_x, int s2_y);
double __imlib_point_delta_from_line(int p_x, int p_y, int s1_x, int s1_y,

                                     int s2_x, int s2_y);
void __spanlist_clip(edgeRec * table1, edgeRec * table2, int *sy, int *ey,
                     int xmin, int xmax, int ymin, int ymax);
#endif
