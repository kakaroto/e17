#include <stdio.h>
#include <string.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <stdlib.h>
#include "EvasTextEntry.h"

#define DEBUG 0
static Evas_Smart *evas_text_entry_smart_get(void);
static void evas_text_entry_text_fix(Evas_Object * o);

Evas_Object *
evas_text_entry_new(Evas * e)
{
   Evas_Object *o = NULL;

   o = evas_object_smart_add(e, evas_text_entry_smart_get());
   return (o);
}

int
evas_text_entry_is_focused(Evas_Object * o)
{
   Evas_Text_Entry *e = NULL;

   if ((e = evas_object_smart_data_get(o)))
   {
      return (evas_object_focus_get(e->base));
   }
   return (0);
}

void
evas_text_entry_focus_set(Evas_Object * o, int val)
{
   Evas_Text_Entry *e = NULL;

   if ((e = evas_object_smart_data_get(o)))
   {
      evas_object_focus_set(e->base, val);
   }
}

char *
evas_text_entry_string_get(Evas_Object * o)
{
   char *result = NULL;
   Evas_Text_Entry *e = NULL;

   if ((e = evas_object_smart_data_get(o)))
   {
      if (e->buf.text)
         result = strdup(e->buf.text);
   }
   return (result);
}

void
evas_text_entry_is_password_set(Evas_Object * o, int val)
{
   Evas_Text_Entry *e = NULL;

   if ((e = evas_object_smart_data_get(o)))
   {
      e->passwd = val;
   }

}

void
evas_text_entry_edje_part_set(Evas_Object * o, Evas_Object * edje, char *part)
{
   Evas_Text_Entry *e = NULL;

   if ((e = evas_object_smart_data_get(o)))
   {
      e->edje.o = edje;
      if (e->edje.part)
         free(e->edje.part);
      e->edje.part = strdup(part);
   }
}

void
evas_text_entry_return_key_callback_set(Evas_Object * o,
                                        void (*func) (void *data,
                                                      const char *str),
                                        void *data)
{
   Evas_Text_Entry *e = NULL;

   e = evas_object_smart_data_get(o);
   e->return_key.func = func;
   e->return_key.arg = data;
}

void
evas_text_entry_max_chars_set(Evas_Object * o, int max)
{
   Evas_Text_Entry *e = NULL;

   e = evas_object_smart_data_get(o);
   if (max < PATH_MAX)
   {
      char buf[PATH_MAX];

      snprintf(buf, PATH_MAX, "%s", e->buf.text);
      free(e->buf.text);
      e->buf.size = max;
      e->buf.text = (char *) malloc((sizeof(char) * e->buf.size) + 1);
      memset(e->buf.text, 0, sizeof(e->buf.text));
      snprintf(e->buf.text, strlen(buf), "%s", buf);
   }
}
void
evas_text_entry_text_set(Evas_Object * o, const char *str)
{
   Evas_Text_Entry *e = NULL;

   if ((e = evas_object_smart_data_get(o)))
   {
      if (str)
         snprintf(e->buf.text, e->buf.size, "%s", str);
      else
      {
         memset(e->buf.text, 0, e->buf.size);
      }
      e->buf.index = strlen(e->buf.text);
      evas_text_entry_text_fix(o);
#if DEBUG
      fprintf(stderr, "Text Set (%d) %s\n", e->buf.index, e->buf.text);
#endif
   }
}

/*========================================================================*/
static void
evas_text_entry_buffer_char_append(Evas_Object * o, char c)
{
   Evas_Text_Entry *e = NULL;

   e = evas_object_smart_data_get(o);

   if (e->buf.index < e->buf.size)
      e->buf.text[e->buf.index++] = c;
   e->buf.text[e->buf.index] = 0;
   evas_text_entry_text_fix(o);
}
static void
evas_text_entry_buffer_backspace(Evas_Object * o)
{
   Evas_Text_Entry *e = NULL;

   e = evas_object_smart_data_get(o);

   if (e->buf.index > 0)
      e->buf.text[--e->buf.index] = 0;
   evas_text_entry_text_fix(o);
}
static void
evas_text_entry_text_fix(Evas_Object * o)
{
   Evas_Text_Entry *e = NULL;

   if (!o)
      return;
   if ((e = evas_object_smart_data_get(o)))
   {
      if (e->passwd)
      {
         int i, len;
         char buf[e->buf.size];

         len = strlen(e->buf.text);
         memset(buf, 0, e->buf.size);
         for (i = 0; i < len; i++)
            buf[i] = '*';
         edje_object_part_text_set(e->edje.o, e->edje.part, buf);
      }
      else
         edje_object_part_text_set(e->edje.o, e->edje.part, e->buf.text);
   }
}
static void
_key_down_cb(void *data, Evas * e, Evas_Object * o, void *ev)
{
   if (ecore_event_current_type_get() == ECORE_X_EVENT_KEY_DOWN)
   {
      Ecore_X_Event_Key_Down *evx = NULL;

      if ((evx = (Ecore_X_Event_Key_Down *) ecore_event_current_event_get()))
      {
         if ((!strcmp(evx->keyname, "Control_L"))
             || (!strcmp(evx->keyname, "Control_R"))
             || (!strcmp(evx->keyname, "Shift_R"))
             || (!strcmp(evx->keyname, "Shift_L"))
             || (!strcmp(evx->keyname, "Alt_R"))
             || (!strcmp(evx->keyname, "Alt_L")))
         {
            evas_key_modifier_on(e, evx->keyname);
         }
      }
   }
}
static void
_key_up_cb(void *data, Evas * e, Evas_Object * o, void *ev)
{
   Evas_Event_Key_Up *up = NULL;
   Evas_Text_Entry *entry = NULL;

   entry = evas_object_smart_data_get(data);
   up = (Evas_Event_Key_Up *) ev;

   if (ecore_event_current_type_get() == ECORE_X_EVENT_KEY_UP)
   {
      Ecore_X_Event_Key_Up *evx = NULL;

      if ((evx = (Ecore_X_Event_Key_Up *) ecore_event_current_event_get()))
      {
         if (evas_key_modifier_is_set_get(up->modifiers, evx->keyname))
            evas_key_modifier_off(e, evx->keyname);
         else if (evas_key_modifier_is_set_get(up->modifiers, "Control_L")
                  || evas_key_modifier_is_set_get(up->modifiers, "Control_R"))
         {
            switch ((int) evx->keyname[0])
            {
              case 117:
                 evas_text_entry_text_set(data, "");
                 break;
              default:
#if DEBUG
                 fprintf(stderr, "(%d) is the key value\n",
                         (int) evx->keyname[0]);
#endif
                 break;
            }

         }
         else if ((strlen(evx->keyname) > 1)
                  && (!evx->key_compose || (strlen(evx->key_compose) > 1)))
         {
            if (!strcmp(evx->keyname, "BackSpace"))
            {
               evas_text_entry_buffer_backspace(data);
            }
            else
            {
               fprintf(stderr, "Unknown string %s\n", evx->keyname);
            }
         }
         else
         {
            switch ((int) evx->key_compose[0])
            {
              case 127:        /* Delete */
              case 9:          /* Backspace */
                 break;
              case 8:          /* \t */
                 evas_text_entry_buffer_backspace(data);
                 break;
              case 13:         /* \r */
                 if (entry->return_key.func)
                 {
                    entry->return_key.func(entry->return_key.arg,
                                           entry->buf.text);
#if DEBUG
                    fprintf(stderr, "Buffer Length %d\n",
                            strlen(entry->buf.text));
#endif
                 }
                 break;
              default:
                 evas_text_entry_buffer_char_append(data,
                                                    evx->key_compose[0]);
#if DEBUG
                 fprintf(stderr, "(%d) is the key_compose value\n",
                         (int) evx->key_compose[0]);
#endif
                 break;
            }
         }
      }
   }
   evas_text_entry_text_fix(data);
}

static void
evas_text_entry_add(Evas_Object * o)
{
   Evas_Text_Entry *entry = NULL;

   entry = (Evas_Text_Entry *) malloc(sizeof(Evas_Text_Entry));
   memset(entry, 0, sizeof(Evas_Text_Entry));
   entry->buf.size = 128;
   entry->buf.text = (char *) malloc((sizeof(char) * 128) + 1);
   memset(entry->buf.text, 0, 128);

   evas_object_smart_data_set(o, entry);

   evas_key_modifier_add(evas_object_evas_get(o), "Control_L");
   evas_key_modifier_add(evas_object_evas_get(o), "Control_R");
   evas_key_modifier_add(evas_object_evas_get(o), "Shift_L");
   evas_key_modifier_add(evas_object_evas_get(o), "Shift_R");

   entry->clip = evas_object_rectangle_add(evas_object_evas_get(o));
   evas_object_layer_set(entry->clip, 0);

   entry->base = evas_object_rectangle_add(evas_object_evas_get(o));
   evas_object_layer_set(entry->base, 0);
   evas_object_color_set(entry->base, 255, 255, 255, 0);
   evas_object_clip_set(entry->base, entry->clip);
   evas_object_event_callback_add(entry->base, EVAS_CALLBACK_KEY_DOWN,
                                  _key_down_cb, o);
   evas_object_event_callback_add(entry->base, EVAS_CALLBACK_KEY_UP,
                                  _key_up_cb, o);

   evas_object_show(entry->base);
}
static void
evas_text_entry_del(Evas_Object * o)
{
   Evas_Text_Entry *e = NULL;

   e = evas_object_smart_data_get(o);
   evas_object_del(e->base);
   evas_object_del(e->clip);
   free(e->buf.text);
   free(e);
}
static void
evas_text_entry_layer_set(Evas_Object * o, int l)
{
   Evas_Text_Entry *e = NULL;

   e = evas_object_smart_data_get(o);
   evas_object_layer_set(e->base, l);
}
static void
evas_text_entry_raise(Evas_Object * o)
{
   Evas_Text_Entry *e = NULL;

   e = evas_object_smart_data_get(o);
   evas_object_raise(e->base);
}
static void
evas_text_entry_lower(Evas_Object * o)
{
   Evas_Text_Entry *e = NULL;

   e = evas_object_smart_data_get(o);
   evas_object_lower(e->base);
}
static void
evas_text_entry_stack_above(Evas_Object * o, Evas_Object * above)
{
   Evas_Text_Entry *e = NULL;

   e = evas_object_smart_data_get(o);
   evas_object_stack_above(e->base, above);

}
static void
evas_text_entry_stack_below(Evas_Object * o, Evas_Object * below)
{
   Evas_Text_Entry *e = NULL;

   e = evas_object_smart_data_get(o);
   evas_object_stack_below(e->base, below);
}
static void
evas_text_entry_move(Evas_Object * o, Evas_Coord x, Evas_Coord y)
{
   Evas_Text_Entry *e = NULL;

   e = evas_object_smart_data_get(o);
   evas_object_move(e->clip, x, y);
   evas_object_move(e->base, x, y);
}
static void
evas_text_entry_resize(Evas_Object * o, Evas_Coord w, Evas_Coord h)
{
   Evas_Text_Entry *e = NULL;

   e = evas_object_smart_data_get(o);
   evas_object_resize(e->clip, w, h);
   evas_object_resize(e->base, w, h);
}
static void
evas_text_entry_show(Evas_Object * o)
{
   Evas_Text_Entry *e = NULL;

   e = evas_object_smart_data_get(o);
   evas_object_show(e->clip);
}
static void
evas_text_entry_hide(Evas_Object * o)
{
   Evas_Text_Entry *e = NULL;

   e = evas_object_smart_data_get(o);
   evas_object_hide(e->clip);
}
static void
evas_text_entry_color_set(Evas_Object * o, int r, int g, int b, int a)
{
   Evas_Text_Entry *e = NULL;

   e = evas_object_smart_data_get(o);
}
static void
evas_text_entry_clip_set(Evas_Object * o, Evas_Object * clip)
{
   Evas_Text_Entry *e = NULL;

   e = evas_object_smart_data_get(o);
   evas_object_clip_set(e->clip, clip);
}
static void
evas_text_entry_clip_unset(Evas_Object * o)
{
   Evas_Text_Entry *e = NULL;

   e = evas_object_smart_data_get(o);
   evas_object_clip_unset(e->clip);
}
static Evas_Smart *
evas_text_entry_smart_get(void)
{
   Evas_Smart *s = NULL;

   s = evas_smart_new("Text Entry", evas_text_entry_add, evas_text_entry_del,
                      evas_text_entry_layer_set, evas_text_entry_raise,
                      evas_text_entry_lower, evas_text_entry_stack_above,
                      evas_text_entry_stack_below, evas_text_entry_move,
                      evas_text_entry_resize, evas_text_entry_show,
                      evas_text_entry_hide, evas_text_entry_color_set,
                      evas_text_entry_clip_set, evas_text_entry_clip_unset,
                      NULL);
   return (s);
}
