#include "signals_thumb.h"
#include <limits.h>
#include <Esmart/Esmart_Thumb.h>
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
 * @o - the edje object we're hooking signals into
 * @im - the E_Thumb object want want to translate mouse events for
 * Evil things are done in order to get mouse events through to edje, it
 * works out nicel though.
 */
void
hookup_entice_thumb_signals(Evas_Object * o, Evas_Object * im)
{
   int i, count;
   Evas_Object *image = NULL;
   char *signals[] = { "entice,thumb,preview", "entice,thumb,load" };
   void (*funcs[]) (void *data, Evas_Object * obj, const char *emission,
                    const char *source) =
   {
   _entice_thumb_preview_cb, _entice_thumb_load_cb};
   count = sizeof(signals) / sizeof(char *);
   for (i = 0; i < count; i++)
      edje_object_signal_callback_add(o, signals[i], "", funcs[i], im);
   if ((image = esmart_thumb_evas_object_image_get(im)))
      evas_object_pass_events_set(image, 1);
}
