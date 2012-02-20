#include <X11/Xlib.h>
#include <Eina.h>
#include <Evas.h>
#include <Ecore_X.h>
#include "ekbd_private.h"
#include "Ekbd.h"
#include "ekbd_send.h"

// Disabled this because the updated keymap cause some troubles with x11
// Need to investigate how to fill the keymap with keysyms missing (Without
// bugs, doable ?).
//static Ecore_Event_Handler *_handler;
//static Eina_Bool _ekbd_send_update(void *data, int type __UNUSED__, void *event __UNUSED__);
//static void _ekbd_send_keysyms_get(Eina_List **keysyms, Eina_List *keys);

static const char *_glyph_to_keysym(int glyph);
static void _ekbd_send_x_string_press(const char *key, Ekbd_Mod mod);
static void _ekbd_send_x_keysym_press(const char *key, Ekbd_Mod mod);

EAPI void
ekbd_send_x_press(const char *str, Ekbd_Mod mod)
{
   if (str[0] == '"')
     _ekbd_send_x_string_press(str, mod);
   else
     _ekbd_send_x_keysym_press(str, mod);
}

static const char *
_glyph_to_keysym(int glyph)
{
   if (glyph < 0) return NULL;
   if (glyph > 0xff) glyph |= 0x1000000;
   return ecore_x_keysym_string_get(glyph);
}

static void
_ekbd_send_x_string_press(const char *str, Ekbd_Mod mod)
{
   char *string = (char*)str;
   char *key;
   int glyph = 0;

   /* First keysym is 'quotedbl' */
   string += evas_string_char_next_get(string, 0, &glyph);
   key = (char*) _glyph_to_keysym(glyph);

   while(key)
     {
        if(strcmp(key,"quotedbl"))
          _ekbd_send_x_keysym_press(key, mod);
        glyph = 0;
        /* utf8 -> glyph id (unicode - ucs4) */
        string += evas_string_char_next_get(string, 0, &glyph);
        key = (char*) _glyph_to_keysym(glyph);
     }
}

static void
_ekbd_send_x_keysym_press(const char *key, Ekbd_Mod mod)
{
   if (mod & EKBD_MOD_CTRL) ecore_x_test_fake_key_down("Control_L");
   if (mod & EKBD_MOD_ALT) ecore_x_test_fake_key_down("Alt_L");
   if (mod & EKBD_MOD_WIN) ecore_x_test_fake_key_down("Super_L");
   ecore_x_test_fake_key_press(key);
   if (mod & EKBD_MOD_WIN) ecore_x_test_fake_key_up("Super_L");
   if (mod & EKBD_MOD_ALT) ecore_x_test_fake_key_up("Alt_L");
   if (mod & EKBD_MOD_CTRL) ecore_x_test_fake_key_up("Control_L");
}

/*
static void
_ekbd_send_keysyms_get(Eina_List **keysyms, Eina_List *keys)
{
   Eina_List *l, *ll;
   Ekbd_Int_Key *key;
   Ekbd_Int_Key_State *st;
   EINA_LIST_FOREACH(keys, l, key)
     {
        EINA_LIST_FOREACH(key->states, ll, st)
          {
             if (st->tie)
               {
                  _ekbd_send_keysyms_get(keysyms, st->tie->keys);
               }
             if ((st->out)
                 && (st->out[0] != '"')
                 && (!eina_list_data_find(*keysyms, st->out)))
               {
                  *keysyms = eina_list_append(*keysyms, st->out);
               }
          }
     }
}


void
ekbd_send_x_init(Smart_Data *sd)
{
   _handler = ecore_event_handler_add(ECORE_X_EVENT_MAPPING_CHANGE,
                                      _ekbd_send_update, sd);
}

void
ekbd_send_x_shutdown()
{
   if (_handler)
     ecore_event_handler_del(_handler);
}

static Eina_Bool
_ekbd_send_update(void *data, int type __UNUSED__, void *event __UNUSED__)
{
   Smart_Data *sd;
   sd = data;
   if (!sd)
     {
        _handler = NULL;
        return ECORE_CALLBACK_DONE;
     }
   ekbd_send_update(sd);
   return ECORE_CALLBACK_RENEW;
}

void
ekbd_send_update(Smart_Data *sd)
{
   Ecore_X_Display *dpy;
   size_t  i;
   int min_keycode, max_keycode, keysyms_per_keycode;
   const char *s, *ss;
   Eina_List *nkl = NULL;
   KeySym *keymap, *nkm;
   Eina_List *unused = NULL, *used = NULL, *keysyms = NULL;
   Eina_List *l;
   Eina_Bool update = EINA_FALSE;

   _ekbd_send_keysyms_get(&keysyms, sd->layout.keys);

   dpy = ecore_x_display_get();
   XDisplayKeycodes(dpy, &min_keycode, &max_keycode);
   keymap = XGetKeyboardMapping(dpy, min_keycode,
                                    (max_keycode - min_keycode + 1),
                                    &keysyms_per_keycode);
   if (!keymap) return;
   nkm = keymap;
   for (i = (size_t)min_keycode; i <= (size_t)max_keycode; ++i)
     {
        Eina_Bool assigned = EINA_FALSE;
        int j, max;
        max = keysyms_per_keycode - 1;
        while ((max >= 0) && (keymap[max] == NoSymbol))
          max--;
        for (j = max; j >= 0 ; --j)
          {
             register KeySym ks = keymap[j];
             if (ks != NoSymbol)
               {
                  s = XKeysymToString(ks);
                  if (s)
                    {
                       const char *cs;
                       cs = eina_stringshare_add(s);
                       if (!eina_list_data_find(used, cs))
                         used = eina_list_append(used, cs);
                       else
                         eina_stringshare_del(cs);
                    }
               }
             else
               s = "NoSymbol";
             assigned = EINA_TRUE;
          }
        if (!assigned)
          unused = eina_list_append(unused, i);
        keymap += keysyms_per_keycode;
     }

   EINA_LIST_FREE(keysyms, ss)
     {
        if (!eina_list_data_find(used, ss))
          nkl = eina_list_append(nkl, ss);
     }
   if (nkl)
     {
        i = 0;
        printf("keysym missing: ");
        EINA_LIST_FREE(nkl, ss)
          {
             l = unused;
             if (l)
               {
                  s = XStringToKeysym(ss);
                  printf(" %s", ss);
                  if (s)
                    {
                       nkm[((uintptr_t)l->data - min_keycode) * keysyms_per_keycode] = s;
                       unused = eina_list_remove_list(unused, l);
                       if (!update) update = EINA_TRUE;
                    }
               }
          }
        printf("\n");
        if (update)
          {
             XChangeKeyboardMapping(dpy, min_keycode, keysyms_per_keycode,
                                    nkm, (max_keycode - min_keycode));
             ecore_x_flush();
          }
     }
   XFree(nkm);
   EINA_LIST_FREE(used, s)
     {
        eina_stringshare_del(s);
     }
   EINA_LIST_FREE(unused, i);
}
*/

