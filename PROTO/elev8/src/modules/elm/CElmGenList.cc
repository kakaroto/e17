#include "CElmGenList.h"

CElmGenList::CElmGenList(CEvasObject *parent, Local<Object> obj) :
    CEvasObject(),
    prop_handler(property_list_base)
{
    eo = elm_genlist_add(parent->top_widget_get());
    construct(eo, obj);
    get_object()->Set(String::New("append"), FunctionTemplate::New(append)->GetFunction());
    get_object()->Set(String::New("clear"), FunctionTemplate::New(clear)->GetFunction());
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

    if (text->IsString())
    {
        String::Utf8Value str(text->ToString());
        return strdup(*str);
    }
    else
        return NULL;
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

    if (retval->IsObject())
    {
        CEvasObject *content = make_or_get(itc->genlist, retval->ToObject());
        if (content)
            return content->get();
    }

    return NULL;
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
/* End of GenList functions */

Handle<Value> CElmGenList::clear(const Arguments& args)
{
    CEvasObject *self = eo_from_info(args.This());
    elm_genlist_clear(static_cast<CElmGenList *>(self)->get());
    return Undefined();
}

Handle<Value> CElmGenList::append(const Arguments& args)
{
    CEvasObject *self = eo_from_info(args.This());
    CElmGenList *genlist = static_cast<CElmGenList *>(self);
    if (args[0]->IsObject())
    {
        GenListItemClass *itc = new GenListItemClass();

        Local<Object> obj = args[0]->ToObject();
        itc->type = Persistent<Value>::New(obj->Get(String::New("type")));
        itc->on_text = Persistent<Value>::New(obj->Get(String::New("on_text")));
        itc->on_content = Persistent<Value>::New(obj->Get(String::New("on_content")));
        itc->on_state = Persistent<Value>::New(obj->Get(String::New("on_state")));
        itc->on_select = Persistent<Value>::New(obj->Get(String::New("on_select")));
        itc->data = Persistent<Value>::New(obj->Get(String::New("data")));
        //TODO : Check genlist class type and modify or add

        itc->eitc.item_style = "default";
        itc->eitc.func.text_get = text_get;
        itc->eitc.func.content_get = content_get;
        itc->eitc.func.state_get = state_get;
        itc->eitc.func.del = del;
        itc->genlist = genlist;
        elm_genlist_item_append(genlist->get(), &itc->eitc, itc, NULL,
                ELM_GENLIST_ITEM_NONE,
                sel, itc);
    }
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
    return Number::New(elm_genlist_mode_get(eo));
}

void CElmGenList::mode_set(Handle<Value> value)
{
    if (value->IsNumber())
        elm_genlist_mode_set(eo, (Elm_List_Mode)value->ToNumber()->Value());
}

Handle<Value> CElmGenList::select_mode_get() const
{
    return Number::New(elm_genlist_select_mode_get(eo));
}

void CElmGenList::select_mode_set(Handle<Value> value)
{
    if (value->IsNumber())
      {
          elm_genlist_select_mode_set(eo,(Elm_Object_Select_Mode)value->ToNumber()->Value());
      }
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

template<> CEvasObject::CPropHandler<CElmGenList>::property_list
CEvasObject::CPropHandler<CElmGenList>::list[] = {
    PROP_HANDLER(CElmGenList, multi_select),
    PROP_HANDLER(CElmGenList, select_mode),
    PROP_HANDLER(CElmGenList, reorder_mode),
    PROP_HANDLER(CElmGenList, mode),
    PROP_HANDLER(CElmGenList, block_count),
    PROP_HANDLER(CElmGenList, longpress_timeout),
    { NULL, NULL, NULL },
};

