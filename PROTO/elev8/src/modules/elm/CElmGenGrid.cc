#include "elm.h"
#include "CElmGenGrid.h"

namespace elm {

using namespace v8;
using namespace elm::gen;

GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, item_size_horizontal);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, item_size_vertical);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, highlight_mode);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, vertical_bounce);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, horizontal_bounce);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, select_mode);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, reorder_mode);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, multi_select);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, classes);
GENERATE_METHOD_CALLBACKS(CElmGenGrid, append);
GENERATE_METHOD_CALLBACKS(CElmGenGrid, clear);

GENERATE_TEMPLATE(CElmGenGrid,
                  PROPERTY(item_size_horizontal),
                  PROPERTY(item_size_vertical),
                  PROPERTY(highlight_mode),
                  PROPERTY(horizontal_bounce),
                  PROPERTY(vertical_bounce),
                  PROPERTY(select_mode),
                  PROPERTY(reorder_mode),
                  PROPERTY(multi_select),
                  PROPERTY(classes),
                  METHOD(append),
                  METHOD(clear));

CElmGenGrid::CElmGenGrid(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_gengrid_add(elm_object_top_widget_get(parent->GetEvasObject())))
{
   elm_gengrid_item_size_set(eo, 64, 64);
}

void CElmGenGrid::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Gengrid"), GetTemplate()->GetFunction());
}

Handle<Value> CElmGenGrid::clear(const Arguments&)
{
   /* FIXME: Properly dispose of resources */
   elm_gengrid_clear(eo);
   return Undefined();
}

Handle<Value> CElmGenGrid::append(const Arguments& args)
{
   if (!args[0]->IsString())
     return Undefined();

   Handle<Value> klass = cached.classes->Get(args[0]->ToString());
   if (klass.IsEmpty() || !klass->IsObject())
     return Undefined();

   ItemClass<CElmGenGrid> *item_class = static_cast<ItemClass<CElmGenGrid> *>(External::Unwrap(klass->ToObject()->GetHiddenValue(String::NewSymbol("gengrid::itemclass"))));
   Item<CElmGenGrid> *item = new Item<CElmGenGrid>(item_class, args[1], args[2]);

   elm_gengrid_item_append(eo, item_class->GetElmClass(), item, Item<CElmGenGrid>::OnSelect, item);
   return Undefined();
}

Handle<Value> CElmGenGrid::item_size_vertical_get() const
{
   Evas_Coord w;
   elm_gengrid_item_size_get(eo, &w, NULL);
   return Integer::New(w);
}

void CElmGenGrid::item_size_vertical_set(Handle<Value> value)
{
   if (value->IsNumber())
     {
        Evas_Coord h;
        elm_gengrid_item_size_get(eo, NULL, &h);
        elm_gengrid_item_size_set(eo, value->IntegerValue(), h);
     }
}

Handle<Value> CElmGenGrid::item_size_horizontal_get() const
{
   Evas_Coord h;
   elm_gengrid_item_size_get(eo, NULL, &h);
   return Integer::New(h);
}

void CElmGenGrid::item_size_horizontal_set(Handle<Value> value)
{
   if (value->IsNumber())
     {
        Evas_Coord w;
        elm_gengrid_item_size_get(eo, &w, NULL);
        elm_gengrid_item_size_set(eo, w, value->IntegerValue());
     }
}

Handle<Value> CElmGenGrid::multi_select_get() const
{
   return Boolean::New(elm_gengrid_multi_select_get(eo));
}

void CElmGenGrid::multi_select_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_gengrid_multi_select_set(eo, value->BooleanValue());
}

Handle<Value> CElmGenGrid::reorder_mode_get() const
{
   return Boolean::New(elm_gengrid_reorder_mode_get(eo));
}

void CElmGenGrid::reorder_mode_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_gengrid_reorder_mode_set(eo, value->BooleanValue());
}

Handle<Value> CElmGenGrid::select_mode_get() const
{
   return Number::New(elm_gengrid_select_mode_get(eo));
}

void CElmGenGrid::select_mode_set(Handle<Value> value)
{
   if (value->IsNumber())
     elm_gengrid_select_mode_set(eo, (Elm_Object_Select_Mode)value->ToNumber()->Value());
}

void CElmGenGrid::vertical_bounce_set(Handle<Value> val)
{
   Eina_Bool h;
   elm_gengrid_bounce_get(eo, &h, NULL);
   elm_gengrid_bounce_set(eo, h, val->IsBoolean() && val->BooleanValue());
}

Handle<Value> CElmGenGrid::vertical_bounce_get() const
{
   Eina_Bool v;
   elm_gengrid_bounce_get(eo, NULL, &v);
   return Boolean::New(v);
}

void CElmGenGrid::horizontal_bounce_set(Handle<Value> val)
{
   Eina_Bool v;
   elm_gengrid_bounce_get(eo, NULL, &v);
   elm_gengrid_bounce_set(eo, val->IsBoolean() && val->BooleanValue(), v);
}

Handle<Value> CElmGenGrid::horizontal_bounce_get() const
{
   Eina_Bool h;
   elm_gengrid_bounce_get(eo, &h, NULL);
   return Boolean::New(h);
}

Handle<Value> CElmGenGrid::highlight_mode_get() const
{
   return Boolean::New(elm_gengrid_highlight_mode_get(eo));
}

void CElmGenGrid::highlight_mode_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_gengrid_highlight_mode_set(eo, value->BooleanValue());
}

void CElmGenGrid::classes_set(Handle<Value> value)
{
   Local<Object> classes = value->ToObject();
   Local<Array> properties = classes->GetOwnPropertyNames();

   if (!cached.classes.IsEmpty())
     {
        for (unsigned int i = 0; i < properties->Length(); ++i)
          {
             Local<String> class_name = properties->Get(i)->ToString();
             Local<Object> class_desc = classes->Get(class_name)->ToObject();
             delete static_cast<ItemClass<CElmGenGrid> *>(External::Unwrap(class_desc->GetHiddenValue(String::NewSymbol("gengrid::itemclass"))));
          }
     }
   cached.classes.Dispose();

   cached.classes = Persistent<Object>::New(classes);
   for (unsigned int i = 0; i < properties->Length(); ++i)
     {
        Local<String> class_name = properties->Get(i)->ToString();
        Local<Object> class_desc = classes->Get(class_name)->ToObject();
        ItemClass<CElmGenGrid> *item_class = new ItemClass<CElmGenGrid>(this, class_name, class_desc);
        class_desc->SetHiddenValue(String::NewSymbol("gengrid::itemclass"), External::Wrap(item_class));
     }
}

Handle<Value> CElmGenGrid::classes_get() const
{
   return cached.classes;
}

}
