#include "private.h"
#include <Ecore.h>
#include <math.h>

#define HACK_FLUSH_EVENTS_DURING_LONG_CALCULATE 1
#ifdef HACK_FLUSH_EVENTS_DURING_LONG_CALCULATE
static const double LONG_CALCULATE_TIMEOUT = 0.6;
#include <Ecore_X.h>
#include <X11/Xlib.h>
#endif

/* value to consider as double/float error, differences smaller than
 * this are ignored
 */
static const double DOUBLE_ERROR = 0.00001;

/* constant to multiply offsets.
 *
 * The smaller the value, the faster it will stop.
 */
static const double KINETIC_FRICTION = 1.0;

/* Minimum value to consider velocity worth doing kinetic animation */
static const Evas_Coord KINETIC_VELOCITY_THRESHOLD = 20;

/* How long the auto zoom animation should take, in seconds */
static const double ZOOM_AUTO_ANIMATION_DURATION = 0.5;

/* Timeout in seconds to consider double-click an auto zoom */
static const double ZOOM_AUTO_TIMEOUT = 0.2;

/* Padding in pixels around object being zoomed */
static const Evas_Coord ZOOM_AUTO_PADDING = 15;

static const float ZOOM_AUTO_MIN = 0.3;
static const float ZOOM_AUTO_MAX = 4.0;
static const float ZOOM_AUTO_MIN_DIFFERENCE = 0.01;

/* How long the interactive zoom animation should take, in seconds */
static const double ZOOM_STEP_ANIMATION_DURATION = 0.8;

/* Threshold in pixels to change zoom step. */
static const Evas_Coord ZOOM_STEP_THRESHOLD = 50;
static const float ZOOM_STEPS[] = {
   0.3, 0.5, 0.67, 0.8, 0.9, 1.0, 1.1, 1.2, 1.33, 1.5, 1.7, 2.0, 2.4, 3.0,
};
static const unsigned int ZOOM_STEPS_LAST = sizeof(ZOOM_STEPS) / sizeof(ZOOM_STEPS[0]);

/*
 * Structure filled by ewk_view_single_smart_set() from
 * view_add(). This contains the pointers to vanilla/pristine
 * ewk_view_single, which will be used to implement super() behavior
 * in C.
 */
static Ewk_View_Smart_Class _parent_sc = EWK_VIEW_SMART_CLASS_INIT_NULL;

typedef struct _View_Smart_Data   View_Smart_Data;

#define VIEW_SD_GET_OR_RETURN(o, sd, ...)\
   View_Smart_Data * sd = evas_object_smart_data_get(o);\
   if (!sd)\
     {\
        CRITICAL("no smart data for object %p [%s]",\
                 o, evas_object_type_get(o));\
        return __VA_ARGS__;\
     }

#define EWK_VIEW_SD_GET_OR_RETURN(o, sd, ...)\
   Ewk_View_Smart_Data * sd = evas_object_smart_data_get(o);\
   if (!sd)\
     {\
        CRITICAL("no smart data for object %p [%s]",\
                 o, evas_object_type_get(o));\
        return __VA_ARGS__;\
     }

struct point_history
{
   Evas_Coord x, y;
   double     timestamp;
};

/* Extends Ewk_View_Smart_Data to add some members we'll need */
struct _View_Smart_Data
{
   /* Original type must be the first, so ewk_view and ewk_view_single know
    * how to access it (the memory offsets will remain the same).
    */
   Ewk_View_Smart_Data base;

   /* where to keep track of our animators */
   struct
   {
      Ecore_Animator *pan;
      Ecore_Animator *zoom;
      Ecore_Animator *kinetic;
   } animator;
   Ecore_Idler *idler_close_window;

   /* context used during pan/scroll animator */
   struct
   {
      /* circular array with collected samples (see _view_smart_mouse_move()) */
#define PAN_HISTORY_SIZE (20)
      struct point_history history[PAN_HISTORY_SIZE];
      unsigned int         idx; /* index in the circular buffer of the last stored value */
      unsigned int         count; /* total number of samples collected */

      struct point_history last_move; /* used by _view_animator_pan() */
   } pan;

   Evas_Event_Mouse_Down mouse_down_copy; /* copy of down event, used by _view_smart_mouse_up() */

   struct
   {
      struct
      {
         float      zoom;
         Evas_Coord x, y;
         double     timestamp;
         int        step_idx;
      } start;

      struct
      {
         Evas_Coord y;
         float      zoom;
         double     timestamp;
         int        step_idx;
      } last, next;

      struct
      {
         float  zoom;
         double time;
      } diff;

      float     min_zoom;

      Eina_Bool interactive : 1;
   } zoom;

   /* context used during kinetic animator */
   struct
   {
      /* starting point */
      struct
      {
         Evas_Coord dx, dy;
         double     timestamp;
      } start;

      /* last run */
      struct
      {
         Evas_Coord dx, dy;
      } last;

      /* flags to state we ended animation in those axis */
      struct
      {
         Eina_Bool x : 1;
         Eina_Bool y : 1;
      } done;
   } kinetic;

   /* general flags */
   struct
   {
      Eina_Bool first_calculate : 1;
      Eina_Bool animated_zoom : 1;
      Eina_Bool touch_interface : 1;
   } flags;
#if 0
   Evas_Object *context_menu;
#endif
};

/***********************************************************************
 * Helper functions                                                    *
 **********************************************************************/

/* calculates one good zoom level to fit given hit test result */
static float
_view_zoom_fits_hit(View_Smart_Data *sd, const Ewk_Hit_Test *hit_test)
{
   Evas_Coord x, y; /* hit test point */
   Evas_Coord bx, by, bw, bh, bw1, bw2, bh1, bh2; /* bounding box */
   Evas_Coord vx, vy, vw, vh, vw1, vw2, vh1, vh2; /* visible content */
   Evas_Coord cw, ch;
   float zx, zy, z, old_zoom, zx1, zx2, zy1, zy2;

   if (!hit_test)
      return 0.0;

   old_zoom = ewk_frame_page_zoom_get(sd->base.main_frame);

   /* save some typing */
   x = hit_test->x;
   y = hit_test->y;

   bx = hit_test->bounding_box.x;
   by = hit_test->bounding_box.y;
   bw = hit_test->bounding_box.w;
   bh = hit_test->bounding_box.h;

   if ((bw <= 0) || (bh <= 0))
      return 0.0;

   ewk_frame_visible_content_geometry_get
      (sd->base.main_frame, EINA_FALSE, &vx, &vy, &vw, &vh);
   if ((vw <= 0) || (vh <= 0))
      return 0.0;

   ewk_frame_contents_size_get(sd->base.main_frame, &cw, &ch);
   if ((cw <= 0) || (ch <= 0))
      return 0.0;

   /* split width of left and right parts from hit test position */
   bw1 = x - bx;
   bw2 = bw - bw1;

   /* split height of top and bottom parts from hit test position */
   bh1 = y - by;
   bh2 = bh - bh1;

   /* split size of viewport as well. We'll use this as the available space */
   vw1 = x - vx;
   vw2 = vw - vw1;

   vh1 = y - vy;
   vh2 = vh - vh1;

   /* check if we can add padding around those sizes, "adding" if possible
    * (we actually subtract as adding a pad is removing available space)
    */
   if (vw1 > ZOOM_AUTO_PADDING)
      vw1 -= ZOOM_AUTO_PADDING;

   if (vw2 > ZOOM_AUTO_PADDING)
      vw2 -= ZOOM_AUTO_PADDING;

   if (vh1 > ZOOM_AUTO_PADDING)
      vh1 -= ZOOM_AUTO_PADDING;

   if (vh2 > ZOOM_AUTO_PADDING)
      vh2 -= ZOOM_AUTO_PADDING;

   /* check individual zoom to fit each part */
   zx1 = (bw1 > 0) ? (vw1 / (float)bw1) : 0.0;
   zx2 = (bw2 > 0) ? (vw2 / (float)bw2) : 0.0;

   zy1 = (bh1 > 0) ? (vh1 / (float)bh1) : 0.0;
   zy2 = (bh2 > 0) ? (vh2 / (float)bh2) : 0.0;

   if ((zx1 >= ZOOM_AUTO_MIN_DIFFERENCE) && (zx2 >= ZOOM_AUTO_MIN_DIFFERENCE))
      zx = (zx1 < zx2) ? zx1 : zx2;
   else if (zx1 >= ZOOM_AUTO_MIN_DIFFERENCE)
      zx = zx1;
   else
      zx = zx2;

   if ((zy1 >= ZOOM_AUTO_MIN_DIFFERENCE) && (zy2 >= ZOOM_AUTO_MIN_DIFFERENCE))
      zy = (zy1 < zy2) ? zy1 : zy2;
   else if (zy1 >= ZOOM_AUTO_MIN_DIFFERENCE)
      zy = zy1;
   else
      zy = zy2;

   if ((zx >= ZOOM_AUTO_MIN_DIFFERENCE) && (zy >= ZOOM_AUTO_MIN_DIFFERENCE))
      z = (zx < zy) ? zx : zy;
   else if (zx >= ZOOM_AUTO_MIN_DIFFERENCE)
      z = zx;
   else
      z = zy;

   /* zoom will make contents be smaller than viewport, limit it */
   if (((int)(z * old_zoom * cw) < vw) || ((int)(z * old_zoom * ch) < vh))
     {
        float ac = cw / (float)ch;
        float av = vw / (float)vh;

        if (ac < av)
           z = vw / (float)cw;
        else
           z = vh / (float)ch;
     }

#if 0
   /* debug */
   printf(">>> fit: center=%3d,%3d   box=%3d,%3d+%3dx%3d\n"
          "    x:  %3d = %3d + %3d,  %3d = %3d + %3d,  %2.4f  %2.4f  -> %2.4f\n"
          "    y:  %3d = %3d + %3d,  %3d = %3d + %3d,  %2.4f  %2.4f  -> %2.4f\n"
          "    final: %2.4f   %2.4f (old=%0.3f, difference=%0.3f)\n"
          "    contents: %4dx%4d  -> %4dx%4d\n"
          "\n",
          x, y, bx, by, bw, bh,
          bw, bw1, bw2, vw, vw1, vw2, zx1, zx2, zx,
          bh, bh1, bh2, vh, vh1, vh2, zy1, zy2, zy,
          z, old_zoom * z, old_zoom, fabs(old_zoom - z),
          cw, ch, (int)(z * old_zoom * cw), (int)(z * old_zoom * ch));
#endif

   z *= old_zoom;

   if (z < ZOOM_AUTO_MIN)
      z = ZOOM_AUTO_MIN;
   else if (z > ZOOM_AUTO_MAX)
      z = ZOOM_AUTO_MAX;

   if (fabs(old_zoom - z) < ZOOM_AUTO_MIN_DIFFERENCE)
      return 0.0;

   return z;
}

/* remove flag saying that object is animating zoom */
static void
_view_zoom_animated_end(void *data, Evas_Object *view __UNUSED__, void *event_info __UNUSED__)
{
   View_Smart_Data *sd = data;
   sd->flags.animated_zoom = EINA_FALSE;
}

/* ask for pre-render when load finished */
static void
_view_load_finished(void *data, Evas_Object *view, void *event_info __UNUSED__)
{
   View_Smart_Data *sd = data;
   float zoom = ewk_frame_page_zoom_get(sd->base.main_frame);
   Evas_Coord x, y, w, h;
   ewk_frame_visible_content_geometry_get
      (sd->base.main_frame, EINA_TRUE, &x, &y, &w, &h);

   w *= 2;
   h *= 2;

   INF("load finished, pre-render %d,%d+%dx%d at %0.2f", x, y, w, h, zoom);
   ewk_view_pre_render_region(view, x, y, w, h, zoom);
}

/* stop animators, we changed page */
static void
_view_uri_changed(void *data, Evas_Object *view, void *event_info __UNUSED__)
{
   View_Smart_Data *sd = data;

   if (sd->animator.pan)
     {
        ecore_animator_del(sd->animator.pan);
        sd->animator.pan = NULL;
     }

   if (sd->animator.kinetic)
     {
        ecore_animator_del(sd->animator.kinetic);
        sd->animator.kinetic = NULL;
     }

   if (sd->animator.zoom)
     {
        /* inform  ewk_view that we finished performing zoom animation */
        ewk_view_zoom_animated_mark_stop(view);
        evas_object_smart_callback_call(view, "zoom,interactive,end", NULL);
        ecore_animator_del(sd->animator.zoom);
        sd->animator.zoom = NULL;
     }
}

/* ask ewk_view to pre render in the given direction.
 *
 * This uses a heuristics to create a pre-render region using the
 * given motion vector.
 */
static void
_view_pan_pre_render(View_Smart_Data *sd, Evas_Coord dx, Evas_Coord dy)
{
   float zoom = ewk_frame_page_zoom_get(sd->base.main_frame);
   double weightx, weighty;
   Evas_Coord x, y, w, h, px, py, pw, ph;
   unsigned int vx, vy;

   /* where are we now */
   ewk_frame_visible_content_geometry_get
      (sd->base.main_frame, EINA_TRUE, &x, &y, &w, &h);

   /* get absolute value for dx and dy, remove it's precision */
   vx = abs(dx) >> 4;
   vy = abs(dy) >> 4;

   if (vx == 0 && vy == 0)
      return;  /* motion vector is not significant, don't pre-render */
   else if (vx < vy)
     {
        /* moving more on Y-axis than X. */
        weightx = 0.5;
        weighty = 1.0;
     }
   else if (vx > vy)
     {
        /* moving more on X-axis than X. */
        weightx = 1.0;
        weighty = 0.5;
     }
   else
     {
        /* moving equally on both axis, be more conservative */
        weightx = 0.6;
        weighty = 0.6;
     }

   /* if values were not significant, zero their extra weight */
   if (vx == 0)
      weightx = 0.0;
   else if (vy == 0)
      weighty = 0.0;

   /* use single region that includes existing viewport.
    *
    * This is simple and should work. Another option is to do more
    * detailed analysis and possible create special cases for
    * vertical, horizontal and diagonal moves, with diagonal being
    * smarter and rendering less than we do now.
    */
   pw = w * (1.0 + weightx);
   ph = h * (1.0 + weighty);

   px = x;
   py = y;

   if (dx > 0)
      px -= (pw - w);

   if (dy > 0)
      py -= (ph - h);

   INF("pre-render region %d,%d+%dx%d at %0.2f (viewport=%d,%d+%dx%d)",
       px, py, pw, ph, zoom, x, y, w, h);
   ewk_view_pre_render_region(sd->base.self, px, py, pw, ph, zoom);
}

static unsigned int
_view_zoom_closest_index_find(float zoom)
{
   unsigned int i, close_idx;
   float close_zoom;

   close_idx = 0;
   close_zoom = 999999.99;
   for (i = 0; i < ZOOM_STEPS_LAST; i++)
     {
        float cur = fabs(zoom - ZOOM_STEPS[i]);
        if (cur < close_zoom)
          {
             close_idx = i;
             close_zoom = cur;
             if (cur < DOUBLE_ERROR) /* close enough */
                break;
          }
     }

   return close_idx;
}

/***********************************************************************
 * Animators: shared timers at fixed frame rate/interval.              *
 **********************************************************************/

/* Animator that implements the kinetic animation (momentum).
 *
 * This is started by _view_smart_mouse_up() if it is worth doing the
 * animation.
 *
 * Code is heavily based on els_scroller.c from Elementary.
 */
static Eina_Bool
_view_animator_kinetic(void *data)
{
   View_Smart_Data *sd = data;
   double p, dt, now = ecore_loop_time_get();
   Evas_Coord x, y, sx, sy, sw, sh;

   dt = now - sd->kinetic.start.timestamp;
   /* time difference is too small, ignore it */
   if (dt <= DOUBLE_ERROR)
      goto end;

   dt = dt / KINETIC_FRICTION;
   if (dt > 1.0)
      dt = 1.0;

   p = 1.0 - ((1.0 - dt) * (1.0 - dt));

   /* query scroll area and current position */
   ewk_frame_scroll_pos_get(sd->base.main_frame, &sx, &sy);
   ewk_frame_scroll_size_get(sd->base.main_frame, &sw, &sh);

   if (!sd->kinetic.done.x)
     {
        /* we're not done on x-axis yet, calculate new displacement */
        Evas_Coord dx = sd->kinetic.start.dx * KINETIC_FRICTION * p;
        x = sd->kinetic.last.dx - dx;
        sd->kinetic.last.dx = dx;

        /* check if new displacement fit in scroll area */
        if (sx + x < 0)
          {
             x = -sx;
             sd->kinetic.done.x = EINA_TRUE;
          }
        else if (sx + x >= sw)
          {
             x = sw - sx;
             sd->kinetic.done.x = EINA_TRUE;
          }
     }
   else
      x = 0;

   if (!sd->kinetic.done.y)
     {
        /* we're not done on y-axis yet, calculate new displacement */
        Evas_Coord dy = sd->kinetic.start.dy * KINETIC_FRICTION * p;
        y = sd->kinetic.last.dy - dy;
        sd->kinetic.last.dy = dy;

        /* check if new displacement fit in scroll area */
        if (sy + y < 0)
          {
             y = -sy;
             sd->kinetic.done.y = EINA_TRUE;
          }
        else if (sy + y >= sh)
          {
             y = sh - sy;
             sd->kinetic.done.y = EINA_TRUE;
          }
     }
   else
      y = 0;

   /* if there is anything to scroll, ask it */
   if (x != 0 || y != 0)
      ewk_frame_scroll_add(sd->base.main_frame, x, y);

   /* if we finished our work, just stop the animator */
   if (dt >= 1.0 || (sd->kinetic.done.x && sd->kinetic.done.y))
     {
        _view_pan_pre_render(sd, sd->kinetic.start.dx, sd->kinetic.start.dy);
        sd->animator.kinetic = NULL;
        return ECORE_CALLBACK_CANCEL;
     }

end:
   return ECORE_CALLBACK_RENEW; /* keep running until we finish */
}

/* Animator to apply scroll/pan at fixed frame rate.
 *
 * If pointer (mouse) was moved since last run, then request main
 * frame to scroll.
 */
static Eina_Bool
_view_animator_pan(void *data)
{
   View_Smart_Data *sd = data;
   Evas_Coord x, y, dx, dy;
   double timestamp;

   evas_pointer_canvas_xy_get(sd->base.base.evas, &x, &y);

   /* webkit is swapped */
   dx = sd->pan.last_move.x - x;
   dy = sd->pan.last_move.y - y;

   /* ignore this sample if it did not change */
   if ((dx == 0) && (dy == 0))
      goto end;

   timestamp = ecore_loop_time_get();
   if (timestamp <= sd->pan.last_move.timestamp) /* did time went backwards?! */
      goto end;

   /* request scroll by same displacement */
   ewk_frame_scroll_add(sd->base.main_frame, dx, dy);

   /* save new last position */
   sd->pan.last_move.x = x;
   sd->pan.last_move.y = y;
   sd->pan.last_move.timestamp = timestamp;

end:
   return ECORE_CALLBACK_RENEW; /* keep running until something else remove */
}

/* start pan/scroll animation */
static void
_view_pan_start(View_Smart_Data *sd, const Evas_Event_Mouse_Down *ev)
{
   struct point_history *p;

   /* not really required, but let's clean it */
   memset(&sd->pan, 0, sizeof(sd->pan));

   /* start history with one sample being the down position */
   sd->pan.idx = 0;
   sd->pan.count = 1;
   p = sd->pan.history;
   p->x = ev->canvas.x;
   p->y = ev->canvas.y;
   p->timestamp = ecore_loop_time_get();

   sd->pan.last_move = *p;

   ewk_view_pre_render_cancel(sd->base.self);

   /* register function to collect samples and apply scrolls at fixed interval*/
   if (!sd->animator.pan)
      sd->animator.pan = ecore_animator_add(_view_animator_pan, sd);
}

/* stop pan animation and possible schedule a kinetic scrolling.
 *
 * @return @c EINA_TRUE if event was handled inside this function and
 *         should not be used by calling function or @c EINA_FALSE if
 *         event is unused and should be forwarded by caller.
 */
static Eina_Bool
_view_pan_stop(View_Smart_Data *sd, const Evas_Event_Mouse_Up *ev)
{
   double t, at;
   Evas_Coord ax, ay, dx, dy, vel;
   unsigned int i, todo, samples;

   /* Stop pan animator, we're done already. */
   if (sd->animator.pan)
     {
        ecore_animator_del(sd->animator.pan);
        sd->animator.pan = NULL;
     }

   /* If we do not have enough samples, just assume it was a click */
   if (sd->pan.count < 2)
      return EINA_FALSE;

   /* the following code is heavily based on els_scroller.c from Elementary.
    *
    * It will check the samples from history and get the average point.
    */
   t = ecore_loop_time_get();

   ax = ev->canvas.x;
   ay = ev->canvas.y;
   at = 0.0;

   i = sd->pan.idx;
   todo = sd->pan.count > PAN_HISTORY_SIZE ? PAN_HISTORY_SIZE : sd->pan.count;
   samples = 0;

   for (; todo > 0; todo--)
     {
        struct point_history *p = sd->pan.history + i;
        double dt = t - p->timestamp;

        if (dt > 0.2 && samples > 0)
           break;

        at += dt;
        ax += p->x;
        ay += p->y;
        samples++;

        if (i > 0)
           i--;
        else
           i = PAN_HISTORY_SIZE - 1;
     }

   /* time was too short, consider it a click */
   if (at <= DOUBLE_ERROR)
      return EINA_FALSE;

   ax /= samples + 1;
   ay /= samples + 1;
   at *= 4.0;

   dx = ev->canvas.x - ax;
   dy = ev->canvas.y - ay;

   vel = sqrt((dx * dx) + (dy * dy)) / at;
   /* velocity was too short, consider it a click */
   if (vel <= KINETIC_VELOCITY_THRESHOLD)
     {
        _view_pan_pre_render(sd, dx, dy);
        return EINA_FALSE;
     }

   /* it's really woth animating. setup kinetic animation context and start
    * animator function.
    */
   sd->kinetic.start.timestamp = t;
   sd->kinetic.start.dx = dx;
   sd->kinetic.start.dy = dy;
   sd->kinetic.last.dx = 0;
   sd->kinetic.last.dy = 0;
   sd->kinetic.done.x = EINA_FALSE;
   sd->kinetic.done.y = EINA_FALSE;

   sd->animator.kinetic = ecore_animator_add(_view_animator_kinetic, sd);
   return EINA_TRUE;
}

/* Animator to collect mouse position and apply zoom at fixed frame rate.
 *
 * This animator works in two parts:
 *
 *    1. check if mouse moved up/down enough to change the zoom level
 *       to another band. We move in bands specified in ZOOM_STEPS every
 *       ZOOM_STEP_THRESHOLD pixels, up or down.
 *
 *    2. interpolates zoom level using weak zoom progressively until
 *       we reach desired level (sd->zoom.next.zoom) in calculated
 *       time slot (based on ZOOM_STEP_ANIMATION_DURATION).
 */
static Eina_Bool
_view_animator_zoom(void *data)
{
   View_Smart_Data *sd = data;
   Evas_Object *o = sd->base.self;
   Evas_Coord y, dy;
   double timestamp = ecore_loop_time_get();
   int step_inc, idx;

   /* wait to know if we're in auto zoom or interactive zooming */
   if (timestamp - sd->zoom.start.timestamp <= ZOOM_AUTO_TIMEOUT)
      goto end;

   if (!sd->zoom.interactive)
     {
        evas_object_smart_callback_call(o, "zoom,interactive,start", NULL);
        sd->zoom.interactive = EINA_TRUE;
     }

   evas_pointer_canvas_xy_get(sd->base.base.evas, NULL, &y);
   dy = y - sd->zoom.start.y;
   step_inc = dy / ZOOM_STEP_THRESHOLD;

   /* find out new step */
   idx = step_inc + sd->zoom.start.step_idx;

   /* check for out of bounds access */
   if (idx < 0)
      idx = 0;
   else if (idx >= (int)ZOOM_STEPS_LAST)
      idx = ZOOM_STEPS_LAST - 1;

   if (ZOOM_STEPS[idx] < sd->zoom.min_zoom)
      idx = sd->zoom.next.step_idx;

   /* Part 1. check if mouse moved enough to change zoom level band */
   if (idx != sd->zoom.next.step_idx)
     {
        View_Zoom_Interactive data;
        /* new target (next) values */
        sd->zoom.next.step_idx = idx;
        sd->zoom.next.zoom = ZOOM_STEPS[idx];
        sd->zoom.next.timestamp = timestamp + ZOOM_STEP_ANIMATION_DURATION;
        sd->zoom.diff.zoom = sd->zoom.next.zoom - sd->zoom.last.zoom;
        sd->zoom.diff.time = sd->zoom.next.timestamp - sd->zoom.last.timestamp;

        /* we'll animated, disable smooth scaling in evas so it's faster */
        ewk_view_zoom_weak_smooth_scale_set(o, EINA_FALSE);

        /* inform user that new level was requested */
        data.x = sd->zoom.start.x;
        data.y = sd->zoom.start.y;
        data.zoom = sd->zoom.next.zoom;
        evas_object_smart_callback_call(o, "zoom,interactive", &data);
     }

   if (sd->zoom.next.timestamp < DOUBLE_ERROR)
      goto end;  /* sd->zoom.next.timestamp is zero, we're stopped */

   /* Part 2. interpolate values to animate zoom change */
   else if (timestamp >= sd->zoom.next.timestamp)
     {
        /* we're done, enable smooth scaling so the still image looks better
         * and apply the final zoom level
         */
        ewk_view_zoom_weak_smooth_scale_set(o, EINA_TRUE);
        ewk_view_zoom_weak_set
           (o, sd->zoom.next.zoom, sd->zoom.start.x, sd->zoom.start.y);

        sd->zoom.last = sd->zoom.next;
        sd->zoom.next.timestamp = 0.0; /* say we're stopped, see above */
     }
   else if (sd->zoom.diff.time > 0.0)
     {
        /* regular intermediate animation frame, interpolate and apply */
        float zoom, p;

        p = (timestamp - sd->zoom.last.timestamp) / sd->zoom.diff.time;
        zoom = sd->zoom.last.zoom + sd->zoom.diff.zoom * p;

        ewk_view_zoom_weak_set(o, zoom, sd->zoom.start.x, sd->zoom.start.y);
     }

end:
   return ECORE_CALLBACK_RENEW; /* keep running until something else remove */
}

/* start zoom animation */
static void
_view_zoom_start(View_Smart_Data *sd, const Evas_Event_Mouse_Down *ev)
{
   Evas_Object *frame = sd->base.main_frame;
   Evas_Coord cw, ch, vw, vh;
   float z, zx, zy;

   ewk_view_pre_render_cancel(sd->base.self);

   /* remember starting point so we have a reference */
   sd->zoom.start.zoom = ewk_frame_page_zoom_get(frame);
   sd->zoom.start.x = ev->canvas.x;
   sd->zoom.start.y = ev->canvas.y;
   sd->zoom.start.timestamp = ecore_loop_time_get();

   /* find out which zoom level band we're in, closest match */
   sd->zoom.start.step_idx = _view_zoom_closest_index_find(sd->zoom.start.zoom);

   sd->zoom.last.y = sd->zoom.start.x;
   sd->zoom.last.zoom = sd->zoom.start.zoom;
   sd->zoom.last.timestamp = sd->zoom.start.timestamp;

   sd->zoom.next.y = sd->zoom.start.x;
   sd->zoom.next.zoom = sd->zoom.start.zoom;
   sd->zoom.next.timestamp = 0.0; /* this means we're stopped */

   sd->zoom.diff.zoom = 0.0;
   sd->zoom.diff.time = 0.0;

   sd->zoom.interactive = EINA_FALSE;

   /* find out the minimum zoom level that contents is greater or equal
    * viewport size
    */
   ewk_frame_visible_content_geometry_get(frame, 0, NULL, NULL, &vw, &vh);
   ewk_frame_contents_size_get(frame, &cw, &ch);

   zx = vw / (float)cw;
   zy = vh / (float)ch;
   z = (zx > zy) ? zx : zy;
   z *= sd->zoom.start.zoom;
   if (z >= ZOOM_STEPS[0])
      sd->zoom.min_zoom = z;
   else
      sd->zoom.min_zoom = ZOOM_STEPS[0];

   /* inform ewk_view that we'll perform an animation with zoom */
   ewk_view_zoom_animated_mark_start(sd->base.self, sd->zoom.start.zoom);

   /* register function to query pointer position and apply new zoom */
   if (!sd->animator.zoom)
      sd->animator.zoom = ecore_animator_add(_view_animator_zoom, sd);
}

/* stop zoom animation.
 *
 * @return @c EINA_TRUE if event was handled inside this function and
 *         should not be used by calling function or @c EINA_FALSE if
 *         event is unused and should be forwarded by caller.
 */
static Eina_Bool
_view_zoom_stop(View_Smart_Data *sd, const Evas_Event_Mouse_Up *ev __UNUSED__)
{
   double timestamp = ecore_loop_time_get();
   Evas_Object *view = sd->base.self;
   Evas_Object *frame = sd->base.main_frame;
   const Evas_Coord x = sd->zoom.start.x, y = sd->zoom.start.y;

   /* Stop zoom animator, we're done already. */
   if (sd->animator.zoom)
     {
        ecore_animator_del(sd->animator.zoom);
        sd->animator.zoom = NULL;
     }

   /* inform  ewk_view that we finished performing zoom animation */
   ewk_view_zoom_animated_mark_stop(view);
   evas_object_smart_callback_call(view, "zoom,interactive,end", NULL);

   /* if it was an auto zoom (zoom to fit some element) */
   if (timestamp - sd->zoom.last.timestamp < ZOOM_AUTO_TIMEOUT)
     {
        Ewk_Hit_Test *hit_test = ewk_frame_hit_test_new(frame, x, y);
        float zoom = _view_zoom_fits_hit(sd, hit_test);
        ewk_frame_hit_test_free(hit_test);

        if (zoom > 0.0)
          {
             /* wait until animation ends (see _view_zoom_animated_end) */
             sd->flags.animated_zoom = EINA_TRUE;
             ewk_view_zoom_animated_set
                (view, zoom, ZOOM_AUTO_ANIMATION_DURATION, x, y);
          }

        return EINA_TRUE;
     }

   /* otherwise, apply definitive zoom */
   ewk_view_zoom_set(view, sd->zoom.next.zoom, x, y);

   return EINA_TRUE;
}

/************** Event handlers *****************/

static Eina_Bool
_view_contextmenu_free(void *data)
{
   Evas_Object *notify = data;
   evas_object_del(notify);
   return EINA_FALSE;
}
#if 0
static void
_view_contextmenu_item_selected(void *data, Evas_Object *li, void *event_info __UNUSED__)
{
   Ewk_Context_Menu_Item *item = data;
   Evas_Object *view = evas_object_data_get(li, "view");
   Ewk_Context_Menu *menu = view_context_menu_get(view);

   ewk_context_menu_item_select(menu, item);
   ewk_context_menu_destroy(menu);
}

static void
_view_contextmenu_cancel(void *data, Evas_Object *notify __UNUSED__, void *event_info __UNUSED__)
{
   Ewk_Context_Menu *menu = data;
   ewk_context_menu_destroy(menu);
}

static void
on_view_contextmenu_show(void *data __UNUSED__, Evas_Object *view, void *event_info)
{
   Ewk_Context_Menu *menu = event_info;
   const Eina_List *l, *items = ewk_context_menu_item_list_get(menu);
   Ewk_Context_Menu_Item *item;
   Evas_Object *chrome = evas_object_data_get(view, "chrome");
   Evas_Object *notify, *li;
   Elm_List_Item *it;

   if (eina_list_count(items) == 0)
     {
        WRN("Context Menu with no items. Aborting operation.");
        ewk_context_menu_destroy(menu);
     }

   notify = elm_notify_add(chrome);
   elm_object_style_set(notify, "ewebkit");
   evas_object_size_hint_weight_set(notify, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_notify_repeat_events_set(notify, EINA_FALSE);
   elm_notify_orient_set(notify, ELM_NOTIFY_ORIENT_BOTTOM);

   li = elm_list_add(view);
   elm_object_style_set(li, "ewebkit");
   elm_list_always_select_mode_set(li, 1);
   evas_object_size_hint_weight_set(li, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set(li, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_min_set(li, 200, 200);
   elm_notify_content_set(notify, li);

   evas_object_smart_callback_add(notify, "block,clicked",
                                  _view_contextmenu_cancel, menu);
   evas_object_data_set(li, "view", view);
   EINA_LIST_FOREACH(items, l, item)
   {
      it = elm_list_item_append(li, ewk_context_menu_item_title_get(item), 0, 0,
                                _view_contextmenu_item_selected, item);
      elm_list_item_separator_set(it, ewk_context_menu_item_type_get(item) == EWK_SEPARATOR_TYPE);
   }
   view_context_menu_set(view, notify, menu);
   elm_list_go(li);
   evas_object_show(notify);
}

static void
on_view_contextmenu_new(void *data, Evas_Object *view __UNUSED__, void *event_info)
{
   DBG("menu=%p sd=%p", event_info, data);
   Ewk_Context_Menu *menu = event_info;
   ewk_context_menu_ref(menu);
}

static void
on_view_contextmenu_freed(void *data, Evas_Object *view, void *event_info)
{
   DBG("menu=%p view=%p", event_info, data);
   Ewk_Context_Menu *menu = event_info;
   Evas_Object *notify = view_context_menu_widget_get(view);

   view_context_menu_set(view, NULL, NULL);
   ewk_context_menu_unref(menu);
   ecore_idler_add(_view_contextmenu_free, notify);
}
#endif
/***********************************************************************
 * Smart Class Methods:                                                *
 *                                                                     *
 * Implements callback interface defined by Evas with Evas_Smart_Class *
 * and WebKit-EFL with Ewk_View_Smart_Class, a super-set of Evas.      *
 *                                                                     *
 * This allow us to inherit the class to override or extend, such as   *
 * creating a bigger smart data structure to account our animators and *
 * other context data and automatically apply fixed layout size when   *
 * object size changes.
 **********************************************************************/

/* object constructor. receives new object to set its internal state */
static void
_view_smart_add(Evas_Object *o)
{
   /* Create our extension of Ewk_View_Smart_Data so we can have our own
    * context, such as the animators and extra flags.
    */
   View_Smart_Data *sd = calloc(1, sizeof(View_Smart_Data));
   evas_object_smart_data_set(o, sd);
   sd->flags.first_calculate = EINA_TRUE;
   sd->flags.touch_interface = EINA_TRUE;

   /* call parent and let it do the whole thing */
   _parent_sc.sc.add(o);

   /* set some common properties, such as theme and default zoom weak method */
   ewk_view_theme_set(o, EWEBKIT_DATA_DIR "/themes/default.edj");
   ewk_view_zoom_weak_smooth_scale_set(o, EINA_FALSE);

   evas_object_smart_callback_add
      (o, "zoom,animated,end", _view_zoom_animated_end, sd);
   evas_object_smart_callback_add
      (o, "load,finished", _view_load_finished, sd);
   evas_object_smart_callback_add
      (o, "uri,changed", _view_uri_changed, sd);
#if 0
   evas_object_smart_callback_add
      (o, "contextmenu,new", on_view_contextmenu_new, sd);
   evas_object_smart_callback_add
      (o, "contextmenu,free", on_view_contextmenu_freed, sd);
   evas_object_smart_callback_add
      (o, "contextmenu,show", on_view_contextmenu_show, sd);
#endif
}

/* object destructor. receives object that dies when this function returns */
static void
_view_smart_del(Evas_Object *o)
{
   VIEW_SD_GET_OR_RETURN(o, sd);

   /* Must delete all extra fields before calling parent method,
    * as parent's will free View_Smart_Data.
    */
   if (sd->animator.pan)
      ecore_animator_del(sd->animator.pan);

   if (sd->animator.zoom)
      ecore_animator_del(sd->animator.zoom);

   if (sd->animator.kinetic)
      ecore_animator_del(sd->animator.kinetic);

   if (sd->idler_close_window)
      ecore_idler_del(sd->idler_close_window);

   _parent_sc.sc.del(o);
}

/* calculate is called on dirty objects (evas_object_smart_changed())
 * right before Evas paints the new scene.
 *
 * It is a good place to do last-minute calculations and checks,
 * allowing one to postpone such tasks as much as possible.
 *
 * For example, if user do for (i=0; i<1000; i++)
 * evas_object_resize(o, w, h), then Evas will just use the last value
 * when painting, however it will call Evas_Smart_Class::resize() lots
 * of times, as well as EVAS_CALLBACK_RESIZE. Thus if we apply
 * ewk_view_fixed_layout_size_set() on these callbacks, we'd apply 999
 * useless values! By postponing it to calculate(), we know we'll
 * apply just what matters.
 */
static void
_view_smart_calculate(Evas_Object *o)
{
   VIEW_SD_GET_OR_RETURN(o, sd);

#ifdef HACK_FLUSH_EVENTS_DURING_LONG_CALCULATE
   Display *dpy = ecore_x_display_get();
   double before = ecore_time_get();
   XSync(dpy, False); /* force processing all events */
#endif

   /* call parent smart calculate and let ewk_view do all its work */
   _parent_sc.sc.calculate(o);

   if (sd->flags.touch_interface)
     {
        Evas_Coord w, h;
        evas_object_geometry_get(o, NULL, NULL, &w, &h);
        ewk_view_fixed_layout_size_set(o, w, h);
     }

#ifdef HACK_FLUSH_EVENTS_DURING_LONG_CALCULATE
   double now = ecore_time_get();
   double elapsed = now - before;
   if (elapsed > LONG_CALCULATE_TIMEOUT)
     {
        WRN("calculate took too long (%0.3f of %0.3f), ignoring events.",
            elapsed, LONG_CALCULATE_TIMEOUT);
        XSync(dpy, True); /* throw away events received during this timeout */
     }

#endif
}

/* called by ewk_view when mouse down events happen.
 *
 * here we'll check if we should start a pan/scroll if user move mouse
 * (drag) or if we should forward this event to parent method, that
 * feeds it to WebKit-EFL.
 *
 * The way we do it here breaks drag&drop of elements required by some
 * JavaScript websites such as GMail and Yahoo! Mail. We should do it
 * in a way that WebKit says if element can be dragged or not, but it
 * is not exposed by WebKit now.
 */
static Eina_Bool
_view_smart_mouse_down(Ewk_View_Smart_Data *esd, const Evas_Event_Mouse_Down *ev)
{
   View_Smart_Data *sd = (View_Smart_Data *)esd;

   /* non-touch interface just forwards events */
   if (!sd->flags.touch_interface)
     goto forward_event;

   /* do not handle down when doing animated zoom */
   if (sd->flags.animated_zoom)
      return EINA_FALSE;

   /* mouse down immediately cancels previous kinetic animation */
   if (sd->animator.kinetic)
     {
        ecore_animator_del(sd->animator.kinetic);
        sd->animator.kinetic = NULL;
     }

   /* we just want to start pan on button 1 (left) and not triple click */
   if (ev->button != 1)
      goto forward_event;

   if (ev->flags & EVAS_BUTTON_TRIPLE_CLICK)
      goto forward_event;

#if 0 // at the end, this is not good usability.
      /* no pan or zoom if click over editable or links */
   Ewk_Hit_Test *hit_test = ewk_frame_hit_test_new
         (esd->main_frame, ev->canvas.x, ev->canvas.y);
   if (hit_test)
     {
        Eina_Bool skip = (hit_test->flags.editable ||
                          (hit_test->link.url && hit_test->link.url[0]));
        ewk_frame_hit_test_free(hit_test);
        if (skip && !(ev->flags & EVAS_BUTTON_DOUBLE_CLICK))
           goto forward_event;
     }

#endif

   /* cancel previous animators, if any (there should be none) */
   if (sd->animator.pan)
     {
        ecore_animator_del(sd->animator.pan);
        sd->animator.pan = NULL;
     }

   if (sd->animator.zoom)
     {
        ecore_animator_del(sd->animator.zoom);
        sd->animator.zoom = NULL;
     }

   /* choose if we're zooming or panning */
   if (ev->flags & EVAS_BUTTON_DOUBLE_CLICK)
      _view_zoom_start(sd, ev);
   else
      _view_pan_start(sd, ev);

   /* keep a copy in the case we notice a click instead of drag to pan/scroll */
   sd->mouse_down_copy = *ev;

   return EINA_TRUE;

forward_event:
#if 0
   if (ev->button == 3) // forward of context menu event is special
      return ewk_view_context_menu_forward_event(sd->base.self, ev);
#endif

   /* If we should forward/feed event using parent class method, then
    * just do it and do NOT create an animator. See _view_smart_mouse_up().
    */
   return _parent_sc.mouse_down(esd, ev);
}

/* called by ewk_view when mouse up events happen.
 *
 * This pairs with _view_smart_mouse_down(), forwarding events if
 * required, feeding saved mouse down event or even starting the
 * kinetic animation.
 */
static Eina_Bool
_view_smart_mouse_up(Ewk_View_Smart_Data *esd, const Evas_Event_Mouse_Up *ev)
{
   View_Smart_Data *sd = (View_Smart_Data *)esd;
   Eina_Bool used;

   /* non-touch interface just forwards events */
   if (!sd->flags.touch_interface)
     return _parent_sc.mouse_up(esd, ev);

   /* cancel any previous kinetic animation (but should have none) */
   if (sd->animator.kinetic)
     {
        ecore_animator_del(sd->animator.kinetic);
        sd->animator.kinetic = NULL;
     }

   /* do not handle up when doing animated zoom */
   if (sd->flags.animated_zoom)
      return EINA_FALSE;

   /* if it was not doing pan or zoom (see _view_smart_mouse_down()),
    * then we just feed mouse_up using parent method.
    */
   if (sd->animator.pan)
      used = _view_pan_stop(sd, ev);
   else if (sd->animator.zoom)
      used = _view_zoom_stop(sd, ev);
   else
      return _parent_sc.mouse_up(esd, ev);

   if (used)
      return EINA_TRUE;

   /* it was not used, so just feed the saved down then
    * mouse up event. This is required to let WebKit process the
    * sequence correctly.
    */
   _parent_sc.mouse_down(esd, &sd->mouse_down_copy);
   return _parent_sc.mouse_up(esd, ev);
}

/* called by ewk_view when mouse move events happen.
 *
 * Let's just ignore mouse move events while doing pan/scrolling or zooming.
 */
static Eina_Bool
_view_smart_mouse_move(Ewk_View_Smart_Data *esd, const Evas_Event_Mouse_Move *ev)
{
   View_Smart_Data *sd = (View_Smart_Data *)esd;

   if (sd->animator.pan)
     {
        /* account sample in circular array */
        struct point_history *p;
        unsigned int next_idx = (sd->pan.idx + 1) % PAN_HISTORY_SIZE;
        p = sd->pan.history + next_idx;
        p->x = ev->cur.canvas.x;
        p->y = ev->cur.canvas.y;
        p->timestamp = ecore_loop_time_get();

        sd->pan.idx = next_idx;
        sd->pan.count++;

        return EINA_FALSE;
     }

   if (sd->animator.zoom)
      return EINA_FALSE;

   return _parent_sc.mouse_move(esd, ev);
}

/* called by ewk_view when something goes to console
 *
 * We just print message to stdout
 */
static void
_view_smart_add_console_message(Ewk_View_Smart_Data *esd __UNUSED__, const char *message, unsigned int lineNumber, const char *sourceID)
{
   printf("BROWSER console: %s @%d: %s\n", sourceID, lineNumber, message);
}

enum dialog_type
{
   DIALOG_ALERT,
   DIALOG_CONFIRM,
   DIALOG_PROMPT
};

struct _dialog_data
{
   Evas_Object *notify;
   Evas_Object *bt_ok, *bt_cancel;
   Evas_Object *entry;

   Eina_Bool   *response;
};

static void
_bt_close(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   struct _dialog_data *d = data;

   *d->response = (obj == d->bt_ok);
   evas_object_hide(d->notify);
   ecore_main_loop_quit();
}

static Eina_Bool
_run_dialog(Evas_Object *parent, enum dialog_type type, const char *message, const char *default_entry_value, char **entry_value)
{
   EINA_SAFETY_ON_TRUE_RETURN_VAL((type != DIALOG_PROMPT) && (!!default_entry_value), EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL((type != DIALOG_PROMPT) && (!!entry_value), EINA_FALSE);

   struct _dialog_data *dialog_data = calloc(1, sizeof(*dialog_data));
   Eina_Bool response = EINA_FALSE;

   Evas_Object *bx_v, *lb;

   dialog_data->notify = elm_notify_add(parent);
   evas_object_size_hint_weight_set(dialog_data->notify, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_notify_repeat_events_set(dialog_data->notify, EINA_FALSE);

   bx_v = elm_box_add(parent);
   elm_notify_content_set(dialog_data->notify, bx_v);
   elm_box_horizontal_set(bx_v, 0);
   evas_object_size_hint_weight_set(bx_v, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bx_v);

   lb = elm_label_add(bx_v);
   elm_object_text_set(lb, message);
   elm_box_pack_end(bx_v, lb);
   evas_object_show(lb);

   dialog_data->response = &response;

   if (type == DIALOG_ALERT)
     {
        dialog_data->bt_ok = elm_button_add(bx_v);
        elm_object_text_set(dialog_data->bt_ok, "Close");
        elm_box_pack_end(bx_v, dialog_data->bt_ok);
        evas_object_size_hint_align_set(dialog_data->bt_ok, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_smart_callback_add(dialog_data->bt_ok, "clicked", _bt_close, dialog_data);
        evas_object_show(dialog_data->bt_ok);
     }
   else
     {
        if (type == DIALOG_PROMPT)
          {
             dialog_data->entry = elm_entry_add(bx_v);
             elm_entry_entry_set(dialog_data->entry, default_entry_value);
             elm_box_pack_end(bx_v, dialog_data->entry);
             evas_object_show(dialog_data->entry);
          }

        if (type == DIALOG_PROMPT || type == DIALOG_CONFIRM)
          {
             Evas_Object *bx_h = elm_box_add(bx_v);
             elm_box_horizontal_set(bx_h, 1);
             elm_box_pack_end(bx_v, bx_h);
             evas_object_size_hint_weight_set(bx_h, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
             evas_object_size_hint_align_set(bx_h, EVAS_HINT_FILL, EVAS_HINT_FILL);
             evas_object_show(bx_h);

             dialog_data->bt_cancel = elm_button_add(bx_h);
             elm_object_text_set(dialog_data->bt_cancel, "Cancel");
             elm_box_pack_end(bx_h, dialog_data->bt_cancel);
             evas_object_size_hint_weight_set(dialog_data->bt_cancel, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
             evas_object_size_hint_align_set(dialog_data->bt_cancel, EVAS_HINT_FILL, EVAS_HINT_FILL);
             evas_object_smart_callback_add(dialog_data->bt_cancel, "clicked", _bt_close, dialog_data);
             evas_object_show(dialog_data->bt_cancel);

             dialog_data->bt_ok = elm_button_add(bx_h);
             elm_object_text_set(dialog_data->bt_ok, "Ok");
             elm_box_pack_end(bx_h, dialog_data->bt_ok);
             evas_object_size_hint_weight_set(dialog_data->bt_ok, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
             evas_object_size_hint_align_set(dialog_data->bt_ok, EVAS_HINT_FILL, EVAS_HINT_FILL);
             evas_object_smart_callback_add(dialog_data->bt_ok, "clicked", _bt_close, dialog_data);
             evas_object_show(dialog_data->bt_ok);
          }
        else
           return EINA_FALSE;
     }

   evas_object_show(dialog_data->notify);
   ecore_main_loop_begin();

   if ((type == DIALOG_PROMPT) && (response == EINA_TRUE))
      *entry_value = strdup(elm_entry_entry_get(dialog_data->entry));

   evas_object_del(dialog_data->notify);
   free(dialog_data);

   return response;
}

/* called by ewk_view when javascript called alert()
 *
 */
static void
_view_smart_run_javascript_alert(Ewk_View_Smart_Data *esd, Evas_Object *frame __UNUSED__, const char *message)
{
   View_Smart_Data *sd = (View_Smart_Data *)esd;
   Evas_Object *view = sd->base.self;

   _run_dialog(view, DIALOG_ALERT, message, NULL, NULL);
}

/* called by ewk_view when javascript called confirm()
 *
 */
static Eina_Bool
_view_smart_run_javascript_confirm(Ewk_View_Smart_Data *esd, Evas_Object *frame __UNUSED__, const char *message)
{
   View_Smart_Data *sd = (View_Smart_Data *)esd;
   Evas_Object *view = sd->base.self;

   return _run_dialog(view, DIALOG_CONFIRM, message, NULL, NULL);
}

/* called by ewk_view when javascript called confirm()
 *
 */
static Eina_Bool
_view_smart_run_javascript_prompt(Ewk_View_Smart_Data *esd, Evas_Object *frame __UNUSED__, const char *message, const char *default_value, char **value)
{
   View_Smart_Data *sd = (View_Smart_Data *)esd;
   Evas_Object *view = sd->base.self;
   Eina_Bool confirm;

   confirm = _run_dialog(view, DIALOG_PROMPT, message, default_value, value);
   if (!confirm)
      *value = NULL;

   return EINA_TRUE;
}

/**
 * Called by webkit when a new window is requested to be created
 *
 * @param sd Smart Data of current view.
 *
 * @return a new view, owned by the window just created or @c NULL on error.
 */
static Evas_Object *
_view_smart_window_create(Ewk_View_Smart_Data *sd __UNUSED__, Eina_Bool javascript __UNUSED__, const Ewk_Window_Features *window_features __UNUSED__)
{
   return window_create();
}

static Eina_Bool
_window_close_delayed(void *data)
{
   View_Smart_Data *sd = data;
   sd->idler_close_window = NULL;
   Evas_Object *view = sd->base.self;
   Evas_Object *chrome = evas_object_data_get(view, "chrome");
   Browser_Window *win = evas_object_data_get(chrome, "win");

   tab_close_chrome(win, chrome);
   return EINA_FALSE;
}

/**
 * Called by webkit when a window is requested to be closed
 *
 * @param sd Smart Data of view to be closed.
 */
static void
_view_smart_window_close(Ewk_View_Smart_Data *esd)
{
   View_Smart_Data *sd = (View_Smart_Data *)esd;
   EINA_SAFETY_ON_TRUE_RETURN(!!sd->idler_close_window);
   sd->idler_close_window = ecore_idler_add(_window_close_delayed, sd);
}

/**
 * Creates a new view object given the parent.
 *
 * @param parent object to use as parent.
 *
 * @return newly added Evas_Object or @c NULL on errors.
 */
Evas_Object *
view_add(Evas_Object *parent, Backing_Store bs)
{
   static Evas_Smart *smart = NULL;
   Evas *canvas = evas_object_evas_get(parent);
   Evas_Object *view;

   if (!smart)
     {
        /* create ewk_view_single subclass, this is done only once! */
        static Ewk_View_Smart_Class api = EWK_VIEW_SMART_CLASS_INIT_NAME_VERSION("EWK_View_Demo");

        /* set current and parent apis to vanilla ewk_view_single methods */
        if (bs == BACKING_STORE_TILED)
          {
             ewk_view_tiled_smart_set(&api);
             ewk_view_tiled_smart_set(&_parent_sc);
          }
        else
          {
             ewk_view_single_smart_set(&api);
             ewk_view_single_smart_set(&_parent_sc);
          }

        /* override methods we want custom behavior */
        api.sc.add = _view_smart_add;
        api.sc.del = _view_smart_del;
        api.sc.calculate = _view_smart_calculate;
        api.mouse_down = _view_smart_mouse_down;
        api.mouse_up = _view_smart_mouse_up;
        api.mouse_move = _view_smart_mouse_move;
        api.add_console_message = _view_smart_add_console_message;
        api.window_create = _view_smart_window_create;
        api.window_close = _view_smart_window_close;
        api.run_javascript_alert = _view_smart_run_javascript_alert;
        api.run_javascript_confirm = _view_smart_run_javascript_confirm;
        api.run_javascript_prompt = _view_smart_run_javascript_prompt;

        /* create Evas_Smart class for this new smart object type. */
        smart = evas_smart_class_new(&api.sc);
        if (!smart)
          {
             CRITICAL("Could not create smart class");
             return NULL;
          }
     }

   view = evas_object_smart_add(canvas, smart);
   if (!view)
     {
        ERR("Could not create smart object object for view");
        return NULL;
     }

   return view;
}

void view_zoom_reset(Evas_Object *view)
{
   Evas_Coord w, h;
   VIEW_SD_GET_OR_RETURN(view, sd);
   if (sd->flags.animated_zoom || sd->animator.pan || sd->animator.zoom)
      return;

   if (sd->animator.kinetic)
     {
        ecore_animator_del(sd->animator.kinetic);
        sd->animator.kinetic = NULL;
     }

   sd->flags.animated_zoom = EINA_TRUE;
   ewk_frame_visible_content_geometry_get
      (sd->base.main_frame, EINA_FALSE, NULL, NULL, &w, &h);
   ewk_view_zoom_animated_set
      (view, 1.0, ZOOM_AUTO_ANIMATION_DURATION, w / 2, h / 2);
}

void view_zoom_next_up(Evas_Object *view)
{
   VIEW_SD_GET_OR_RETURN(view, sd);
   float zoom = ewk_frame_page_zoom_get(sd->base.main_frame);
   unsigned int idx = _view_zoom_closest_index_find(zoom);
   Evas_Coord w, h;
   if (sd->flags.animated_zoom || sd->animator.pan || sd->animator.zoom)
      return;

   if (idx + 1 >= ZOOM_STEPS_LAST)
      return;

   if (sd->animator.kinetic)
     {
        ecore_animator_del(sd->animator.kinetic);
        sd->animator.kinetic = NULL;
     }

   idx++;
   zoom = ZOOM_STEPS[idx];
   sd->flags.animated_zoom = EINA_TRUE;
   ewk_frame_visible_content_geometry_get
      (sd->base.main_frame, EINA_FALSE, NULL, NULL, &w, &h);
   ewk_view_zoom_animated_set
      (view, zoom, ZOOM_AUTO_ANIMATION_DURATION, w / 2, h / 2);
}

void view_zoom_next_down(Evas_Object *view)
{
   VIEW_SD_GET_OR_RETURN(view, sd);
   float zoom = ewk_frame_page_zoom_get(sd->base.main_frame);
   unsigned int idx = _view_zoom_closest_index_find(zoom);
   Evas_Coord w, h;
   if (sd->flags.animated_zoom || sd->animator.pan || sd->animator.zoom)
      return;

   if (idx == 0)
      return;

   if (sd->animator.kinetic)
     {
        ecore_animator_del(sd->animator.kinetic);
        sd->animator.kinetic = NULL;
     }

   idx--;
   zoom = ZOOM_STEPS[idx];
   sd->flags.animated_zoom = EINA_TRUE;
   ewk_frame_visible_content_geometry_get
      (sd->base.main_frame, EINA_FALSE, NULL, NULL, &w, &h);
   ewk_view_zoom_animated_set
      (view, zoom, ZOOM_AUTO_ANIMATION_DURATION, w / 2, h / 2);
}

#if 0
Eina_Bool view_context_menu_set(Evas_Object *view, Evas_Object *widget, Ewk_Context_Menu *menu)
{
   VIEW_SD_GET_OR_RETURN(view, sd, EINA_FALSE);
   if (widget && sd->context_menu)
     {
        CRITICAL("Trying to overwrite existing menu");
        return EINA_FALSE;
     }

   sd->context_menu = widget;
   if (menu)
      evas_object_data_set(view, "context-menu", menu);
   else
      evas_object_data_del(view, "context-menu");

   return EINA_TRUE;
}

Evas_Object *view_context_menu_widget_get(Evas_Object *view)
{
   VIEW_SD_GET_OR_RETURN(view, sd, NULL);
   return sd->context_menu;
}

Ewk_Context_Menu *view_context_menu_get(Evas_Object *view)
{
   return evas_object_data_get(view, "context-menu");
}
#endif

void view_touch_interface_set(Evas_Object *view, Eina_Bool setting)
{
   Evas_Event_Mouse_Up ev;

   VIEW_SD_GET_OR_RETURN(view, sd);
   setting = !!setting;
   if (sd->flags.touch_interface == setting) return;
   sd->flags.touch_interface = setting;

   if (setting)
     {
        Evas_Coord w, h;
        evas_object_geometry_get(view, NULL, NULL, &w, &h);
        ewk_view_fixed_layout_size_set(view, w, h);
     }
   else
     ewk_view_fixed_layout_size_set(view, 0, 0);

   if (setting) return; /* nothing else to do to enter touch mode */

   if (sd->animator.kinetic)
     {
        ecore_animator_del(sd->animator.kinetic);
        sd->animator.kinetic = NULL;
     }

   memset(&ev, 0, sizeof(ev));
   if (sd->animator.pan)
     {
        sd->pan.count = 0; /* avoid loops in _view_pan_stop() */
        _view_pan_stop(sd, &ev);
     }
   else if (sd->animator.zoom)
     _view_zoom_stop(sd, &ev);
}

Eina_Bool view_touch_interface_get(const Evas_Object *view)
{
   VIEW_SD_GET_OR_RETURN(view, sd, EINA_FALSE);
   return sd->flags.touch_interface;
}
