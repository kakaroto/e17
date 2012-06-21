#include "elm.h"
#include "CElmActionSlider.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmActionSlider, labels);
GENERATE_PROPERTY_CALLBACKS(CElmActionSlider, slider);
GENERATE_PROPERTY_CALLBACKS(CElmActionSlider, magnet);
GENERATE_PROPERTY_CALLBACKS(CElmActionSlider, on_select);
GENERATE_PROPERTY_CALLBACKS(CElmActionSlider, enabled);

GENERATE_TEMPLATE(CElmActionSlider,
                  PROPERTY(labels),
                  PROPERTY(slider),
                  PROPERTY(magnet),
                  PROPERTY(on_select),
                  PROPERTY(enabled));

CElmActionSlider::CElmActionSlider(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_actionslider_add(parent->GetEvasObject()))
{
}

CElmActionSlider::~CElmActionSlider()
{
   on_select_set(Undefined());
}

void CElmActionSlider::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("ActionSlider"),
               GetTemplate()->GetFunction());
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

void CElmActionSlider::OnSelect(void *event_info)
{
   Handle<Function> callback(Function::Cast(*cb.select));
   char *label = (char *)event_info;
   Handle<Value> args[2] = { jsObject, Undefined()};

   if (label)
     args[1] = String::New(label);

   callback->Call(jsObject, 2, args);
}

void CElmActionSlider::OnSelectWrapper(void *data, Evas_Object *, void *event_info)
{
   static_cast<CElmActionSlider*>(data)->OnSelect(event_info);
}

Handle<Value> CElmActionSlider::on_select_get() const
{
   return cb.select;
}

void CElmActionSlider::on_select_set(Handle<Value> val)
{
   if (!cb.select.IsEmpty())
     {
        evas_object_smart_callback_del(eo, "selected", &OnSelectWrapper);
        cb.select.Dispose();
        cb.select.Clear();
     }

   if (!val->IsFunction())
     return;

   cb.select = Persistent<Value>::New(val);
   evas_object_smart_callback_add(eo, "selected", &OnSelectWrapper, this);
}

void CElmActionSlider::enabled_set(Handle<Value> val)
{
   Elm_Actionslider_Pos pos;

   if (position_from_string(val, pos))
     elm_actionslider_enabled_pos_set(eo, pos);
}

Handle<Value> CElmActionSlider::enabled_get() const
{
   return Integer::New(elm_actionslider_enabled_pos_get(eo));
}

}
