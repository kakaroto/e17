#include "elm.h"
#include "CElmGenList.h"

namespace elm {

using namespace v8;

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
GENERATE_METHOD_CALLBACKS(CElmGenList, append);
GENERATE_METHOD_CALLBACKS(CElmGenList, clear);

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
                  METHOD(append),
                  METHOD(clear));

CElmGenList::CElmGenList(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_genlist_add(elm_object_top_widget_get(parent->GetEvasObject())))
{
}

void CElmGenList::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("Genlist"), GetTemplate()->GetFunction());
}

/* GenList functions that are going to do the heavy weight lifting */
char *CElmGenList::text_get(void *data, Evas_Object *, const char *part)
{
   GenListItemClass *itc = (GenListItemClass *)data;
   Handle<Function> fn(Function::Cast(*(itc->on_text)));
   Local<Object> temp = Object::New();
   temp->Set(String::New("part"), String::New(part));
   temp->Set(String::New("data"), itc->data);
   Handle<Value> args[1] = { temp };
   Local<Value> text = fn->Call(temp, 1, args);

   if (!text->IsString())
     return NULL;

   return strdup(*String::Utf8Value(text->ToString()));
}

Evas_Object *CElmGenList::content_get(void *data, Evas_Object *, const char *part)
{
   GenListItemClass *itc = (GenListItemClass *)data;
   Handle<Function> fn(Function::Cast(*(itc->on_content)));
   Local<Object> temp = Object::New();
   temp->Set(String::New("part"), String::New(part));
   temp->Set(String::New("data"), itc->data);
   Handle<Value> args[1] = { temp };
   Local<Value> retval = fn->Call(temp, 1, args);

   if (!retval->IsObject())
     return NULL;

   /* FIXME: This leaks -- properly handle these resources. */
   Handle<Value> content = Realise(retval->ToObject(), itc->genlist->GetJSObject());
   if (content.IsEmpty())
     return NULL;
   return GetEvasObjectFromJavascript(content);
}

Eina_Bool CElmGenList::state_get(void *, Evas_Object *, const char *)
{
   return EINA_TRUE;
}

void CElmGenList::del(void *data, Evas_Object *)
{
   delete static_cast<GenListItemClass *>(data);
}

void CElmGenList::sel(void *data, Evas_Object *, void *)
{
   GenListItemClass *itc = (GenListItemClass *)data;

   if (itc->on_select.IsEmpty())
     return;

   Handle<Function> fn(Function::Cast(*(itc->on_select)));
   Local<Object> temp = Object::New();
   temp->Set(String::New("data"), itc->data);
   fn->Call(temp, 0, 0);
}

Handle<Value> CElmGenList::clear(const Arguments&)
{
   /* FIXME: Properly dispose of resources */
   elm_genlist_clear(eo);
   return Undefined();
}

Handle<Value> CElmGenList::append(const Arguments& args)
{
   if (!args[0]->IsObject())
     return Undefined();

   GenListItemClass *itc = new GenListItemClass();

   Local<Object> obj = args[0]->ToObject();
   itc->type = Persistent<Value>::New(obj->Get(String::New("type")));
   itc->on_text = Persistent<Value>::New(obj->Get(String::New("text")));
   itc->on_content = Persistent<Value>::New(obj->Get(String::New("content")));
   itc->on_state = Persistent<Value>::New(obj->Get(String::New("state")));
   itc->on_select = Persistent<Value>::New(obj->Get(String::New("select")));
   itc->data = Persistent<Value>::New(obj->Get(String::New("data")));
   //TODO : Check genlist class type and modify or add

   itc->eitc.item_style = "default";
   itc->eitc.func.text_get = text_get;
   itc->eitc.func.content_get = content_get;
   itc->eitc.func.state_get = state_get;
   itc->eitc.func.del = del;
   itc->genlist = this;
   elm_genlist_item_append(eo, &itc->eitc, itc, NULL,
                           ELM_GENLIST_ITEM_NONE,
                           sel, itc);

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

}
