#include "signals_thumb.h"
#include <limits.h>
#include <Esmart/E_Thumb.h>
#include "entice.h"

/** _entice_thumb_load_cb - when a EnticeThumbLoad signal is emitted
 * @data - the E_Thumb we want to load
 * @o - the evas object that emitted the signal
 * @emission - "EnticeThumbLoad"
 * @source - ""
 */
static void
_entice_thumb_load_cb(void *data, Evas_Object * o, const char *emission,
                      const char *source)
{
   if (data)
      entice_thumb_load_ethumb(data);
   else
      fprintf(stderr, "load request with null data\n");
}

/** _entice_thumb_preview_cb - when a EnticeThumbPreview signal is emitted
 * @data - the E_Thumb we want to preview
 * @o - the evas object that emitted the signal
 * @emission - "EnticeThumbPreview"
 * @source - ""
 */
static void
_entice_thumb_preview_cb(void *data, Evas_Object * o, const char *emission,
                         const char *source)
{
   /* 
      fprintf(stderr, "preview request\n"); */
   if (data)
      entice_preview_thumb(data);
}

/**
 * The following five functions translate evas mouse events into edje
 * mouse event signal emissions
 */
static void
_entice_thumb_mouse_wheel_translate(void *data, Evas * e, Evas_Object * obj,
                                    void *event_info)
{
   Evas_Event_Mouse_Wheel *ev = NULL;
   Evas_Object *o = NULL;

   if ((ev = (Evas_Event_Mouse_Wheel *) event_info))
   {
      if ((o = (Evas_Object *) data))
      {
         fprintf(stderr, "Mouse WHEEL: %d,%d\n", ev->direction, ev->z);
         /* 
            evas_event_feed_mouse_wheel_data(e, ev->direction, ev->z, o); */
      }
   }
}
static void
_entice_thumb_mouse_in_translate(void *data, Evas * e, Evas_Object * obj,
                                 void *event_info)
{
   Evas_Event_Mouse_In *ev = NULL;
   Evas_Object *o = NULL;

   if ((ev = (Evas_Event_Mouse_In *) event_info))
      if ((o = (Evas_Object *) data))
         edje_object_signal_emit(o, "mouse,in", "EnticeThumb");
}
static void
_entice_thumb_mouse_out_translate(void *data, Evas * e, Evas_Object * obj,
                                  void *event_info)
{
   Evas_Event_Mouse_Out *ev = NULL;
   Evas_Object *o = NULL;

   if ((ev = (Evas_Event_Mouse_Out *) event_info))
      if ((o = (Evas_Object *) data))
         edje_object_signal_emit(o, "mouse,out", "EnticeThumb");
}
static void
_entice_thumb_mouse_up_translate(void *data, Evas * e, Evas_Object * obj,
                                 void *event_info)
{
   Evas_Event_Mouse_Up *ev = NULL;
   Evas_Object *o = NULL;

   if ((ev = (Evas_Event_Mouse_Up *) event_info))
   {
      if ((o = (Evas_Object *) data))
      {
         char buf[PATH_MAX];

         snprintf(buf, PATH_MAX, "mouse,up,%i", (int) ev->button);
         edje_object_signal_emit(o, buf, "EnticeThumb");
      }
   }
}
static void
_entice_thumb_mouse_down_translate(void *data, Evas * e, Evas_Object * obj,
                                   void *event_info)
{
   Evas_Event_Mouse_Down *ev = NULL;
   Evas_Object *o = NULL;

   if ((ev = (Evas_Event_Mouse_Down *) event_info))
   {
      if ((o = (Evas_Object *) data))
      {
         char buf[PATH_MAX];

         snprintf(buf, PATH_MAX, "mouse,down,%i", (int) ev->button);
         edje_object_signal_emit(o, buf, "EnticeThumb");
      }
   }
}

/**
 * @o - the edje object we're hooking signals into
 * @im - the E_Thumb object want want to translate mouse events for
 * Evil things are done in order to get mouse events through to edje, it
 * works out nicel though.
 */
void
hookup_entice_thumb_signals(Evas_Object * o, Evas_Object * im)
{
   int i, count;
   E_Thumb *thumb = NULL;
   typedef struct
   {
      void (*func) (void *data, Evas_Object * o, const char *emission,
                    const char *source);
   } edje_callbacks;
   char *signals[] = { "EnticeThumbPreview", "EnticeThumbLoad" };
   edje_callbacks funcs[] = { _entice_thumb_preview_cb,
      _entice_thumb_load_cb
   };
   count = sizeof(signals) / sizeof(char *);
   for (i = 0; i < count; i++)
      edje_object_signal_callback_add(o, signals[i], "", funcs[i].func, im);

   /* 
    *  this is weird, but we intercept callbacks and feed them to edje, for
    *  some reason i can't get the mouse callbacks to be handled properly
    *  for a swallowed smart object w/o this
    */
   if ((thumb = (E_Thumb *) evas_object_smart_data_get(im)))
   {
      evas_object_event_callback_add(thumb->image, EVAS_CALLBACK_MOUSE_IN,
                                     _entice_thumb_mouse_in_translate, o);
      evas_object_event_callback_add(thumb->image, EVAS_CALLBACK_MOUSE_OUT,
                                     _entice_thumb_mouse_out_translate, o);
      evas_object_event_callback_add(thumb->image, EVAS_CALLBACK_MOUSE_UP,
                                     _entice_thumb_mouse_up_translate, o);
      evas_object_event_callback_add(thumb->image, EVAS_CALLBACK_MOUSE_DOWN,
                                     _entice_thumb_mouse_down_translate, o);
      evas_object_event_callback_add(thumb->image, EVAS_CALLBACK_MOUSE_WHEEL,
                                     _entice_thumb_mouse_wheel_translate, o);
   }
}
