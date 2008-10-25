#include "Esmart_Textarea.h"
#include "esmart_textarea_private.h"

/* Evas Smart Object API Wrapping methods */

Evas_Smart *
esmart_textarea_smart_get()
{
   static Evas_Smart *s = NULL;
   static const Evas_Smart_Class sc = {
      "Text Area",
      EVAS_SMART_CLASS_VERSION,
      esmart_textarea_add,
      esmart_textarea_del,
      esmart_textarea_move,
      esmart_textarea_resize,
      esmart_textarea_show,
      esmart_textarea_hide,
      esmart_textarea_color_set,
      esmart_textarea_clip_set,
      esmart_textarea_clip_unset,
      NULL,
      NULL,
      NULL
  };

  if (!s)
    s = evas_smart_class_new(&sc);

  return s;
}

void
esmart_textarea_smart_add(Evas_Object *o)
{
   Esmart_Text_Area *data;
   
   data = _esmart_textarea_init(o);
   if (!data) return;
   evas_object_smart_data_set(o, data);
}

void
esmart_textarea_smart_del(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   evas_object_del(t->text);
   evas_object_del(t->bg);
   evas_object_del(t->cursor);
}
 
void
esmart_textarea_smart_move(Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);   
   evas_object_move(t->text, x, y);
   evas_object_move(t->bg, x, y);
   _esmart_textarea_cursor_goto_cursor(t);
}

void
esmart_textarea_smart_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   evas_object_resize(t->text, w, h);
   evas_object_resize(t->bg, w, h);
   _esmart_textarea_cursor_goto_cursor(t);   
}

void
esmart_textarea_smart_show(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   evas_object_show(t->text);
   evas_object_show(t->bg);
   evas_object_show(t->cursor);
}

void
esmart_textarea_smart_hide(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   evas_object_hide(t->text);
   evas_object_hide(t->bg);
   evas_object_hide(t->cursor);   
}

void
esmart_textarea_smart_color_set(Evas_Object *o, int r, int g, int b, int a)
{
}

void 
esmart_textarea_smart_clip_set(Evas_Object *o, Evas_Object *clip)
{
}

void
esmart_textarea_smart_clip_unset(Evas_Object *o)
{
}
