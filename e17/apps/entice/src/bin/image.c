/**
 * Corey Donohoe <atmos@atmos.org>
 * Filename: entice_image.c
 * Smart Object: ;
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>
#include <Ecore.h>
#include "image.h"
#include "entice.h"

#define DEBUG 0

static void entice_image_resize(Evas_Object * o, Evas_Coord w, Evas_Coord h);
static int _entice_image_scroll_timer(void *data);

static void
_im_down_cb(void *data, Evas * evas, Evas_Object * obj, void *ev)
{
   Entice_Image *im = NULL;
   Evas_Event_Mouse_Down *e = NULL;
   Evas_Coord ox = 0.0, oy = 0.0;

   if ((im = evas_object_smart_data_get((Evas_Object *) data)))
   {
      if ((e = (Evas_Event_Mouse_Down *) ev))
      {
         evas_object_geometry_get(im->obj, &ox, &oy, NULL, NULL);
         im->mouse.dragging = 1;
         im->mouse.off_x = e->canvas.x - ox;
         im->mouse.off_y = e->canvas.y - oy;
      }
   }
}
static void
_im_up_cb(void *data, Evas * evas, Evas_Object * obj, void *ev)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get((Evas_Object *) data)))
   {
      im->mouse.dragging = 0;
      im->mouse.off_x = im->mouse.off_y = 0.0;
   }
}
static void
_im_move_cb(void *data, Evas * evas, Evas_Object * obj, void *ev)
{
   Entice_Image *im = NULL;
   Evas_Coord ox = 0.0, oy = 0.0;
   Evas_Coord ww = 0.0, hh = 0.0;
   Evas_Coord idx = 0.0, idy = 0.0;
   Evas_Event_Mouse_Move *e = NULL;

   if ((im = evas_object_smart_data_get((Evas_Object *) data)))
   {
      if (im->mouse.dragging)
      {
         if ((e = (Evas_Event_Mouse_Move *) ev))
         {
            ww = im->iw / im->zoom;
            hh = im->ih / im->zoom;
            evas_object_move(im->obj, e->cur.canvas.x - im->mouse.off_x,
                             e->cur.canvas.y - im->mouse.off_y);
            evas_object_geometry_get(im->obj, &ox, &oy, NULL, NULL);
            idx = (ww - (double) im->w);
            idy = (hh - (double) im->h);
            if (idx == 0.0)
               idx = 1.0;
            if (idy == 0.0)
               idy = 1.0;
            im->align.x = ((double) im->x - ox) / idx;
            im->align.y = ((double) im->y - oy) / idy;

            /* keep it bounded */
            if (im->align.x < 0.0)
               im->align.x = 0.0;
            else if (im->align.x > 1.0)
               im->align.x = 1.0;
            if (im->align.y < 0.0)
               im->align.y = 0.0;
            else if (im->align.y > 1.0)
               im->align.y = 1.0;

            entice_image_vertical_align_set(im->align.y);
            entice_image_horizontal_align_set(im->align.x);
         }
      }
   }
}
double
entice_image_x_align_get(Evas_Object * o)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      return (im->align.x);
   }
   return (0.5);
}

double
entice_image_y_align_get(Evas_Object * o)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      return (im->align.y);
   }
   return (0.5);
}

void
entice_image_x_align_set(Evas_Object * o, double align)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
#if DEBUG
      fprintf(stderr, "Adding X OFfset: %0.2f\n", offset);
#endif
      im->align.x = align;
   }

}
void
entice_image_y_align_set(Evas_Object * o, double align)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
#if DEBUG
      fprintf(stderr, "Adding Y OFfset: %0.2f\n", offset);
#endif
      im->align.y = align;
   }

}
void
entice_image_geometry_get(Evas_Object * o, Evas_Coord * x, Evas_Coord * y,
                          Evas_Coord * w, Evas_Coord * h)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      if (x)
         *x = im->x;
      if (y)
         *y = im->y;
      if (w)
         *w = im->w;
      if (h)
         *h = im->h;
   }
}
const char *
entice_image_format_get(Evas_Object * o)
{
   char *result = NULL;
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
      result = im->format;
   return (result);
}

void
entice_image_format_set(Evas_Object * o, const char *format)
{
   char buf[PATH_MAX];
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      if (im->format)
         free(im->format);
      snprintf(buf, PATH_MAX, "%s", format);
      im->format = strdup(buf);
   }
}

/**
 * entice_image_rotate - rotate the image using imlib2
 * @o - the Entice Image Object
 * @direction - 1 to flip clockwise, 3 to flip counter clockwise
 */
int
entice_image_rotate(Evas_Object * o, int orientation)
{
   int iw, ih;
   Evas_Coord w, h, x, y;
   Entice_Image *im = NULL;
   Imlib_Image imlib_im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      evas_object_image_size_get(im->obj, &iw, &ih);
      evas_object_geometry_get(o, &x, &y, &w, &h);

      if ((imlib_im =
           imlib_create_image_using_copied_data(im->iw, im->ih,
                                                evas_object_image_data_get
                                                (im->obj, 1))))
      {
         imlib_context_set_image(imlib_im);
         imlib_image_orientate(orientation);
         im->iw = imlib_image_get_width();
         im->ih = imlib_image_get_height();
         evas_object_image_size_set(im->obj, im->iw, im->ih);
         evas_object_image_fill_set(im->obj, 0.0, 0.0, im->iw, im->ih);
         evas_object_image_data_copy_set(im->obj, imlib_image_get_data());
         evas_object_resize(o, w, h);
         /* if we're fitting, it'll need to be recalculated */
         if (entice_image_zoom_fit_get(o))
            entice_image_zoom_fit(o);
         evas_damage_rectangle_add(evas_object_evas_get(o), x, y, w, h);
         imlib_free_image();
         return (1);
      }
   }
   return (0);
}

/**
 * entice_image_flip - flip the image using imlib2
 * @o - the Entice Image Object
 * @direction - non-zero to flip vertical, zero to flip horizontal
 */
int
entice_image_flip(Evas_Object * o, int orientation)
{
   int iw, ih;
   Evas_Coord w, h, x, y;
   Entice_Image *im = NULL;
   Imlib_Image imlib_im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      evas_object_image_size_get(im->obj, &iw, &ih);
      evas_object_geometry_get(o, &x, &y, &w, &h);

      if ((imlib_im =
           imlib_create_image_using_copied_data(iw, ih,
                                                evas_object_image_data_get
                                                (im->obj, 1))))
      {
         imlib_context_set_image(imlib_im);
         if (orientation)
            imlib_image_flip_horizontal();
         else
            imlib_image_flip_vertical();

         im->iw = imlib_image_get_width();
         im->ih = imlib_image_get_height();
         evas_object_image_size_set(im->obj, im->iw, im->ih);
         evas_object_image_data_copy_set(im->obj,
                                         imlib_image_get_data_for_reading_only
                                         ());
         evas_object_resize(o, w, h);
         /* if we're fitting, it'll need to be recalculated */
         if (entice_image_zoom_fit_get(o))
            entice_image_zoom_fit(o);
         evas_damage_rectangle_add(evas_object_evas_get(o), x, y, w, h);
         imlib_free_image();
         return (1);
      }
   }
   return (0);
}

/**
 * entice_image_save - save the image using imlib2
 * @o - the Entice Image Object
 */
int
entice_image_save(Evas_Object * o)
{
   int iw, ih, result = 0;
   Entice_Image *im = NULL;
   Imlib_Image imlib_im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      evas_object_image_size_get(im->obj, &iw, &ih);
      if ((imlib_im =
           imlib_create_image_using_copied_data(iw, ih,
                                                evas_object_image_data_get
                                                (im->obj, 1))))
      {
         imlib_context_set_image(imlib_im);
         if (im->format && im->filename)
         {
            char *tmp = NULL;
            char tmpfile[PATH_MAX];
            Imlib_Load_Error err;

            imlib_image_set_has_alpha((char)
                                      evas_object_image_alpha_get(im->obj));
            imlib_image_attach_data_value("quality", NULL, im->quality, NULL);
            if ((tmp = strrchr(im->filename, '.')))
               imlib_image_set_format(tmp + 1);
            snprintf(tmpfile, PATH_MAX, "%s.%d", im->filename, getpid());
            imlib_save_image_with_error_return(tmpfile, &err);
            evas_image_cache_flush(evas_object_evas_get(im->obj));
            switch (err)
            {
              case 0:
                 unlink(im->filename);
                 if (!rename(tmpfile, im->filename))
                    result = 1;
                 break;
              default:
                 fprintf(stderr, "Unable to save file(%d)", (int) err);
                 break;
            }
         }
         imlib_free_image();
      }
   }
   return (result);
}

void
entice_image_file_set(Evas_Object * o, const char *filename)
{
   char buf[PATH_MAX];
   Entice_Image *im = NULL;

   if ((filename) && (im = evas_object_smart_data_get(o)))
   {
      snprintf(buf, PATH_MAX, "%s", filename);
      if (im->filename)
         free(im->filename);
      im->filename = strdup(buf);
   }
}
const char *
entice_image_file_get(Evas_Object * o)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
      return (im->filename);
   return (NULL);
}

/**
 * entice_image_zoom_fit_get - find out whether we're fitting this obj
 * @o - the Entice_Image object
 * Return 1 if it is being fitted, 0 if it's not
 */
int
entice_image_zoom_fit_get(Evas_Object * o)
{
   int result = 0;
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      result = im->fit;
   }
   return (result);
}

/**
 * entice_image_zoom_get - get the current zoom value for the image
 * @o - The Entice_Image we're curious about
 */
double
entice_image_zoom_get(Evas_Object * o)
{
   double result = 1.0;
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
      result = im->zoom;
   return (result);
}

/**
 * entice_image_zoom_set - set the current zoom value for the image
 * @o - The Entice_Image we're curious about
 * @val - the new zoom value for our Image
 */
void
entice_image_zoom_set(Evas_Object * o, double val)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      im->zoom = val;
      im->fit = 0;
      evas_object_resize(o, im->w, im->h);
   }
}

/**
 * entice_image_zoom_fit - fit the current image to the clip
 * @o - The Entice_Image we're fitting
 */
void
entice_image_zoom_fit(Evas_Object * o)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      double wfactor, hfactor;

      wfactor = (double) (im->iw) / (double) (im->w);
      hfactor = (double) (im->ih) / (double) (im->h);
#if 0
      if (im->iw > im->ih)
         im->zoom = ((double) (im->iw) / (double) im->w);
      else
         im->zoom = ((double) (im->ih) / (double) im->h);
#endif
      im->zoom = (wfactor > hfactor ? wfactor : hfactor);
      im->fit = 1;
      entice_image_resize(o, im->w, im->h);
   }
}

/**
 * entice_image_zoom_reset - set the scale to be 1:1
 * @o - The Entice_Image we're resetting zoom for
 */
void
entice_image_zoom_reset(Evas_Object * o)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      im->zoom = 1.0;
      im->fit = 0;
      entice_image_resize(o, im->w, im->h);
   }
}

/**
 * entice_image_zoom_out - zoom out by a factor of zoom *= 1.414
 * @o - The Entice_Image we're zooming
 */
void
entice_image_zoom_out(Evas_Object * o)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
#if DEBUG
      fprintf(stderr, "Zooming Out!! %0.2f\n", im->zoom);
#endif
      im->zoom *= 1.414;
      im->fit = 0;
      entice_image_resize(o, im->w, im->h);
   }

}

/**
 * entice_image_zoom_in - zoom in by a factor of zoom *= 1.414
 * @o - The Entice_Image we're zooming
 */
void
entice_image_zoom_in(Evas_Object * o)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
#if DEBUG
      fprintf(stderr, "Zooming In!! %0.2f\n", im->zoom);
#endif
      im->zoom /= 1.414;
      if (im->zoom < 0.03125)
         im->zoom = 0.03125;
      im->fit = 0;
      entice_image_resize(o, im->w, im->h);
   }
}

#if 0
/**
 * _entice_image_scroll_timer - our ecore timer to do continuous
 * scrolling
 * @data - a pointer to the object we're scrolling
 * Returns 1 until the image's boundaries are reached, or the timer is
 * manually deleted elsewhere
 */
static int
_entice_image_scroll_timer(void *data)
{
   Entice_Image *im = NULL;
   int ok = 1;

   if (data && ((im = evas_object_smart_data_get((Evas_Object *) data))))
   {
      double dt, dx;
      double offset;
      Evas_Coord ix, iy, iw, ih;

      dt = ecore_time_get() - im->scroll.start_time;
      dx = 10 * (1 - exp(-dt));
      offset = dx * im->scroll.velocity;

      evas_object_geometry_get(im->obj, &ix, &iy, &iw, &ih);


      switch (im->scroll.direction)
      {
        case ENTICE_SCROLL_NORTH:
           if (ih > im->h)
           {
              iy += offset;
              im->scroll.y += offset;
              if (im->scroll.y > ((ih - im->h) / 2))
              {
                 im->scroll.y = ((ih - im->h) / 2);
                 ok = 0;
              }
           }
           break;
        case ENTICE_SCROLL_SOUTH:
           if (ih > im->h)
           {
              iy -= offset;
              im->scroll.y -= offset;
              if (im->scroll.y < -((ih - im->h) / 2))
              {
                 im->scroll.y = -((ih - im->h) / 2);
                 ok = 0;
              }

           }
           break;
        case ENTICE_SCROLL_EAST:
           if (iw > im->w)
           {
              ix -= offset;
              im->scroll.x -= offset;
              if (im->scroll.x < -((iw - im->w) / 2))
              {
                 im->scroll.x = -((iw - im->w) / 2);
                 ok = 0;
              }
           }
           break;
        case ENTICE_SCROLL_WEST:
           if (iw > im->w)
           {
              ix += offset;
              im->scroll.x += offset;
              if (im->scroll.x > ((iw - im->w) / 2))
              {
                 im->scroll.x = ((iw - im->w) / 2);
                 ok = 0;
              }
           }
           break;
        default:
#if DEBUG
           fprintf(stderr, "Scrolling\n");
#endif
           break;
      }
      evas_object_resize((Evas_Object *) data, im->w, im->h);
      if (!ok)
      {
         ecore_timer_del(im->scroll.timer);
         im->scroll.timer = NULL;
      }
      /* 
         evas_object_move(im->obj, ix, iy); */
   }
   return (ok);
}
#endif

/*=========================================================================
 * Entice_Image smart object definitions
 *=======================================================================*/
static void
entice_image_add(Evas_Object * o)
{
   Entice_Image *im = NULL;

   im = (Entice_Image *) malloc(sizeof(Entice_Image));
   memset(im, 0, sizeof(Entice_Image));
   im->zoom = 1.0;

   im->clip = evas_object_rectangle_add(evas_object_evas_get(o));
   evas_object_color_set(im->clip, 255, 255, 255, 255);
   evas_object_layer_set(im->clip, 0);

   im->zoom = 1.0;
   im->fit = 1;
   evas_object_smart_data_set(o, im);
}
static void
entice_image_del(Evas_Object * o)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      if (im->filename)
         free(im->filename);
      if (im->format)
         free(im->format);
      if (im->obj)
         evas_object_del(im->obj);
      if (im->clip)
         evas_object_del(im->clip);
      free(im);
   }
}
static void
entice_image_layer_set(Evas_Object * o, int layer)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      evas_object_layer_set(im->obj, layer);
   }
}
static void
entice_image_raise(Evas_Object * o)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      evas_object_raise(im->obj);
   }
}
static void
entice_image_lower(Evas_Object * o)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      evas_object_lower(im->obj);
   }
}
static void
entice_image_stack_above(Evas_Object * o, Evas_Object * above)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      evas_object_stack_above(im->obj, above);
   }
}
static void
entice_image_stack_below(Evas_Object * o, Evas_Object * below)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      evas_object_stack_below(im->obj, below);
   }
}
static void
entice_image_move(Evas_Object * o, Evas_Coord x, Evas_Coord y)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      if (im->x == x && im->y == y)
         return;
      im->dx = im->x - x;
      im->dy = im->y - y;

      im->x = x;
      im->y = y;
      entice_image_resize(o, im->w, im->h);
   }
}
static void
entice_image_resize(Evas_Object * o, Evas_Coord w, Evas_Coord h)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      Evas_Coord ww = 0.0, hh = 0.0;

      im->w = w;
      im->h = h;

      evas_object_move(im->clip, im->x, im->y);
      evas_object_resize(im->clip, w, h);
      if (w < 5 || h < 5)
         return;
      if (im->zoom > w || im->zoom > h)
         im->zoom = w < h ? w : h;

      ww = (Evas_Coord) ((double) im->iw / im->zoom);
      hh = (Evas_Coord) ((double) im->ih / im->zoom);
#if 0
      if (ww > w)
      {
         if (im->scroll.x > ((ww - w) / 2))
            im->scroll.x = ((ww - w) / 2);
         else if (im->scroll.x < -((ww - w + 1) / 2))
            im->scroll.x = -((ww - w + 1) / 2);
      }
      else
      {
         if (im->scroll.x > ((w - ww) / 2))
            im->scroll.x = ((w - ww) / 2);
         else if (im->scroll.x < -((w - ww + 1) / 2))
            im->scroll.x = -((w - ww + 1) / 2);
      }
      if (hh > h)
      {
         if (im->scroll.y > ((hh - h) / 2))
            im->scroll.y = ((hh - h) / 2);
         else if (im->scroll.y < -((hh - h + 1) / 2))
            im->scroll.y = -((hh - h + 1) / 2);
      }
      else
      {
         if (im->scroll.y > ((h - hh) / 2))
            im->scroll.y = ((h - hh) / 2);
         else if (im->scroll.y < -((h - hh + 1) / 2))
            im->scroll.y = -((h - hh + 1) / 2);
      }
#endif
      evas_object_resize(im->obj, ww, hh);
      evas_object_image_fill_set(im->obj, 0, 0, ww, hh);
      evas_object_move(im->obj, im->x - ((ww - im->w) * im->align.x),
                       im->y - ((hh - im->h) * im->align.y));
   }
}
static void
entice_image_show(Evas_Object * o)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      evas_object_show(im->clip);
   }
}
static void
entice_image_hide(Evas_Object * o)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      evas_object_hide(im->clip);
   }
}
static void
entice_image_color_set(Evas_Object * o, int r, int g, int b, int a)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      evas_object_color_set(im->clip, r, g, b, a);
   }
}
static void
entice_image_clip_set(Evas_Object * o, Evas_Object * clip)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      evas_object_clip_set(im->clip, clip);
   }
}
static void
entice_image_clip_unset(Evas_Object * o)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      evas_object_clip_unset(im->clip);
   }
}
static Evas_Smart *
entice_image_get(void)
{
   static Evas_Smart *s = NULL;

   if (!s)
   {
      s = evas_smart_new("EnticeImage", entice_image_add, entice_image_del,
                         entice_image_layer_set, entice_image_raise,
                         entice_image_lower, entice_image_stack_above,
                         entice_image_stack_below, entice_image_move,
                         entice_image_resize, entice_image_show,
                         entice_image_hide, entice_image_color_set,
                         entice_image_clip_set, entice_image_clip_unset,
                         NULL);
   }
   return (s);
}

Evas_Object *
entice_image_new(Evas_Object * image)
{
   int w, h;
   Evas_Object *o = NULL;
   Entice_Image *im = NULL;

   if (image)
   {
      o = evas_object_smart_add(evas_object_evas_get(image),
                                entice_image_get());

      im = evas_object_smart_data_get(o);
      im->obj = image;

      evas_object_image_size_get(im->obj, &w, &h);
      evas_object_clip_set(im->obj, im->clip);
      evas_object_show(im->obj);
      evas_object_pass_events_set(im->clip, 1);
      evas_object_repeat_events_set(im->obj, 1);
      evas_object_event_callback_add(im->obj, EVAS_CALLBACK_MOUSE_DOWN,
                                     _im_down_cb, o);
      evas_object_event_callback_add(im->obj, EVAS_CALLBACK_MOUSE_UP,
                                     _im_up_cb, o);
      evas_object_event_callback_add(im->obj, EVAS_CALLBACK_MOUSE_MOVE,
                                     _im_move_cb, o);
      evas_object_move(im->obj, 0, 0);

      im->iw = w;
      im->ih = h;
   }
   return (o);
}

void
entice_image_dragable_state_set(Evas_Object * o, int state)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      im->state = state;
   }
}

void
entice_image_save_quality_set(Evas_Object * o, int quality)
{
   Entice_Image *im = NULL;

   if ((im = evas_object_smart_data_get(o)))
   {
      if ((quality >= 70) && (quality <= 100))
         im->quality = quality;
   }
}
