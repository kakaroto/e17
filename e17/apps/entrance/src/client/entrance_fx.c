#include "entrance_fx.h"
#define FADE_STEP 0.1           /* This should probably be a config option */

static void
_fx_fade_cb(int val, void *data)
{
   int r, g, b, alpha;
   int diff;
   char *name, buf[PATH_MAX];
   Entrance_FX_Fade_Data *d = (Entrance_FX_Fade_Data *) data;

   evas_object_color_get(d->obj, &r, &g, &b, &alpha);
   name = strdup(evas_object_name_get(d->obj));

   /* Object must have a name for us to handle it */
   if (!name || !d)
   {
      printf("abort\n");
      if (d)
         free(d);
      return;
   }

   /* Unique timer for each object */
   snprintf(buf, PATH_MAX, "%s_fade_timer", name);
   free(name);

   diff = (int) (((double) (d->end_a - d->start_a)) * d->step);

   /* Avoid a potential infinite loop */
   if (!diff)
   {
      printf("abort\n");
      free(d);
      return;
   }

   /* printf("alpha: %d, start: %d, end: %d, diff: %d, in: %f\n", alpha,
      d->start_a, d->end_a, diff, d->in); */

   /* Safety check */
   if ((alpha <= d->end_a && diff < 0) || (alpha >= d->end_a && diff > 0))
   {
      evas_object_color_set(d->obj, r, g, b, d->end_a);
      /* Assume hide if end_a is zero */
      if (!d->end_a)
      {
         evas_object_hide(d->obj);
         /* Restore original color of object */
         evas_object_color_set(d->obj, d->orig_color->r, d->orig_color->g,
                               d->orig_color->b, d->orig_color->a);
      }
      free(d->orig_color);
      free(d);
      return;
   }

   evas_object_color_set(d->obj, r, g, b, alpha + diff);
//    ecore_del_event_timer(buf);
   ecore_add_event_timer(buf, d->in, _fx_fade_cb, 0, d);
}

/**
 * fx_fade - Change the alpha transparency of an object over a time period
 * @o        The object to be faded
 * @t        Total fade time (in seconds)
 * @start_a  Initial color alpha value
 * @end_a    Final color alpha value
 *
 * The object must have a name for the fade function to work.
 */
void
fx_fade(Evas_Object * o, double t, int start_a, int end_a)
{
   Entrance_FX_Fade_Data *d = NULL;
   char *name, buf[PATH_MAX];
   double diff;
   E_Color *obj_color = NULL;
   int r, g, b, a;

   if (!o)
      return;

   /* No fades for nameless objects */
   if (!(name = strdup(evas_object_name_get(o))))
      return;
   snprintf(buf, PATH_MAX, "%s_fade_timer", name);

   d = (Entrance_FX_Fade_Data *) malloc(sizeof(Entrance_FX_Fade_Data));
   obj_color = (E_Color *) malloc(sizeof(E_Color));

   if (!d || !obj_color)
   {
      fprintf(stderr, "WARNING: fx_fade data allocation failed.\n");
      return;
   }

   /* Set up fade data */
   d->obj = o;
   d->start_a = start_a;
   d->end_a = end_a;
   d->step = FADE_STEP;         /* FIXME */
   diff = d->step * ((double) end_a - (double) start_a);
   d->in = diff / ((double) end_a - (double) start_a) * t;

   /* this is a little dirty. Should pass the actual struct from the theme in 
      order to preserve the correct original color (especially if its in a
      transient highlighted state or something */
   evas_object_color_get(o, &r, &g, &b, &a);
   obj_color->r = r;
   obj_color->g = g;
   obj_color->b = b;
   obj_color->a = a;
   d->orig_color = obj_color;

   /* Go */
   ecore_add_event_timer(name, d->in, _fx_fade_cb, 0, d);
   free(name);
}

/* Fade convenience functions */
void
fx_fade_in(Evas_Object * o, double t)
{
   int r, g, b, end_a;

   evas_object_color_get(o, &r, &g, &b, &end_a);
   evas_object_color_set(o, r, g, b, 0);
   evas_object_show(o);
   fx_fade(o, t, 0, end_a);
}

void
fx_fade_out(Evas_Object * o, double t)
{
   int r, g, b, start_a;

   evas_object_color_get(o, &r, &g, &b, &start_a);
   fx_fade(o, t, start_a, 0);
}
