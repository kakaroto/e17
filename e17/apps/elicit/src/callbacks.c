#include "Elicit.h"
#include "callbacks.h"

static int elicit_timer_color(void *data);

void
elicit_cb_exit(Ecore_Evas *ee)
{
  ecore_main_loop_quit();
}    

void 
elicit_cb_resize(Ecore_Evas *ee)
{
  int x,y,w, h;
  double sw, sh;
  Evas_Object *gui, *shot;

  /* keep the gui the same size as the window */
  ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);

  gui = evas_object_name_find(ecore_evas_get(ee), "gui");
  shot = evas_object_name_find(ecore_evas_get(ee), "shot");

  evas_object_resize(gui, w, h);

  /* keep the shot filling its bounds */
  evas_object_geometry_get(shot, NULL, NULL, &sw, &sh);
  evas_object_image_fill_set(shot, 0, 0, sw, sh);
}

void elicit_cb_pick(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Elicit *el = data;

  if (!strcmp(sig, "elicit,pick,start"))
  {
    printf("pick!\n");
    el->flags.picking = 1;
  }
  else if (!strcmp(sig, "elicit,pick,stop"))
  {
    el->flags.picking = 0;
  }
  else if (!strcmp(sig, "mouse,move"))
  {
    if (el->flags.picking == 1)
    {
      elicit_action_color_get(&(el->color.r), &(el->color.g), &(el->color.b));
      evas_object_color_set(el->swatch, el->color.r, el->color.g, el->color.b, 255);
      elicit_color_rgb_to_hsv(el->color.r, el->color.g, el->color.b,
                              &(el->color.h), &(el->color.s), &(el->color.v));
      if (el->color.hex) free (el->color.hex);
      el->color.hex = elicit_color_rgb_to_hex(el->color.r, el->color.g, el->color.b);
      elicit_ui_update_text(el);
    }
  }
}


void elicit_cb_shoot(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Elicit *el = data;

  if (!strcmp(sig, "elicit,shoot,start"))
  {
    printf("shoot!\n");
    el->flags.shooting = 1;
  }
  else if (!strcmp(sig, "elicit,shoot,stop"))
  {
    el->flags.shooting = 0;
  }
  else if (!strcmp(sig, "mouse,move"))
  {
    if (el->flags.shooting == 1)
    {
      double sw, sh;
      double x, y, w, h;

      evas_object_geometry_get(el->shot, NULL, NULL, &sw, &sh);
      printf("sw/h: %.2f, %.2f\n", sw, sh);
      w = sw * (1 / el->zoom);
      h = sh * (1 / el->zoom);
      printf("w/h: %.2f, %.2f\n", w, h);
      elicit_action_shoot(el->shot, (int)w, (int)h);
    }
  }
}

void
elicit_cb_colors(void *data, Evas_Object *o, const char *sig, const char *src)
{
  Elicit *el = data;
  if (elicit_glob_match(sig, "*,start"))
  {
    el->flags.changing = 1;
    el->change_sig = strdup(sig);
    el->change_timer = ecore_timer_add(0.01, elicit_timer_color, data);
  }
  else if (elicit_glob_match(sig, "*,stop"))
  {
    el->flags.changing = 0;
    if (el->change_sig) free(el->change_sig);
  }
}

static int
elicit_timer_color(void *data)
{
  Elicit *el = data;
  static double start = 0.0;
  double duration = 2.0;
  double r, d, dir;
  int w;

  /* we're done */
  if (!el->flags.changing)
  {
    if (0 && el->change_timer)
    {
      ecore_timer_del(el->change_timer);
      el->change_timer = NULL;
    }
    return 0;
  }
 
  /* this is the first time */
  if (el->flags.changing == 1)
  { 
    start = ecore_time_get();
    el->flags.changing = 2;
  }

  d = (ecore_time_get() - start) / duration;
  if (d > 1) d = 1.0;
  r = sin(d * .5 * M_PI);

  if (elicit_glob_match(el->change_sig, "*,up,*")) dir = 1;
  else if (elicit_glob_match(el->change_sig, "*,down,*")) dir = -1;
  else dir = 0; /* this shouldn't happen */

  if (elicit_glob_match(el->change_sig, "*,r,*"))
  {
    el->color.r += dir * 1;
    if (el->color.r > 255) el->color.r = 255;
    if (el->color.r < 0) el->color.r = 0;

    elicit_color_rgb_to_hsv(el->color.r, el->color.g, el->color.b,
                            &(el->color.h), &(el->color.s), &(el->color.v));
    if (el->color.hex) free (el->color.hex);
    el->color.hex = elicit_color_rgb_to_hex(el->color.r, el->color.g, el->color.b);
  }

  else if (elicit_glob_match(el->change_sig, "*,g,*"))
  {
    el->color.g += dir * 1;
    if (el->color.g > 255) el->color.g = 255;
    if (el->color.g < 0) el->color.g = 0;
    
    elicit_color_rgb_to_hsv(el->color.r, el->color.g, el->color.b,
                            &(el->color.h), &(el->color.s), &(el->color.v));
    if (el->color.hex) free (el->color.hex);
    el->color.hex = elicit_color_rgb_to_hex(el->color.r, el->color.g, el->color.b);
  }

  else if (elicit_glob_match(el->change_sig, "*,b,*"))
  {
    el->color.b += dir * 1;
    if (el->color.b > 255) el->color.b = 255;
    if (el->color.b < 0) el->color.b = 0;
    
    elicit_color_rgb_to_hsv(el->color.r, el->color.g, el->color.b,
                            &(el->color.h), &(el->color.s), &(el->color.v));
    if (el->color.hex) free (el->color.hex);
    el->color.hex = elicit_color_rgb_to_hex(el->color.r, el->color.g, el->color.b);
  }

  else if (elicit_glob_match(el->change_sig, "*,h,*"))
  {
    el->color.h += dir * 1;
    if (el->color.h > 360) el->color.h = 360;
    if (el->color.h < 0) el->color.h = 0;

    elicit_color_hsv_to_rgb(el->color.h, el->color.s, el->color.v,
                            &(el->color.r), &(el->color.g), &(el->color.b));
    if (el->color.hex) free (el->color.hex);
    el->color.hex = elicit_color_rgb_to_hex(el->color.r, el->color.g, el->color.b);
  }

  else if (elicit_glob_match(el->change_sig, "*,s,*"))
  {
    el->color.s += dir * .01;
    if (el->color.s > 1) el->color.s = 1;
    if (el->color.s < 0) el->color.s = 0;

    elicit_color_hsv_to_rgb(el->color.h, el->color.s, el->color.v,
                            &(el->color.r), &(el->color.g), &(el->color.b));
    if (el->color.hex) free (el->color.hex);
    el->color.hex = elicit_color_rgb_to_hex(el->color.r, el->color.g, el->color.b);
  }

  else if (elicit_glob_match(el->change_sig, "*,v,*"))
  {
    el->color.v += dir * .01;
    if (el->color.v > 1) el->color.v = 1;
    if (el->color.v < 0) el->color.v = 0;
    
    elicit_color_hsv_to_rgb(el->color.h, el->color.s, el->color.v,
                            &(el->color.r), &(el->color.g), &(el->color.b));
    if (el->color.hex) free (el->color.hex);
    el->color.hex = elicit_color_rgb_to_hex(el->color.r, el->color.g, el->color.b);
  }

  else if (elicit_glob_match(el->change_sig, "*,zoom,*"))
  {
    el->zoom += dir * .1;
    if (el->zoom < 1) el->zoom = 1;
  }

  elicit_ui_update_text(el);
  evas_object_color_set(el->swatch, el->color.r, el->color.g, el->color.b, 255);

  el->flags.changed = 1;
  
  /* if changing, keep on calling it */
  if (el->flags.changing)
  {
   /* we want the time to wait to depend on how far along it is... */
   el->change_timer = ecore_timer_add(.16 - .15*r, elicit_timer_color, el);
   return 0; 
  }

}

