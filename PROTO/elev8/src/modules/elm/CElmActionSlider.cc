#include "elm.h"
#include "CElmActionSlider.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmActionSlider, labels);
GENERATE_PROPERTY_CALLBACKS(CElmActionSlider, slider);
GENERATE_PROPERTY_CALLBACKS(CElmActionSlider, magnet);

GENERATE_TEMPLATE(CElmActionSlider,
                  PROPERTY(labels),
                  PROPERTY(slider),
                  PROPERTY(magnet));

CElmActionSlider::CElmActionSlider(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_actionslider_add(parent->GetEvasObject()))
{
}

void CElmActionSlider::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("ActionSlider"),
               GetTemplate()->GetFunction());
}

void CElmActionSlider::Delete(Persistent<Value>, void *paramenter)
{
   delete static_cast<CElmActionSlider *>(paramenter);
}

/* there's 1 indicator label and 3 position labels */
void CElmActionSlider::labels_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;

   Local<Object> obj = val->ToObject();
   Local<Value> label;

   label = obj->Get(String::NewSymbol("left"));
   if (label->IsString())
     elm_object_part_text_set(eo, "left", *String::Utf8Value(label->ToString()));

   label = obj->Get(String::NewSymbol("center"));
   if (label->IsString())
     elm_object_part_text_set(eo, "center", *String::Utf8Value(label->ToString()));

   label = obj->Get(String::NewSymbol("right"));
   if (label->IsString())
     elm_object_part_text_set(eo, "right", *String::Utf8Value(label->ToString()));
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
   Elm_Actionslider_Pos pos;
   if (position_from_string(val, pos))
     elm_actionslider_indicator_pos_set(eo, pos);
}

Handle<Value> CElmActionSlider::slider_get() const
{
   return Integer::New(elm_actionslider_indicator_pos_get(eo));
}

void CElmActionSlider::magnet_set(Handle<Value> val)
{
   Elm_Actionslider_Pos pos;

   if (position_from_string(val, pos))
     elm_actionslider_magnet_pos_set(eo, pos);
}

Handle<Value> CElmActionSlider::magnet_get() const
{
   return Integer::New(elm_actionslider_magnet_pos_get(eo));
}

}
