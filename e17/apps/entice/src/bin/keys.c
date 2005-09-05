#include "keys.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Edje.h>

#define DEBUG 0
static Entice_Key_Keeper *keykeeper = NULL;

static void
_key_down_cb(void *data, Evas * e, Evas_Object * o, void *ev)
{
   if (ecore_event_current_type_get() == ECORE_X_EVENT_KEY_DOWN)
   {
      Ecore_X_Event_Key_Down *evx = NULL;

      if ((evx = (Ecore_X_Event_Key_Down *) ecore_event_current_event_get()))
      {
         Entice_Key *key = NULL;

#if DEBUG
         fprintf(stderr, "Key Down: %s\n", evx->keysymbol);
#endif
         if ((key = evas_hash_find(keykeeper->down, evx->keysymbol)))
            edje_object_signal_emit(keykeeper->obj, key->signal, "");
      }
   }
}

static void
_key_up_cb(void *data, Evas * e, Evas_Object * o, void *ev)
{
   if (ecore_event_current_type_get() == ECORE_X_EVENT_KEY_UP)
   {
      Ecore_X_Event_Key_Up *evx = NULL;

      if ((evx = (Ecore_X_Event_Key_Up *) ecore_event_current_event_get()))
      {
         Entice_Key *key = NULL;

#if DEBUG
         fprintf(stderr, "Key Up: %s\n", evx->keysymbol);
#endif
         if ((key =
              (Entice_Key *) evas_hash_find(keykeeper->up, evx->keysymbol)))
            edje_object_signal_emit(keykeeper->obj, key->signal, "");
      }
   }
}

void
entice_keys_init(void)
{
   if (!keykeeper)
   {
      keykeeper =
         (Entice_Key_Keeper *) malloc(sizeof(struct _Entice_Key_Keeper));
      memset(keykeeper, 0, sizeof(struct _Entice_Key_Keeper));
   }
}
void
entice_keys_callback_init(Evas_Object * edje)
{
   if (keykeeper)
   {
      keykeeper->obj = edje;
      evas_object_focus_set(keykeeper->obj, 1);
      evas_object_event_callback_add(keykeeper->obj, EVAS_CALLBACK_KEY_UP,
                                     _key_up_cb, NULL);
      evas_object_event_callback_add(keykeeper->obj, EVAS_CALLBACK_KEY_DOWN,
                                     _key_down_cb, NULL);
   }
}

void
entice_keys_free(void)
{
   if (keykeeper)
   {
      if (keykeeper->up)
         evas_hash_free(keykeeper->up);
      if (keykeeper->down)
         evas_hash_free(keykeeper->down);
      keykeeper->obj = NULL;
      free(keykeeper);
   }
}

void
entice_keys_down_add(Entice_Key * e)
{
   if (e && keykeeper)
      keykeeper->down = evas_hash_add(keykeeper->down, e->key, e);
}

void
entice_keys_up_add(Entice_Key * e)
{
   if (e && keykeeper)
      keykeeper->up = evas_hash_add(keykeeper->up, e->key, e);
}

Entice_Key *
entice_key_new(const char *symbol, const char *signal)
{
   Entice_Key *key = NULL;

   if ((key = (Entice_Key *) malloc(sizeof(Entice_Key))))
   {
      if (symbol)
         key->key = strdup(symbol);
      if (signal)
         key->signal = strdup(signal);
   }
   return (key);
}

void
entice_key_free(Entice_Key * key)
{
   if (key)
   {
      if (key->key)
         free(key->key);
      if (key->signal)
         free(key->signal);
      free(key);
   }
}
