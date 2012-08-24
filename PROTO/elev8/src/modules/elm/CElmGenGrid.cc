#include "elm.h"
#include "CElmGenGrid.h"

namespace elm {

using namespace v8;
using namespace elm::gen;

GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, item_size_horizontal);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, item_size_vertical);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, highlight_mode);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, select_mode);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, reorder_mode);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, multi_select);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, horizontal);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, filled);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, item_align);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, group_item_size);
GENERATE_RO_PROPERTY_CALLBACKS(CElmGenGrid, items_count);
GENERATE_RO_PROPERTY_CALLBACKS(CElmGenGrid, realized_items);
GENERATE_METHOD_CALLBACKS(CElmGenGrid, realized_items_update);
GENERATE_METHOD_CALLBACKS(CElmGenGrid, bring_in_item);

GENERATE_TEMPLATE_FULL(CElmObject, CElmGenGrid,
                  PROPERTY(item_size_horizontal),
                  PROPERTY(item_size_vertical),
                  PROPERTY(highlight_mode),
                  PROPERTY(select_mode),
                  PROPERTY(reorder_mode),
                  PROPERTY(multi_select),
                  PROPERTY(horizontal),
                  PROPERTY(filled),
                  PROPERTY(item_align),
                  PROPERTY(group_item_size),
                  PROPERTY_RO(items_count),
                  PROPERTY_RO(realized_items),
                  METHOD(realized_items_update),
                  METHOD(bring_in_item));

CElmGenGrid::CElmGenGrid(Local<Object> _jsObject, CElmObject *p)
   : CElmObject(_jsObject, elm_gengrid_add(elm_object_top_widget_get(p->GetEvasObject())))
{
   elm_gengrid_item_size_set(eo, 64, 64);
}

CElmGenGrid::~CElmGenGrid()
{
   align.Dispose();
}

void CElmGenGrid::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Gengrid"), GetTemplate()->GetFunction());
}

Handle<Value> CElmGenGrid::Pack(Handle<Value> value, Handle<Value> replace)
{
   Item<CElmGenGrid> *item = new Item<CElmGenGrid>(value, jsObject);
   if (!item)
     return Undefined();

   Local<Value> before = item->jsObject->Get(Item<CElmGenGrid>::str_before);

   if (before->IsUndefined() && replace->IsObject())
     before  = replace->ToObject()->Get(Item<CElmGenGrid>::str_before);

   if (before->IsString() || before->IsNumber())
     before = GetJSObject()->Get(String::NewSymbol("elements"))->ToObject()->Get(before);

    Item<CElmGenGrid> *before_item = Item<CElmGenGrid>::Unwrap(before);

   if (!before_item)
     item->object_item = elm_gengrid_item_append(eo, item->GetElmClass(), item,
                                                 Item<CElmGenGrid>::OnSelect, item);
   else
     item->object_item = elm_gengrid_item_insert_before(eo, item->GetElmClass(), item,
                                                        before_item->object_item,
                                                        Item<CElmGenGrid>::OnSelect, item);

   elm_object_item_data_set(item->object_item, item);
   return item->jsObject;
}

Handle<Value> CElmGenGrid::Unpack(Handle<Value> value)
{
   Item<CElmGenGrid> *item = Item<CElmGenGrid>::Unwrap(value);

   if (!item)
     return Undefined();

   Handle<Value> attrs = value->ToObject()->GetHiddenValue(Item<CElmGenGrid>::str_attrs);
   if (!attrs.IsEmpty())
     {
        Local<Object> obj = attrs->ToObject();
        if (obj->Get(Item<CElmGenGrid>::str_before)->IsUndefined())
          {
             Elm_Object_Item *before = elm_gengrid_item_next_get(item->object_item);
             if (before)
               {
                  Item<CElmGenGrid> *before_item = static_cast< Item<CElmGenGrid> *>
                     (elm_object_item_data_get(before));
                  obj->Set(Item<CElmGenGrid>::str_before, before_item->jsObject);
               }
          }
     }
   elm_object_item_del(item->object_item);
   return attrs;
}

void CElmGenGrid::UpdateItem(Handle<Value> value)
{
   Item<CElmGenGrid> *item = Item<CElmGenGrid>::Unwrap(value);
   if (item)
     elm_gengrid_item_item_class_update(item->object_item, item->GetElmClass());
}

Handle<Value> CElmGenGrid::item_size_vertical_get() const
{
   Evas_Coord w;
   elm_gengrid_item_size_get(eo, &w, NULL);
   return Integer::New(w);
}

Handle<Value> CElmGenGrid::realized_items_update(const Arguments&)
{
   elm_gengrid_realized_items_update(eo);
   return Undefined();
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

Handle<Value> CElmGenGrid::highlight_mode_get() const
{
   return Boolean::New(elm_gengrid_highlight_mode_get(eo));
}

void CElmGenGrid::highlight_mode_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_gengrid_highlight_mode_set(eo, value->BooleanValue());
}

Handle<Value> CElmGenGrid::horizontal_get() const
{
   return Boolean::New(elm_gengrid_horizontal_get(eo));
}

void CElmGenGrid::horizontal_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_gengrid_horizontal_set(eo, value->BooleanValue());
}

Handle<Value> CElmGenGrid::filled_get() const
{
   return Boolean::New(elm_gengrid_filled_get(eo));
}

void CElmGenGrid::filled_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_gengrid_filled_set(eo, value->BooleanValue());
}

Handle<Value> CElmGenGrid::item_align_get() const
{
   return align;
}

void CElmGenGrid::item_align_set(Handle<Value> val)
{
   if (!val->IsArray())
     return;

   Local<Object> sizes = val->ToObject();
   elm_gengrid_align_set(eo,
        sizes->Get(0)->ToNumber()->Value(),
        sizes->Get(1)->ToNumber()->Value());

   align.Dispose();
   align = Persistent<Value>::New(val);
}

Handle<Value> CElmGenGrid::group_item_size_get(void) const
{
   Local<Object> item_size = Object::New();
   Evas_Coord w, h;

   elm_gengrid_group_item_size_get(eo,  &w, &h);
   item_size->Set(String::NewSymbol("width"), Number::New(w));
   item_size->Set(String::NewSymbol("height"), Number::New(h));

   return item_size;
}

void CElmGenGrid::group_item_size_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;

   Local<Value> w = val->ToObject()->Get(String::NewSymbol("width"));
   Local<Value> h = val->ToObject()->Get(String::NewSymbol("height"));

   if (w->IsInt32() && h->IsInt32())
     elm_gengrid_group_item_size_set(eo, w->Int32Value(), h->Int32Value());
}

Handle<Value> CElmGenGrid::items_count_get() const
{
   return Number::New(elm_gengrid_items_count(eo));
}

Handle<Value> CElmGenGrid::realized_items_get() const
{
   Eina_List *l = elm_gengrid_realized_items_get(eo);
   Handle<Array> arr = Array::New(eina_list_count(l));

   void *d;
   int i = 0;
   EINA_LIST_FREE(l, d)
     {
        arr->Set(i, External::Wrap(d)); ++i;
     }

   return arr;
}

Handle<Value> CElmGenGrid::bring_in_item(const Arguments& args)
{
   Item<CElmGenGrid> *item = Item<CElmGenGrid>::Unwrap(args[0]);

   if (!item || !item->object_item)
     return Undefined();

   Elm_Gengrid_Item_Scrollto_Type scroll_type = ELM_GENGRID_ITEM_SCROLLTO_NONE;
   if (args[1]->IsString())
     {
        String::Utf8Value s(args[1]->ToString());

        if (!strcmp(*s, "none"))
          scroll_type = ELM_GENGRID_ITEM_SCROLLTO_NONE;
        else if (!strcmp(*s, "in"))
          scroll_type = ELM_GENGRID_ITEM_SCROLLTO_IN;
        else if (!strcmp(*s, "top"))
          scroll_type = ELM_GENGRID_ITEM_SCROLLTO_TOP;
        else if (!strcmp(*s, "middle"))
          scroll_type = ELM_GENGRID_ITEM_SCROLLTO_MIDDLE;
     }

   elm_gengrid_item_bring_in(item->object_item, scroll_type);

   return Undefined();
}

}
