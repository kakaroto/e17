#include "Esmart_Textarea.h"
#include "esmart_textarea_private.h"

/* Evas Smart Object API Wrapping methods */

static Evas_Smart *smart;

Evas_Smart *
esmart_textarea_smart_get()
{
   if (smart) return smart;
   smart = evas_smart_new("text",
			  esmart_textarea_smart_add,
			  esmart_textarea_smart_del,
			  esmart_textarea_smart_layer_set,
			  esmart_textarea_smart_raise,
			  esmart_textarea_smart_lower,
			  esmart_textarea_smart_stack_above,
			  esmart_textarea_smart_stack_below,
			  esmart_textarea_smart_move,
			  esmart_textarea_smart_resize,
			  esmart_textarea_smart_show,
			  esmart_textarea_smart_hide,
			  esmart_textarea_smart_color_set,
			  esmart_textarea_smart_clip_set,
			  esmart_textarea_smart_clip_unset,
			  NULL);
   return smart;
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
esmart_textarea_smart_layer_set(Evas_Object *o, int l)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   evas_object_layer_set(t->text, l);
   evas_object_stack_below(t->bg, t->text);
   evas_object_stack_above(t->cursor, t->text);
}

void
esmart_textarea_smart_raise(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   evas_object_raise(t->text);
   evas_object_raise(t->bg);
   evas_object_raise(t->cursor);
}

void 
esmart_textarea_smart_lower(Evas_Object *o)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   evas_object_lower(t->text);
   evas_object_lower(t->bg);
   evas_object_lower(t->cursor);
}

void 
esmart_textarea_smart_stack_above(Evas_Object *o, Evas_Object *above)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   evas_object_stack_above(t->text, above);
   evas_object_stack_above(t->bg, above);
   evas_object_stack_above(t->cursor, above);
}

void
esmart_textarea_smart_stack_below(Evas_Object *o, Evas_Object *below)
{
   Esmart_Text_Area *t;
   
   t = evas_object_smart_data_get(o);
   evas_object_stack_below(t->text, below);
   evas_object_stack_below(t->bg, below);
   evas_object_stack_below(t->cursor, below);   
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
