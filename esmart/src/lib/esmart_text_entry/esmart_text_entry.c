/*
 * vim:ts=8:sw=3
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include <Ecore.h>
#include <Edje.h>

#include "Esmart_Text_Entry.h"

struct _Esmart_Text_Entry
{
  Evas_Object *clip;
  Evas_Object *base;
  struct
  {
    int size;
    char *text;
    int index;
  } buf;
  struct
  {
    Evas_Object *o;
    char *part;
  } edje;
  struct
  {
    void (*func) (void *data, const char *str);
    void *arg;
  } return_key;
  int passwd;
};
typedef struct _Esmart_Text_Entry Esmart_Text_Entry;

#define DEBUG 0
static Evas_Smart *esmart_text_entry_smart_get (void);
static void esmart_text_entry_text_fix (Evas_Object * o);

EAPI Evas_Object *
esmart_text_entry_new (Evas * e)
{
  Evas_Object *o = NULL;

  o = evas_object_smart_add (e, esmart_text_entry_smart_get ());
  return (o);
}

EAPI const char *
esmart_text_entry_text_get (Evas_Object * o)
{
  Esmart_Text_Entry *e = NULL;

  if ((e = evas_object_smart_data_get (o)))
    {
       return e->buf.text;
    }
  return (NULL);
}

EAPI void
esmart_text_entry_is_password_set (Evas_Object * o, int val)
{
  Esmart_Text_Entry *e = NULL;

  if ((e = evas_object_smart_data_get (o)))
    {
      e->passwd = val;
    }

}

EAPI void
esmart_text_entry_edje_part_set (Evas_Object * o, Evas_Object * edje,
				 const char *part)
{
  Esmart_Text_Entry *e = NULL;

  if ((e = evas_object_smart_data_get (o)))
    {
      e->edje.o = edje;
      if (e->edje.part)
	free (e->edje.part);
      e->edje.part = strdup (part);
    }
}

EAPI Evas_Object *
esmart_text_entry_edje_object_get(Evas_Object * o)
{
   Esmart_Text_Entry *e = NULL;

   if ((e = evas_object_smart_data_get (o)))
     {
	return e->edje.o;
     }
   return NULL;
}

EAPI const char *
esmart_text_entry_edje_part_get(Evas_Object * o)
{
   Esmart_Text_Entry *e = NULL;

   if ((e = evas_object_smart_data_get (o)))
     {
	return e->edje.part;
     }
   return NULL;
}

EAPI void
esmart_text_entry_return_key_callback_set (Evas_Object * o,
					   void (*func) (void *data,
							 const char *str),
					   void *data)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  e->return_key.func = func;
  e->return_key.arg = data;
}

EAPI void
esmart_text_entry_max_chars_set (Evas_Object * o, int max)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  if (max < PATH_MAX)
    {
      char buf[PATH_MAX];

      snprintf (buf, PATH_MAX, "%s", e->buf.text);
      free (e->buf.text);
      e->buf.size = max;
      e->buf.text = (char *) malloc ((sizeof (char) * e->buf.size) + 1);
      memset (e->buf.text, 0, sizeof (e->buf.text));
      snprintf (e->buf.text, strlen (buf), "%s", buf);
    }
}
EAPI void
esmart_text_entry_text_set (Evas_Object * o, const char *str)
{
  Esmart_Text_Entry *e = NULL;

  if ((e = evas_object_smart_data_get (o)))
    {
      if (str)
	snprintf (e->buf.text, e->buf.size, "%s", str);
      else
	{
	  memset (e->buf.text, 0, e->buf.size);
	}
      e->buf.index = strlen (e->buf.text);
      esmart_text_entry_text_fix (o);
#if DEBUG
      fprintf (stderr, "Text Set (%d) %s\n", e->buf.index, e->buf.text);
#endif
    }
}

/*========================================================================*/
static void
esmart_text_entry_buffer_char_append (Evas_Object * o, char c)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);

  if (e->buf.index < e->buf.size)
    e->buf.text[e->buf.index++] = c;
  e->buf.text[e->buf.index] = 0;
  esmart_text_entry_text_fix (o);
}
static void
esmart_text_entry_buffer_backspace (Evas_Object * o)
{
  Esmart_Text_Entry *e = NULL;
  int len, val, pos;
   
  e = evas_object_smart_data_get (o);
  len = strlen(e->buf.text);

  if (len > 0)
  {
    pos = evas_string_char_prev_get(e->buf.text, len, &val);
    e->buf.text[pos] = 0; 
  }
  
  esmart_text_entry_text_fix (o);
}
static void
esmart_text_entry_text_fix (Evas_Object * o)
{
  Esmart_Text_Entry *e = NULL;

  if (!o)
    return;
  if ((e = evas_object_smart_data_get (o)))
    {
      if (e->passwd)
	{
	  int i, len;
	  char buf[e->buf.size];

	  len = strlen (e->buf.text);
	  memset (buf, 0, e->buf.size);
	  for (i = 0; i < len; i++)
	    buf[i] = '*';
	  edje_object_part_text_set (e->edje.o, e->edje.part, buf);
	}
      else
	edje_object_part_text_set (e->edje.o, e->edje.part, e->buf.text);
    }
}
static void
_key_down_cb (void *data, Evas * e, Evas_Object * o, void *ev)
{
  Evas_Event_Key_Down *down = NULL;
  Esmart_Text_Entry *entry = NULL;

  down = ev;
  entry = evas_object_smart_data_get(data);

  /* handle modifiers */
  if (evas_key_modifier_is_set(down->modifiers, "Control"))
  {
    if (strcmp(down->keyname, "a") == 0)
      esmart_text_entry_text_set(data, "");
  }
  else if (strcmp(down->keyname, "BackSpace") == 0)
  {
    esmart_text_entry_buffer_backspace(data);
  }
  else if (strcmp(down->keyname, "Return") == 0 ||
           strcmp(down->keyname, "KP_Enter") == 0)
  {
    if (entry->return_key.func)
      entry->return_key.func(entry->return_key.arg, entry->buf.text);
  }
  else if (down->string && *down->string && (strlen(down->string) != 1 || down->string[0] >= 0x20))
  {
     int i;
     for(i = 0; i < strlen(down->string); i++)
       esmart_text_entry_buffer_char_append(data, down->string[i]);
  }
  esmart_text_entry_text_fix(data);
}

static void
esmart_text_entry_add (Evas_Object * o)
{
  Esmart_Text_Entry *entry = NULL;

  entry = (Esmart_Text_Entry *) malloc (sizeof (Esmart_Text_Entry));
  memset (entry, 0, sizeof (Esmart_Text_Entry));
  entry->buf.size = 128;
  entry->buf.text = (char *) malloc ((sizeof (char) * 128) + 1);
  memset (entry->buf.text, 0, 128);

  evas_object_smart_data_set (o, entry);

  evas_key_modifier_add (evas_object_evas_get (o), "Control");
  evas_key_modifier_add (evas_object_evas_get (o), "Shift");
  evas_key_modifier_add (evas_object_evas_get (o), "Alt");

  entry->clip = evas_object_rectangle_add (evas_object_evas_get (o));
  evas_object_layer_set (entry->clip, 0);

  entry->base = evas_object_rectangle_add (evas_object_evas_get (o));
  evas_object_layer_set (entry->base, 0);
  evas_object_color_set (entry->base, 255, 255, 255, 0);
  evas_object_clip_set (entry->base, entry->clip);
  evas_object_event_callback_add (o, EVAS_CALLBACK_KEY_DOWN,
				  _key_down_cb, o);

  evas_object_show (entry->base);
}
static void
esmart_text_entry_del (Evas_Object * o)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  evas_object_del (e->base);
  evas_object_del (e->clip);
  free (e->buf.text);
  free (e);
}
static void
esmart_text_entry_move (Evas_Object * o, Evas_Coord x, Evas_Coord y)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  evas_object_move (e->clip, x, y);
  evas_object_move (e->base, x, y);
}
static void
esmart_text_entry_resize (Evas_Object * o, Evas_Coord w, Evas_Coord h)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  evas_object_resize (e->clip, w, h);
  evas_object_resize (e->base, w, h);
}
static void
esmart_text_entry_show (Evas_Object * o)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  evas_object_show (e->clip);
}
static void
esmart_text_entry_hide (Evas_Object * o)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  evas_object_hide (e->clip);
}
static void
esmart_text_entry_color_set (Evas_Object * o, int r, int g, int b, int a)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
}
static void
esmart_text_entry_clip_set (Evas_Object * o, Evas_Object * clip)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  evas_object_clip_set (e->clip, clip);
}
static void
esmart_text_entry_clip_unset (Evas_Object * o)
{
  Esmart_Text_Entry *e = NULL;

  e = evas_object_smart_data_get (o);
  evas_object_clip_unset (e->clip);
}
static Evas_Smart *
esmart_text_entry_smart_get (void)
{
  static Evas_Smart *s = NULL;
  static const Evas_Smart_Class sc = {
      "Text Entry",
      EVAS_SMART_CLASS_VERSION,
      esmart_text_entry_add,
      esmart_text_entry_del,
      esmart_text_entry_move,
      esmart_text_entry_resize,
      esmart_text_entry_show,
      esmart_text_entry_hide,
      esmart_text_entry_color_set,
      esmart_text_entry_clip_set,
      esmart_text_entry_clip_unset,
      NULL,
      NULL,
      NULL
  };

  if (!s)
    s = evas_smart_class_new(&sc);

  return s;
}
