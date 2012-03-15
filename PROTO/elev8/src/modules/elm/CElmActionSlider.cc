#include <Eina.h>
#include <Evas.h>

#include "elev8_elm.h"
#include "CElmActionSlider.h"

using namespace v8;

CElmActionSlider::CElmActionSlider(CEvasObject *parent, Local<Object> obj)
   : CEvasObject()
   , prop_handler(property_list_base)
{
   eo = elm_actionslider_add(parent->get());
   construct(eo, obj);
}

/* there's 1 indicator label and 3 position labels */
void CElmActionSlider::labels_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;

   Local<Object> obj = val->ToObject();
   Local<Value> v[3];
   Local<String> str[3];
   const char *name[3] = { "left", "center", "right" };

   for (int i = 0; i < 3; i++)
     {
        v[i] = obj->Get(String::New(name[i]));
        if (v[i]->IsString())
          str[i] = v[i]->ToString();
     }

   String::Utf8Value left(str[0]), middle(str[1]), right(str[2]);
   elm_object_part_text_set(eo, name[0], *left);
   elm_object_part_text_set(eo,name[1], *middle);
   elm_object_part_text_set(eo,name[2], *right);
}

Handle<Value> CElmActionSlider::labels_get() const
{
   // FIXME: implement
   return Undefined();
}

bool CElmActionSlider::position_from_string(Handle<Value> val, Elm_Actionslider_Pos &pos)
{
   if (!val->IsString())
     return false;

   String::Utf8Value str(val);
   if (!strcmp(*str, "left"))
     pos = ELM_ACTIONSLIDER_LEFT;
   else if (!strcmp(*str, "center"))
     pos = ELM_ACTIONSLIDER_CENTER;
   else if (!strcmp(*str, "right"))
     pos = ELM_ACTIONSLIDER_RIGHT;
   else
     {
        ELM_ERR( "Invalid actionslider position: %s", *str);
        return false;
     }
   return true;
}

void CElmActionSlider::slider_set(Handle<Value> val)
{
   Elm_Actionslider_Pos pos = ELM_ACTIONSLIDER_NONE;
   if (position_from_string(val, pos))
     elm_actionslider_indicator_pos_set(eo, pos);
}

Handle<Value> CElmActionSlider::slider_get() const
{
   return Integer::New(elm_actionslider_indicator_pos_get(eo));
}

void CElmActionSlider::magnet_set(Handle<Value> val)
{
   Elm_Actionslider_Pos pos = ELM_ACTIONSLIDER_NONE;

   if (position_from_string(val, pos))
     elm_actionslider_magnet_pos_set(eo, pos);
}

Handle<Value> CElmActionSlider::magnet_get() const
{
   return Integer::New(elm_actionslider_magnet_pos_get(eo));
}

PROPERTIES_OF(CElmActionSlider) =
  {
     PROP_HANDLER(CElmActionSlider, magnet),
     PROP_HANDLER(CElmActionSlider, slider),
     PROP_HANDLER(CElmActionSlider, labels),
     { NULL }
  };
