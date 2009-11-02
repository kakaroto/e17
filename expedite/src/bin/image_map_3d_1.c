#undef FNAME
#undef NAME
#undef ICON

/* metadata */
#define FNAME image_map_3d_1_start
#define NAME "Image Map 3D 1"
#define ICON "bug.png"

#ifndef PROTO
# ifndef UI
#  include "main.h"

typedef struct _Point
{
   Evas_Coord x, y, z, u, v;
} Point;

typedef struct _Side
{
   Evas_Object *o;
   Point pt[4];
} Side;

typedef struct _Cube
{
   Side side[6];
} Cube;

/* standard var */
static int done = 0;

/* private data */
static Cube *cubes[5];

#define POINT(n, p, xx, yy, zz, uu, vv) \
   c->side[n].pt[p].x = xx; \
   c->side[n].pt[p].y = yy; \
   c->side[n].pt[p].z = zz; \
   c->side[n].pt[p].u = uu; \
   c->side[n].pt[p].v = vv

static Cube *
_cube_new(Evas_Coord w, Evas_Coord h, Evas_Coord d)
{
   Cube *c;
   int i;

   w -= (w / 2);
   h -= (h / 2);
   d -= (d / 2);
   c = calloc(1, sizeof(Cube));
   for (i = 0; i < 6; i++)
     {
        Evas_Object *o;
        char buf[256];
        o = evas_object_image_add(evas);
        c->side[i].o = o;
        snprintf(buf, sizeof(buf), "cube%i.png", i + 1);
        evas_object_image_file_set(o, build_path(buf), NULL);
        evas_object_image_fill_set(o, 0, 0, 256, 256);
        evas_object_resize(o, 256, 256);
//        evas_object_image_smooth_scale_set(o, 0);
        evas_object_show(o);
     }
   POINT(0, 0, -w, -h, -d,   0,   0);
   POINT(0, 1,  w, -h, -d, 256,   0);
   POINT(0, 2,  w,  h, -d, 256, 256);
   POINT(0, 3, -w,  h, -d,   0, 256);

   POINT(1, 0,  w, -h, -d,   0,   0);
   POINT(1, 1,  w, -h,  d, 256,   0);
   POINT(1, 2,  w,  h,  d, 256, 256);
   POINT(1, 3,  w,  h, -d,   0, 256);
   
   POINT(2, 0,  w, -h,  d,   0,   0);
   POINT(2, 1, -w, -h,  d, 256,   0);
   POINT(2, 2, -w,  h,  d, 256, 256);
   POINT(2, 3,  w,  h,  d,   0, 256);

   POINT(3, 0, -w, -h,  d,   0,   0);
   POINT(3, 1, -w, -h, -d, 256,   0);
   POINT(3, 2, -w,  h, -d, 256, 256);
   POINT(3, 3, -w,  h,  d,   0, 256);
   
   POINT(4, 0, -w, -h,  d,   0,   0);
   POINT(4, 1,  w, -h,  d, 256,   0);
   POINT(4, 2,  w, -h, -d, 256, 256);
   POINT(4, 3, -w, -h, -d,   0, 256);
   
   POINT(5, 0, -w,  h, -d,   0,   0);
   POINT(5, 1,  w,  h, -d, 256,   0);
   POINT(5, 2,  w,  h,  d, 256, 256);
   POINT(5, 3, -w,  h,  d,   0, 256);
   
   return c;
}

static void
_cube_pos(Cube *c,
          Evas_Coord x, Evas_Coord y, Evas_Coord z,
          double dx, double dy, double dz)
{
   Evas_Map *m;
   int i, j, order[6], sorted;
   Evas_Coord mz[6];
   
   m = evas_map_new(4);
//   evas_map_smooth_set(m, 0);

   for (i = 0; i < 6; i++)
     {
        Evas_Coord tz[4];
        
        for (j = 0; j < 4; j++)
          {
             evas_map_point_coord_set(m, j,
                                      c->side[i].pt[j].x + x, 
                                      c->side[i].pt[j].y + y, 
                                      c->side[i].pt[j].z + z);
             evas_map_point_image_uv_set(m, j,
                                      c->side[i].pt[j].u,
                                      c->side[i].pt[j].v);
             evas_map_point_color_set(m, j, 255, 255, 255, 255);
          }
        evas_map_util_rotate_3d(m, dx, dy, dz, x, y, z,
                                (win_w / 2), (win_h / 2), 0, 512);
        if (evas_map_util_clockwise_get(m))
          {
             evas_object_map_enable_set(c->side[i].o, 1);
             evas_object_map_set(c->side[i].o, m);
             evas_object_show(c->side[i].o);
          }
        else
          evas_object_hide(c->side[i].o);
        
        order[i] = i;
        for (j = 0; j < 4; j++)
          evas_map_point_coord_get(m, j, NULL, NULL, &(tz[j]));
        mz[i] = (tz[0] + tz[1] + tz[2] + tz[3]) / 4;
     }
   sorted = 0;
   do
     {
        sorted = 1;
        for (i = 0; i < 5; i++)
          {
             if (mz[order[i]] > mz[order[i + 1]])
               {
                  j = order[i];
                  order[i] = order[i + 1];
                  order[i + 1] = j;
                  sorted = 0;
               }
          }
     }
   while (!sorted);

   evas_object_raise(c->side[order[0]].o);
   for (i = 1; i < 6; i++)
     evas_object_stack_below(c->side[order[i]].o, c->side[order[i - 1]].o);
   evas_map_free(m);
}

static void
_cube_free(Cube *c)
{
   int i;
   
   for (i = 0; i < 6; i++) evas_object_del(c->side[i].o);
   free(c);
}

/* setup */
static void _setup(void)
{
   cubes[0] = _cube_new(128, 128, 256);
   cubes[1] = _cube_new(256, 128, 128);
   cubes[2] = _cube_new(256, 256, 128);
   cubes[3] = _cube_new(128, 256, 128);
   cubes[4] = _cube_new(256, 256, 256);
   
   done = 0;
}

/* cleanup */
static void _cleanup(void)
{
   _cube_free(cubes[0]);
   _cube_free(cubes[1]);
   _cube_free(cubes[2]);
   _cube_free(cubes[3]);
   _cube_free(cubes[4]);
}

/* loop - do things */
static void _loop(double t, int f)
{
   _cube_pos(cubes[0], 
             (win_w / 2) - 640, (win_h / 2) - 256, 512,
             f / 2.0, f, f / 3.0);
   _cube_pos(cubes[1], 
             (win_w / 2) + 512, (win_h / 2) - 128, 384,
             f / 3.0, f / 2.0, f / 4.0);
   _cube_pos(cubes[2], 
             (win_w / 2) - 384, (win_h / 2) + 128, 256, 
             f / 2.0, f / 3.0, f);
   _cube_pos(cubes[3], 
             (win_w / 2) + 256, (win_h / 2) + 64, 128, 
             f, f / 5.0, f / 2.0);
   _cube_pos(cubes[4], 
             (win_w / 2), (win_h / 2), 0, 
             f / 4.0, f / 3.0, f / 5.0);
   FPS_STD(NAME);
}

/* prepend special key handlers if interactive (before STD) */
static void _key(char *key)
{
   KEY_STD;
}












/* template stuff - ignore */
# endif
#endif

#ifdef UI
_ui_menu_item_add(ICON, NAME, FNAME);
#endif

#ifdef PROTO
void FNAME(void);
#endif

#ifndef PROTO
# ifndef UI
void FNAME(void)
{
   ui_func_set(_key, _loop);
   _setup();
}
# endif
#endif
#undef FNAME
#undef NAME
#undef ICON
