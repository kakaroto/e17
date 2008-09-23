#include <Esmart_Cairo.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

static const int W = 640, H = 480;
static const double OUT_SIZE = 2.0;
static const int OUT_RADIUS = 118;
static const int SECOND_RADIUS = 100;
static const int MINUTE_RADIUS = 75;
static const int HOUR_RADIUS = 50;

struct app
{
   Ecore_Evas *ee;
   Evas *e;
   Evas_Object *bg;
   Evas_Object *img_cairo;
   cairo_surface_t *surface;
   cairo_t *cairo;
   int fill_auto_set;
};

static void
on_resize(Ecore_Evas *ee)
{
   struct app *app = ecore_evas_data_get(ee, "app");
   int w, h;

   ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);

   evas_object_resize(app->bg, w, h);
   evas_object_resize(app->img_cairo, w, h);
}

static void
draw_hand(cairo_t *c, double value, int radius)
{
   int x, y;

   x = W / 4 + cos(value * 2 * M_PI - M_PI / 2) * radius;
   y = H / 4 + sin(value * 2 * M_PI - M_PI / 2) * radius;

   cairo_move_to(c, W/4, H/4);
   cairo_line_to(c, x, y);
}

static int
on_timeout(void *data)
{
   struct app *app = data;
   struct tm *tm;
   time_t t;


   t = time(NULL);
   tm = localtime(&t);

   cairo_set_source_rgba(app->cairo, 1.0, 1.0, 1.0, 0.0);
   cairo_set_operator(app->cairo, CAIRO_OPERATOR_SOURCE);
   cairo_paint(app->cairo);

   cairo_set_line_width(app->cairo, OUT_SIZE);
   cairo_set_source_rgba(app->cairo, 1, 0.2, 0.2, 0.6);
   cairo_arc(app->cairo, W/4, H/4, OUT_RADIUS, 0, 2 * M_PI);
   cairo_stroke(app->cairo);

   cairo_set_line_width(app->cairo, 2.0);
   cairo_set_operator(app->cairo, CAIRO_OPERATOR_OVER);
   cairo_set_source_rgba(app->cairo, 0.2, 0.2, 1.0, 0.6);
   draw_hand(app->cairo, tm->tm_sec / 60.0, SECOND_RADIUS);
   draw_hand(app->cairo, tm->tm_min / 60.0, MINUTE_RADIUS);
   draw_hand(app->cairo, tm->tm_hour / 12.0, HOUR_RADIUS);
   cairo_stroke(app->cairo);

   evas_object_image_pixels_dirty_set(app->img_cairo, 1);

   return 1;
}

static int
on_timeout_fill_change(void *data)
{
   struct app *app = data;

   app->fill_auto_set = !app->fill_auto_set;
   esmart_image_cairo_fill_auto_set(app->img_cairo, app->fill_auto_set);
   return 1;
}

int
main(void)
{
   struct app app;
   int radius;

   evas_init();
   ecore_init();
   ecore_evas_init();

   app.fill_auto_set = 0;

   app.ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, W, H);
   app.e = ecore_evas_get(app.ee);

   app.bg = evas_object_rectangle_add(app.e);
   evas_object_color_set(app.bg, 255, 255, 255, 255);
   evas_object_resize(app.bg, W, H);
   evas_object_show(app.bg);

   app.surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, W/2, H/2);
   app.cairo = cairo_create(app.surface);

   app.img_cairo = esmart_image_cairo_new_from_surface(app.e, app.surface);
   evas_object_resize(app.img_cairo, W, H);
   evas_object_show(app.img_cairo);

   ecore_evas_show(app.ee);
   ecore_evas_data_set(app.ee, "app", &app);
   ecore_evas_callback_resize_set(app.ee, on_resize);

   ecore_timer_add(1.0, on_timeout, &app);
   ecore_timer_add(5.0, on_timeout_fill_change, &app);
   on_timeout(&app);

   ecore_main_loop_begin();

   ecore_evas_shutdown();
   ecore_shutdown();
   evas_shutdown();

   return 0;
}
