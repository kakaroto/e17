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
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, horizontal);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, filled);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, page_relative);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, item_align);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, group_item_size);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, page_size);
GENERATE_PROPERTY_CALLBACKS(CElmGenGrid, scroller_policy);
GENERATE_RO_PROPERTY_CALLBACKS(CElmGenGrid, items_count);
GENERATE_RO_PROPERTY_CALLBACKS(CElmGenGrid, vertical_current_page);
GENERATE_RO_PROPERTY_CALLBACKS(CElmGenGrid, horizontal_current_page);
GENERATE_RO_PROPERTY_CALLBACKS(CElmGenGrid, vertical_last_page);
GENERATE_RO_PROPERTY_CALLBACKS(CElmGenGrid, horizontal_last_page);
GENERATE_RO_PROPERTY_CALLBACKS(CElmGenGrid, realized_items);
GENERATE_METHOD_CALLBACKS(CElmGenGrid, append);
GENERATE_METHOD_CALLBACKS(CElmGenGrid, clear);
GENERATE_METHOD_CALLBACKS(CElmGenGrid, delete_item);
GENERATE_METHOD_CALLBACKS(CElmGenGrid, update_item);
GENERATE_METHOD_CALLBACKS(CElmGenGrid, page_show);
GENERATE_METHOD_CALLBACKS(CElmGenGrid, page_bring);
GENERATE_METHOD_CALLBACKS(CElmGenGrid, realized_items_update);

GENERATE_TEMPLATE_FULL(CElmObject, CElmGenGrid,
                  PROPERTY(item_size_horizontal),
                  PROPERTY(item_size_vertical),
                  PROPERTY(highlight_mode),
                  PROPERTY(horizontal_bounce),
                  PROPERTY(vertical_bounce),
                  PROPERTY(select_mode),
                  PROPERTY(reorder_mode),
                  PROPERTY(multi_select),
                  PROPERTY(horizontal),
                  PROPERTY(filled),
                  PROPERTY(page_relative),
                  PROPERTY(item_align),
                  PROPERTY(group_item_size),
                  PROPERTY(page_size),
                  PROPERTY(scroller_policy),
                  PROPERTY_RO(items_count),
                  PROPERTY_RO(horizontal_current_page),
                  PROPERTY_RO(vertical_current_page),
                  PROPERTY_RO(horizontal_last_page),
                  PROPERTY_RO(vertical_last_page),
                  PROPERTY_RO(realized_items),
                  METHOD(append),
                  METHOD(clear),
                  METHOD(delete_item),
                  METHOD(update_item),
                  METHOD(page_show),
                  METHOD(page_bring),
                  METHOD(realized_items_update));

CElmGenGrid::CElmGenGrid(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_gengrid_add(elm_object_top_widget_get(parent->GetEvasObject())))
{
   elm_gengrid_item_size_set(eo, 64, 64);
}

CElmGenGrid::~CElmGenGrid()
{
   page_relative.Dispose();
   align.Dispose();
   page_size.Dispose();
   scroller_policy.Dispose();
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

   Local<Value> next;

   if (replace->IsObject())
      next = value->ToObject()->GetHiddenValue(Item<CElmGenGrid>::str_next);

   if (next.IsEmpty())
     item->object_item = elm_gengrid_item_append(eo, item->GetElmClass(), item,
                                                 Item<CElmGenGrid>::OnSelect, item);
   else
     item->object_item = elm_gengrid_item_insert_before(eo, item->GetElmClass(), item,
                                                        (Elm_Object_Item *)External::Unwrap(next),
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
        Elm_Object_Item *next = elm_gengrid_item_next_get(item->object_item);
        if (next)
          attrs->ToObject()->SetHiddenValue(Item<CElmGenGrid>::str_next, External::Wrap(next));
     }
   delete item;

   return attrs;
}

void CElmGenGrid::UpdateItem(Handle<Value> value)
{
   Item<CElmGenGrid> *item = Item<CElmGenGrid>::Unwrap(value);
   if (item)
     elm_gengrid_item_item_class_update(item->object_item, item->GetElmClass());
}

Handle<Value> CElmGenGrid::page_show(const Arguments &args)
{
   if ((args[0]->IsInt32()) && (args[1]->IsInt32()))
     elm_gengrid_page_show (eo, args[0]->ToNumber()->Value(), args[1]->ToNumber()->Value());

   return Undefined();
}

Handle<Value> CElmGenGrid::page_bring(const Arguments &args)
{
   if ((args[0]->IsInt32()) && (args[1]->IsInt32()))
     elm_gengrid_page_bring_in (eo, args[0]->ToNumber()->Value(), args[1]->ToNumber()->Value());

   return Undefined();
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

Handle<Value> CElmGenGrid::page_relative_get() const
{
   return page_relative;
}

void CElmGenGrid::page_relative_set(Handle<Value> val)
{
   if (!val->IsArray())
     return;

   Local<Object> sizes = val->ToObject();
   elm_gengrid_page_relative_set(eo,
        sizes->Get(0)->ToNumber()->Value(),
        sizes->Get(1)->ToNumber()->Value());

   page_relative.Dispose();
   page_relative = Persistent<Value>::New(val);
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

Handle<Value> CElmGenGrid::page_size_get(void) const
{
   return page_size;
}

void CElmGenGrid::page_size_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;

   Local<Value> w = val->ToObject()->Get(String::NewSymbol("width"));
   Local<Value> h = val->ToObject()->Get(String::NewSymbol("height"));

   if (w->IsInt32() && h->IsInt32())
     elm_gengrid_page_size_set(eo, w->Int32Value(), h->Int32Value());

   page_size.Dispose();
   page_size = Persistent<Value>::New(val);
}

Handle<Value> CElmGenGrid::scroller_policy_get() const
{
   return scroller_policy;
}

void CElmGenGrid::scroller_policy_set(Handle<Value> val)
{
   if (!val->IsArray())
     return;

   Local<Object> policy = val->ToObject();
   elm_gengrid_scroller_policy_set (eo,
        (Elm_Scroller_Policy) policy->Get(0)->ToNumber()->Value(),
        (Elm_Scroller_Policy) policy->Get(1)->ToNumber()->Value());

   scroller_policy.Dispose();
   scroller_policy = Persistent<Value>::New(val);
}

Handle<Value> CElmGenGrid::items_count_get() const
{
   return Number::New(elm_gengrid_items_count(eo));
}

Handle<Value> CElmGenGrid::vertical_current_page_get() const
{
   int v;
   elm_gengrid_current_page_get(eo, NULL, &v);
   return Number::New(v);
}

Handle<Value> CElmGenGrid::horizontal_current_page_get() const
{
   int h;
   elm_gengrid_current_page_get(eo, &h, NULL);
   return Number::New(h);
}

Handle<Value> CElmGenGrid::vertical_last_page_get() const
{
   int v;
   elm_gengrid_last_page_get(eo, NULL, &v);
   return Number::New(v);
}

Handle<Value> CElmGenGrid::horizontal_last_page_get() const
{
   int h;
   elm_gengrid_last_page_get(eo, &h, NULL);
   return Number::New(h);
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

}
