#include "elicit.h"
#include "math.h"

/* functions */
int setup(int argc, char **argv);
static int cb_exit(Ecore_Evas *ee);
void cb_resize(Ecore_Evas *ee);
static void cb_edje(void *data, Evas_Object *o, const char *sig, const char *src);
static void cb_colors(void *data, Evas_Object *o, const char *sig, const char *src);
void pick_color();
void shoot();
void update_text();
int timer_color(void *data);

void _rgb_to_hex();
void _rgb_to_hsv();
void _hsv_to_rgb();

/* variables */
Ecore_Evas *ee;
Evas *evas;

int win_w = 100;
int win_h = 100;

Evas_Object *gui;

struct {
  Evas_Object *obj;
  double x, y;
  double w, h;
} shot, swatch;

int red, green, blue;
double hue, saturation, value;
char *hex;

/* flags */
int picking;
int shooting;
int changed;
double zoom;
int changing;
Ecore_Timer *change_timer;

int
main (int argc, char **argv)
{
  /* initialize the libraries */
  if (!ecore_init())
  {
    printf("ERROR: can't initialize Ecore.\n");
    return 1;
  }
  
  if (!ecore_evas_init())
  {
    printf("ERROR: can't initialize Ecore_Evas.\n");
    return 1;
  }

  /* create an Ecore_Evas (x11 window with an evas) */
  ee = ecore_evas_software_x11_new(0, 0, 0, 0, win_w, win_h);
  if (!ee)
  {
    printf("ERROR: can't create a software X11 Ecore_Evas.\n");
    return 1;
  }

  /* get the evas from the Ecore_Evas) */
  evas = ecore_evas_get(ee);

  evas_font_path_append(evas, DATADIR"/font");

  /* callbacks to exit on window close, destroy or kill requests */
  ecore_evas_callback_delete_request_set(ee, cb_exit);
  ecore_evas_callback_destroy_set(ee, cb_exit);
  ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, cb_exit, ee);

  ecore_evas_callback_resize_set(ee, cb_resize);
       
  /* do everything app specific in here */
  if (setup(argc, argv))
  {
    printf("ERROR: setting up application failed.\n");
    return 1;
  }
  
  ecore_evas_show(ee);

  /* start the event loop */
  ecore_main_loop_begin();

  /* shutdown the subsystems (when event loop exits, app is done) */
  ecore_evas_shutdown();
  ecore_shutdown();

  return 0;
}

static int
cb_exit(Ecore_Evas *ee)
{
  ecore_main_loop_quit();
  return 0;
}    

/* all app specific setup goes here  *
 * return 0 if everything goes ok.   *
 * return 1 if something goes wrong. */
int
setup(int argc, char **argv)
{
  Evas_Object *o;
  double mw, mh;

  ecore_evas_borderless_set(ee, 1);

  edje_init();
  edje_frametime_set(1.0 / 60.0);

  ecore_evas_shaped_set(ee, 1);
  ecore_evas_resize(ee, 164, 113);

  gui = edje_object_add(evas);
  edje_object_file_set(gui, DATADIR"/themes/elicit.eet", "elicit");
  printf(DATADIR"/themes/elicit.eet");
  evas_object_move(gui, 0, 0);
  evas_object_resize(gui, 163, 113);
  evas_object_show(gui);


  edje_object_size_min_get(gui, &mw, &mh);
  //printf("min size: %f x %f\n", mw, mh);
  ecore_evas_size_min_set(ee, mw, mh);


  /* create the swatch and shot objects */
  o = evas_object_image_add(evas);
  evas_object_show(o);
  shot.obj = o;
  
  o = evas_object_rectangle_add(evas);
  evas_object_color_set(o, 0, 0, 0, 255);
  evas_object_show(o);
  swatch.obj = o;

  /* swallow them */
  edje_object_part_swallow(gui, "shot", shot.obj);
  edje_object_part_swallow(gui, "swatch", swatch.obj);
 
  /* set up edje callbacks */
  edje_object_signal_callback_add(gui, "mouse,*", "pick", cb_edje, NULL);
  edje_object_signal_callback_add(gui, "mouse,*", "shoot", cb_edje, NULL);
  edje_object_signal_callback_add(gui, "mouse,*", "close", cb_edje, NULL);
  
  edje_object_signal_callback_add(gui, "mouse,*", "red-up", cb_colors, NULL);
  edje_object_signal_callback_add(gui, "mouse,*", "red-dn", cb_colors, NULL);
  edje_object_signal_callback_add(gui, "mouse,*", "green-up", cb_colors, NULL);
  edje_object_signal_callback_add(gui, "mouse,*", "green-dn", cb_colors, NULL);
  edje_object_signal_callback_add(gui, "mouse,*", "blue-up", cb_colors, NULL);
  edje_object_signal_callback_add(gui, "mouse,*", "blue-dn", cb_colors, NULL);
  edje_object_signal_callback_add(gui, "mouse,*", "hue-up", cb_colors, NULL);
  edje_object_signal_callback_add(gui, "mouse,*", "hue-dn", cb_colors, NULL);
  edje_object_signal_callback_add(gui, "mouse,*", "sat-up", cb_colors, NULL);
  edje_object_signal_callback_add(gui, "mouse,*", "sat-dn", cb_colors, NULL);
  edje_object_signal_callback_add(gui, "mouse,*", "val-up", cb_colors, NULL);
  edje_object_signal_callback_add(gui, "mouse,*", "val-dn", cb_colors, NULL);
  edje_object_signal_callback_add(gui, "mouse,*", "zoom-up", cb_colors, NULL);
  edje_object_signal_callback_add(gui, "mouse,*", "zoom-dn", cb_colors, NULL);

  zoom = 4;
  red = 0; green = 0; blue = 0;
  hue = 0; saturation = 0; value = 0;
  hex = "#000000";
  update_text();
  return 0;
}

void 
cb_resize(Ecore_Evas *ee)
{
  int x,y,w, h;

  /* keep the gui the same size as the window */
  ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
  evas_object_resize(gui, w, h);

  /* keep the shot filling its bounds */
  evas_object_geometry_get(shot.obj, &(shot.x), &(shot.y), &(shot.w), &(shot.h));
  evas_object_image_fill_set(shot.obj, 0, 0, shot.w, shot.h);
}

static void cb_edje(void *data, Evas_Object *o, const char *sig, const char *src)
{
  if (!strcmp(src, "pick"))
  {
    if (!strcmp(sig, "mouse,down,1"))
    {
      picking = 1;
    }
    else if (!strcmp(sig, "mouse,up,1"))
    {
      picking = 0;
    }
    else if (!strcmp(sig, "mouse,move"))
    {
      if (picking == 1)
      {
        pick_color();
      }
    }
  }

  else if (!strcmp(src, "shoot"))
  {
    if (!strcmp(sig, "mouse,down,1"))
    {
      shooting = 1;
    }
    else if (!strcmp(sig, "mouse,up,1"))
    {
      shooting = 0;
    }
    else if (!strcmp(sig, "mouse,move"))
    {
      if (shooting == 1)
      {
        shoot();
      }
    }
  }

  else if (!strcmp(src, "close"))
  {
    if (!strcmp(sig, "mouse,clicked,1"))
    {
      evas_object_del(gui);
      ecore_main_loop_quit();
    }
  }


}

static void
cb_colors(void *data, Evas_Object *o, const char *sig, const char *src)
{
  //printf("cb_colors!!!\n");
  if (!strcmp(sig, "mouse,down,1"))
  {
    //printf("--down\n");
    changing = 1;
    change_timer = ecore_timer_add(0.01, timer_color, strdup(src));
  }
  else if (!strcmp(sig, "mouse,up,1"))
  {
    //printf("--up\n");
    changing = 0;
    //ecore_timer_del(change_timer);
  }
}

void
pick_color()
{
  Imlib_Image *im;
  Imlib_Color col;
  int h, s, v;
  int x, y;
  int tr;

  /* where are we pointing? */
//  ecore_pointer_xy_get(&x, &y);
  XQueryPointer(ecore_x_display_get(), RootWindow(ecore_x_display_get(),0), &tr, &tr, &tr, &tr, &x, &y, &tr);

  /* setup the imlib context */
  imlib_context_set_display(ecore_x_display_get());
  imlib_context_set_drawable(RootWindow(ecore_x_display_get(),0));
  imlib_context_set_visual( DefaultVisual(ecore_x_display_get(), DefaultScreen(ecore_x_display_get() ) ));

  /* get the color of the current pixel */
  im = imlib_create_image_from_drawable(0, x-1, y-1, 1, 1, 0);
  imlib_context_set_image(im);
  imlib_image_query_pixel(0, 0, &col);

  //printf("(%d, %d, %d)\n", col.red, col.green, col.blue);
  evas_object_color_set(swatch.obj, col.red, col.green, col.blue, 255);
#if 1
  /* set the color values */
  red = col.red;
  green = col.green;
  blue = col.blue;

  /* update the other formats */
  _rgb_to_hsv();
  _rgb_to_hex();

  edje_object_part_text_set(gui, "red-val", "poop!");
  update_text();
#endif
  imlib_free_image(); 

  changed = 1;
}

void
shoot()
{
  Imlib_Image *im;
  int px, py;
  int x, y, w, h;
  int dw, dh;
  int tr;

  XQueryPointer(ecore_x_display_get(), RootWindow(ecore_x_display_get(),0), &tr, &tr, &tr, &tr, &px, &py, &tr);

  /* find out which section of the screen we need to copy */
  w = shot.w * (1 / zoom);
  h = shot.h * (1 / zoom);
  x = px - .5 * w;
  y = py - .5 * h;

  ecore_x_window_size_get(RootWindow(ecore_x_display_get(),0), &dw, &dh);
  if (x < 0) x = 0;
  if (y < 0) y = 0;
  if (x + w > dw) x = dw - w;
  if (y + h > dh) y = dh - h;

  /* setup the imlib context */
  imlib_context_set_display(ecore_x_display_get());
  imlib_context_set_drawable(RootWindow(ecore_x_display_get(),0));
  imlib_context_set_visual( DefaultVisual(ecore_x_display_get(), DefaultScreen(ecore_x_display_get() ) ));

  /* copy the correct part of the screen */
  im = imlib_create_image_from_drawable(0, x, y, w, h, 1);
  imlib_context_set_image(im);
  imlib_image_set_format("argb");

  /* get the object ready, copy the data in */
  evas_object_image_alpha_set(shot.obj, 0);
  evas_object_image_size_set(shot.obj, w, h);
  evas_object_image_smooth_scale_set(shot.obj, 0);

  evas_object_image_data_copy_set(shot.obj, imlib_image_get_data_for_reading_only());
  
  /* tell evas that we changed part of the image data */
  evas_object_image_data_update_add(shot.obj, 0, 0, w, h);

  /* set it to fill the whole object */
  evas_object_image_fill_set(shot.obj, 0, 0, shot.w, shot.h);

  imlib_free_image();

  /* make sure things get updated elsewhere */
  changed = 1;

}

void
update_text()
{
  char buf[100];
 
  snprintf(buf, sizeof(buf)-1, "%d", red);
  edje_object_part_text_set(gui, "red-val", buf); 
  
  snprintf(buf, sizeof(buf)-1, "%d", green);
  edje_object_part_text_set(gui, "green-val", buf); 
  
  snprintf(buf, sizeof(buf)-1, "%d", blue);
  edje_object_part_text_set(gui, "blue-val", buf); 

  snprintf(buf, sizeof(buf)-1, "%.0f", hue);
  edje_object_part_text_set(gui, "hue-val", buf); 

  snprintf(buf, sizeof(buf)-1, "%.2f", saturation);
  edje_object_part_text_set(gui, "sat-val", buf); 

  snprintf(buf, sizeof(buf)-1, "%.2f", value);
  edje_object_part_text_set(gui, "val-val", buf); 

  snprintf(buf, sizeof(buf)-1, "%s", hex);
  edje_object_part_text_set(gui, "hex-val", buf); 

  snprintf(buf, sizeof(buf)-1, "%.2f", zoom);
  edje_object_part_text_set(gui, "zoom-val", buf); 

  /* thaw here to force edje to recalc */
  edje_object_thaw(gui);
}


int
timer_color(void *data)
{
  char *c = data;
  static double start = 0.0;
  double duration = 2.0;
  double r, d, dir;
  int w;

  if (changing == 1)
  { 
    //printf("***get start time!\n");
    start = ecore_time_get();
    changing = 2;
  }

  d = (ecore_time_get() - start) / duration;
  if (d > 1) d = 1.0;
  r = sin(d * .5 * M_PI);

  if (c[strlen(c)-2] == 'u') dir = 1;
  else if (c[strlen(c)-2] == 'd') dir = -1;
  else dir = 0; /* somethigns wrong, this shouldn't happen */

  if (c[0] == 'r')
  {
    red += dir * 1;
    if (red > 255) red = 255;
    if (red < 0) red = 0;
    _rgb_to_hsv();
    _rgb_to_hex();
  }

  if (c[0] == 'g')
  {
    green += dir * 1;
    if (green > 255) green = 255;
    if (green < 0) green = 0;
    _rgb_to_hsv();
    _rgb_to_hex();
  }

  if (c[0] == 'b')
  {
    blue += dir * 1;
    if (blue > 255) blue = 255;
    if (blue < 0) blue = 0;
    _rgb_to_hsv();
    _rgb_to_hex();
  }

  if (c[0] == 'h')
  {
    hue += dir * 1;
    if (hue > 360) hue = 360;
    if (hue < 0) hue = 0;
    _hsv_to_rgb();
    _rgb_to_hex();
  }

  if (c[0] == 's')
  {
    saturation += dir * .01;
    if (saturation > 1) saturation = 1;
    if (saturation < 0) saturation = 0;
    _hsv_to_rgb();
    _rgb_to_hex();
  }

  if (c[0] == 'v')
  {
    value += dir * .01;
    if (value > 1) value = 1;
    if (value < 0) value = 0;
    _hsv_to_rgb();
    _rgb_to_hex();
  }

  if (c[0] == 'z')
  {
    zoom += dir * .1;
    if (zoom < 1) zoom = 1;
  }

  update_text();
  evas_object_color_set(swatch.obj, red, green ,blue, 255);
  changed = 1;

  
  /* if changing, keep on calling it */
  if (changing)
  {
   // return 1;
   /* we want the time to wait to depend on how far along it is... */
   change_timer = ecore_timer_add(.16 - .15*r, timer_color, c);
   return 0; 
  }
  else
  {
    if (change_timer) ecore_timer_del(change_timer);
    free(c);
    return 0;
  }

}

/********* some functions stolen from imlib2 and modified :) ***********/
void
_rgb_to_hsv()
{
   int r, g, b;
   int f;
   float i,j,k,max,min,d;
   float h, s, v;

   r = red;
   g = green;
   b = blue;

   i = ((float)r)/255.0;
   j = ((float)g)/255.0;
   k = ((float)b)/255.0;

   f = 0;
   max = min = i;
   if (j>max) { max = j; f = 1; } else min = j;
   if (k>max) { max = k; f = 2; } else if (k<min) min = k;
   d = max - min;

   v = max;
   if (max!=0) s = d/max; else s = 0;
   if (s==0)
      h = 0;
   else
   {
      switch (f)
      {
         case 0:
           h = (j - k)/d;
           break;
         case 1:
           h = 2 + (k - i)/d;
           break;
         case 2:
           h = 4 + (i - j)/d;
           break;
      }
      h *= 60.0;
      if (h<0) h += 360.0;
   }

   hue = h;
   saturation = s;
   value = v;

   //printf("%i %i %i %f %f %f\n", r, g, b, hue, saturation, value);
}

void
_hsv_to_rgb()
{
   int i,p,q,t;
   float vs,vsf;
   int r, g, b;
   float h, s, v;

   h = hue;
   s = saturation;
   v = value;

   i = (int)(v*255.0);
   if (s==0)
      r = g = b = i;
   else
   {
      if (h==360) h = 0;
      h = h/60.0;
      vs = v * s;
                vsf = vs * (h - (int)h);
      p = (int)(255.0 * (v - vs));
      q = (int)(255.0 * (v - vsf));
      t = (int)(255.0 * (v - vs + vsf));
      switch ((int)h)
      {
         case 0:
           r = i;
                          g = t;
                          b = p;
                          break;
         case 1:
           r = q;
                          g = i;
                          b = p;
           break;
         case 2:
           r = p;
                          g = i;
                          b = t;
           break;
         case 3:
           r = p;
                          g = q;
                          b = i;
           break;
         case 4:
           r = t;
                          g = p;
                          b = i;
           break;
         case 5:
           r = i;
                          g = p;
                          b = q;
           break;
      }
   }

   red = r;
   green = g;
   blue = b;
}

void 
_rgb_to_hex()
{
  char buf[8];

  sprintf(buf, "#%.2x%.2x%.2x\0", red, green, blue);
  hex = (char *)strdup(buf);
}
