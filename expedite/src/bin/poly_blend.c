#undef FNAME
#undef NAME
#undef ICON

/* metadata */
#define FNAME poly_blend_start
#define NAME "Polygon Blend"
#define ICON "flower.png"

#ifndef PROTO
# ifndef UI
#  include "main.h"

/* standard var */
static int done = 0;
/* private data */
static Evas_Object *o_images[OBNUM];

static void
poly(Evas_Object *o, int type, Evas_Coord x, Evas_Coord y)
{
   evas_object_polygon_points_clear(o);
   switch (type % 4)
     {
      case 0: /* triangle */
	evas_object_polygon_point_add(o, x + 50 , y + 0);
	evas_object_polygon_point_add(o, x + 100, y + 100);
	evas_object_polygon_point_add(o, x + 0  , y + 100);
	break;
      case 1: /* square */
	evas_object_polygon_point_add(o, x + 0  , y + 0);
	evas_object_polygon_point_add(o, x + 100, y + 0);
	evas_object_polygon_point_add(o, x + 100, y + 100);
	evas_object_polygon_point_add(o, x + 0  , y + 100);
	break;
      case 2: /* hex */
	evas_object_polygon_point_add(o, x + 50 , y + 0);
	evas_object_polygon_point_add(o, x + 100, y + 30);
	evas_object_polygon_point_add(o, x + 100, y + 70);
	evas_object_polygon_point_add(o, x + 50 , y + 100);
	evas_object_polygon_point_add(o, x + 0  , y + 70);
	evas_object_polygon_point_add(o, x + 0  , y + 30);
	break;
      case 3: /* star */
	evas_object_polygon_point_add(o, x + 50 , y + 0);
	evas_object_polygon_point_add(o, x + 60 , y + 40);
	evas_object_polygon_point_add(o, x + 90 , y + 30);
	evas_object_polygon_point_add(o, x + 70 , y + 60);
	evas_object_polygon_point_add(o, x + 90 , y + 100);
	evas_object_polygon_point_add(o, x + 50 , y + 70);
	evas_object_polygon_point_add(o, x + 10 , y + 100);
	evas_object_polygon_point_add(o, x + 30 , y + 60);
	evas_object_polygon_point_add(o, x + 10 , y + 30);
	evas_object_polygon_point_add(o, x + 40 , y + 40);
	break;
      default:
	break;
     }
}

/* setup */
static void _setup(void)
{
   int i;
   Evas_Object *o;
   srnd();
   for (i = 0; i < OBNUM; i++)
     {
	int r, g, b, a;

	o = evas_object_polygon_add(evas);
	o_images[i] = o;
	a = (rnd()&0xff) / 2;
	r = ((rnd()&0xff) * a) / 255;
	g = ((rnd()&0xff) * a) / 255;
	b = ((rnd()&0xff) * a) / 255;
	evas_object_color_set(o, r, g, b, a);
	poly(o, i, 0, 0);
	evas_object_show(o);
     }
   done = 0;
}

/* cleanup */
static void _cleanup(void)
{
   int i;
   for (i = 0; i < OBNUM; i++) evas_object_del(o_images[i]);
}

/* loop - do things */
static void _loop(double t, int f)
{
   int i;
   Evas_Coord x, y, w, h;
   Evas_Object *o;

   for (i = 0; i < OBNUM; i++)
     {
	o = o_images[i];
	evas_object_geometry_get(o, NULL, NULL, &w, &h);
	x = (win_w / 2) - (w / 2);
	x += sin((double)(f + (i * 13)) / (36.7 * SLOW)) * (win_w / 4);
	y = (win_h / 2) - (h / 2);
	y += cos((double)(f + (i * 28)) / (43.8 * SLOW)) * (win_h / 4);
	poly(o, i, x, y);
     }
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
