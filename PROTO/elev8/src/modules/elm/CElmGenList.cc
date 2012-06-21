#include "elm.h"
#include "CElmGenList.h"

namespace elm {

using namespace v8;
using namespace gen;

GENERATE_PROPERTY_CALLBACKS(CElmGenList, homogeneous);
GENERATE_PROPERTY_CALLBACKS(CElmGenList, decorate_mode);
GENERATE_PROPERTY_CALLBACKS(CElmGenList, tree_effect);
GENERATE_PROPERTY_CALLBACKS(CElmGenList, highlight_mode);
GENERATE_PROPERTY_CALLBACKS(CElmGenList, vertical_bounce);
GENERATE_PROPERTY_CALLBACKS(CElmGenList, horizontal_bounce);
GENERATE_PROPERTY_CALLBACKS(CElmGenList, longpress_timeout);
GENERATE_PROPERTY_CALLBACKS(CElmGenList, block_count);
GENERATE_PROPERTY_CALLBACKS(CElmGenList, select_mode);
GENERATE_PROPERTY_CALLBACKS(CElmGenList, mode);
GENERATE_PROPERTY_CALLBACKS(CElmGenList, reorder_mode);
GENERATE_PROPERTY_CALLBACKS(CElmGenList, multi_select);
GENERATE_PROPERTY_CALLBACKS(CElmGenList, classes);
GENERATE_PROPERTY_CALLBACKS(CElmGenList, on_longpress);
GENERATE_METHOD_CALLBACKS(CElmGenList, append);
GENERATE_METHOD_CALLBACKS(CElmGenList, clear);
GENERATE_METHOD_CALLBACKS(CElmGenList, delete_item);
GENERATE_METHOD_CALLBACKS(CElmGenList, update_item);

GENERATE_TEMPLATE(CElmGenList,
                  PROPERTY(homogeneous),
                  PROPERTY(decorate_mode),
                  PROPERTY(tree_effect),
                  PROPERTY(highlight_mode),
                  PROPERTY(horizontal_bounce),
                  PROPERTY(vertical_bounce),
                  PROPERTY(longpress_timeout),
                  PROPERTY(block_count),
                  PROPERTY(select_mode),
                  PROPERTY(mode),
                  PROPERTY(reorder_mode),
                  PROPERTY(multi_select),
                  PROPERTY(classes),
                  PROPERTY(on_longpress),
                  METHOD(append),
                  METHOD(clear),
                  METHOD(delete_item),
                  METHOD(update_item));

CElmGenList::CElmGenList(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_genlist_add(elm_object_top_widget_get(parent->GetEvasObject())))
{
}

CElmGenList::~CElmGenList()
{
   on_longpress_set(Undefined());
}

void CElmGenList::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Genlist"), GetTemplate()->GetFunction());
}

Handle<Value> CElmGenList::clear(const Arguments&)
{
   /* FIXME: Properly dispose of resources */
   elm_genlist_clear(eo);
   return Undefined();
}

Handle<Value> CElmGenList::append(const Arguments& args)
{
   if (!args[0]->IsString())
     return Undefined();

   Handle<Value> klass = cached.classes->Get(args[0]->ToString());
   if (klass.IsEmpty() || !klass->IsObject())
     return Undefined();

   ItemClass<CElmGenList> *item_class = static_cast<ItemClass<CElmGenList> *>(External::Unwrap(klass->ToObject()->GetHiddenValue(String::NewSymbol("genlist::itemclass"))));
   Item<CElmGenList> *item = new Item<CElmGenList>(item_class, args[1], args[2]);

   item->object_item = elm_genlist_item_append(eo, item_class->GetElmClass(), item, NULL,
          ELM_GENLIST_ITEM_NONE, Item<CElmGenList>::OnSelect, item);
   elm_object_item_data_set(item->object_item, item);

   return External::Wrap(item);
}

Handle<Value> CElmGenList::delete_item(const Arguments &args)
{
   static_cast<Item<CElmGenList> *>(External::Unwrap(args[0]))->Destroy();
   return Undefined();
}

Handle<Value> CElmGenList::update_item(const Arguments &args)
{
   Item<CElmGenList> *item = static_cast<Item<CElmGenList> *>(External::Unwrap(args[0]));
   if (!item)
     return Undefined();

   if (!args[1]->IsUndefined())
     {
        item->data.Dispose();
        item->data = Persistent<Value>::New(args[1]);
     }

   if (item->object_item)
     elm_genlist_item_update(item->object_item);

   return Undefined();
}

Handle<Value> CElmGenList::multi_select_get() const
{
   return Boolean::New(elm_genlist_multi_select_get(eo));
}

void CElmGenList::multi_select_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_genlist_multi_select_set(eo, value->BooleanValue());
}

Handle<Value> CElmGenList::reorder_mode_get() const
{
   return Boolean::New(elm_genlist_reorder_mode_get(eo));
}

void CElmGenList::reorder_mode_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_genlist_reorder_mode_set(eo, value->BooleanValue());
}

Handle<Value> CElmGenList::mode_get() const
{
   switch (elm_genlist_mode_get(eo))
     {
        case ELM_LIST_COMPRESS: return String::NewSymbol("compress");
        case ELM_LIST_SCROLL: return String::NewSymbol("scroll");
        case ELM_LIST_LIMIT: return String::NewSymbol("limit");
        case ELM_LIST_EXPAND: return String::NewSymbol("expand");
        default: return String::NewSymbol("unknown");
     }
}

void CElmGenList::mode_set(Handle<Value> value)
{
   String::Utf8Value mode_string(value->ToString());

   if (!strcmp(*mode_string, "compress"))
     elm_genlist_mode_set(eo, ELM_LIST_COMPRESS);
   else if (!strcmp(*mode_string, "scroll"))
     elm_genlist_mode_set(eo, ELM_LIST_SCROLL);
   else if (!strcmp(*mode_string, "limit"))
     elm_genlist_mode_set(eo, ELM_LIST_LIMIT);
   else if (!strcmp(*mode_string, "expand"))
     elm_genlist_mode_set(eo, ELM_LIST_EXPAND);
}

Handle<Value> CElmGenList::select_mode_get() const
{
   return Number::New(elm_genlist_select_mode_get(eo));
}

void CElmGenList::select_mode_set(Handle<Value> value)
{
   if (value->IsNumber())
     elm_genlist_select_mode_set(eo,(Elm_Object_Select_Mode)value->ToNumber()->Value());
}

Handle<Value> CElmGenList::block_count_get() const
{
   return Number::New(elm_genlist_block_count_get(eo));
}

void CElmGenList::block_count_set(Handle<Value> value)
{
   if (value->IsNumber())
     elm_genlist_block_count_set(eo, value->IntegerValue());
}

Handle<Value> CElmGenList::longpress_timeout_get() const
{
   return Number::New(elm_genlist_longpress_timeout_get(eo));
}

void CElmGenList::longpress_timeout_set(Handle<Value> value)
{
   if (value->IsNumber())
     elm_genlist_longpress_timeout_set(eo, value->IntegerValue());
}

void CElmGenList::vertical_bounce_set(Handle<Value> val)
{
   Eina_Bool h;
   elm_genlist_bounce_get(eo, &h, NULL);
   elm_genlist_bounce_set(eo, h, val->IsBoolean() && val->BooleanValue());
}

Handle<Value> CElmGenList::vertical_bounce_get() const
{
   Eina_Bool v;
   elm_genlist_bounce_get(eo, NULL, &v);
   return Boolean::New(v);
}

void CElmGenList::horizontal_bounce_set(Handle<Value> val)
{
   Eina_Bool v;
   elm_genlist_bounce_get(eo, NULL, &v);
   elm_genlist_bounce_set(eo, val->IsBoolean() && val->BooleanValue(), v);
}

Handle<Value> CElmGenList::horizontal_bounce_get() const
{
   Eina_Bool h;
   elm_genlist_bounce_get(eo, &h, NULL);
   return Boolean::New(h);
}

Handle<Value> CElmGenList::highlight_mode_get() const
{
   return Boolean::New(elm_genlist_highlight_mode_get(eo));
}

void CElmGenList::highlight_mode_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_genlist_highlight_mode_set(eo, value->BooleanValue());
}

Handle<Value> CElmGenList::tree_effect_get() const
{
   return Boolean::New(elm_genlist_tree_effect_enabled_get(eo));
}

void CElmGenList::tree_effect_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_genlist_tree_effect_enabled_set(eo, value->BooleanValue());
}

Handle<Value> CElmGenList::decorate_mode_get() const
{
   return Boolean::New(elm_genlist_decorate_mode_get(eo));
}

void CElmGenList::decorate_mode_set(Handle<Value> value)
{
   if (value->IsBoolean())
     elm_genlist_decorate_mode_set(eo, value->BooleanValue());
}

Handle<Value> CElmGenList::homogeneous_get() const
{
   return Number::New(elm_genlist_homogeneous_get(eo));
}

void CElmGenList::homogeneous_set(Handle<Value> value)
{
   if (value->IsNumber())
     elm_genlist_homogeneous_set(eo, value->IntegerValue());
}

void CElmGenList::classes_set(Handle<Value> value)
{
   Local<Object> classes = value->ToObject();
   Local<Array> properties = classes->GetOwnPropertyNames();

   if (!cached.classes.IsEmpty())
     {
        for (unsigned int i = 0; i < properties->Length(); ++i)
          {
             Local<String> class_name = properties->Get(i)->ToString();
             Local<Object> class_desc = classes->Get(class_name)->ToObject();
             delete static_cast<ItemClass<CElmGenList> *>(External::Unwrap(class_desc->GetHiddenValue(String::NewSymbol("genlist::itemclass"))));
          }
     }
   cached.classes.Dispose();

   cached.classes = Persistent<Object>::New(classes);
   for (unsigned int i = 0; i < properties->Length(); ++i)
     {
        Local<String> class_name = properties->Get(i)->ToString();
        Local<Object> class_desc = classes->Get(class_name)->ToObject();
        ItemClass<CElmGenList> *item_class = new ItemClass<CElmGenList>(this, class_name, class_desc);
        class_desc->SetHiddenValue(String::NewSymbol("genlist::itemclass"), External::Wrap(item_class));
     }
}

Handle<Value> CElmGenList::classes_get() const
{
   return cached.classes;
}

void CElmGenList::OnLongPress(void *event_info)
{
   Handle<Function> callback(Function::Cast(*cb.longpress));
   Item<CElmGenList> *item = static_cast< Item<CElmGenList> *>
      (elm_object_item_data_get((Elm_Object_Item *)event_info));
   Handle<Value> args[2] = { item->data, External::Wrap(item->object_item) };
   callback->Call(jsObject, 2, args);
}

void CElmGenList::OnLongPressWrapper(void *data, Evas_Object *, void *event_info)
{
   static_cast<CElmGenList*>(data)->OnLongPress(event_info);
}

Handle<Value> CElmGenList::on_longpress_get() const
{
   return cb.longpress;
}

void CElmGenList::on_longpress_set(Handle<Value> val)
{
   if (!cb.longpress.IsEmpty())
     {
        evas_object_smart_callback_del(eo, "longpressed", &OnLongPressWrapper);
        cb.longpress.Dispose();
        cb.longpress.Clear();
     }

   if (!val->IsFunction())
     return;

   cb.longpress = Persistent<Value>::New(val);
   evas_object_smart_callback_add(eo, "longpressed", &OnLongPressWrapper, this);
}

}
