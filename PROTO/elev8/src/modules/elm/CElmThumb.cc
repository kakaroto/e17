#include "elm.h"
#include "CElmThumb.h"

namespace elm {

using namespace v8;

GENERATE_PROPERTY_CALLBACKS(CElmThumb, file);
GENERATE_PROPERTY_CALLBACKS(CElmThumb, path);
GENERATE_PROPERTY_CALLBACKS(CElmThumb, animate);
GENERATE_PROPERTY_CALLBACKS(CElmThumb, editable);
GENERATE_PROPERTY_CALLBACKS(CElmThumb, on_generate_start);
GENERATE_PROPERTY_CALLBACKS(CElmThumb, on_generate_stop);
GENERATE_PROPERTY_CALLBACKS(CElmThumb, on_generate_error);
GENERATE_PROPERTY_CALLBACKS(CElmThumb, on_load_error);
GENERATE_METHOD_CALLBACKS(CElmThumb, reload);

GENERATE_TEMPLATE(CElmThumb,
                  METHOD(reload),
                  PROPERTY(file),
                  PROPERTY(path),
                  PROPERTY(animate),
                  PROPERTY(editable),
                  PROPERTY(on_generate_start),
                  PROPERTY(on_generate_stop),
                  PROPERTY(on_generate_error),
                  PROPERTY(on_load_error));

CElmThumb::CElmThumb(Local<Object> _jsObject, CElmObject *parent)
   : CElmObject(_jsObject, elm_thumb_add(parent->GetEvasObject()))
{
}

CElmThumb::~CElmThumb()
{
   on_generate_start_set(Undefined());
   on_generate_stop_set(Undefined());
   on_generate_error_set(Undefined());
   on_load_error_set(Undefined());
}

void CElmThumb::Initialize(Handle<Object> target)
{
#ifdef ELM_ETHUMB
   if (!elm_need_ethumb())
     {
        ELM_ERR("Cannot initialize ethumb!");
        return;
     }
#endif

   target->Set(String::NewSymbol("Thumb"), GetTemplate()->GetFunction());
}

Handle<Value> CElmThumb::reload(const Arguments)
{
   elm_thumb_reload(eo);
   return Undefined();
}

void CElmThumb::file_set(Handle<Value> val)
{
   if (!val->IsString())
     return;

   String::Utf8Value str(val);
   elm_thumb_file_set(eo, *str, NULL);
}

Handle<Value> CElmThumb::file_get(void) const
{
   const char *f = NULL;

   elm_thumb_file_get(eo, &f, NULL);
   if (f)
     return String::New(f);
   else
     return Null();
}

void CElmThumb::path_set(Handle<Value>)
{
   ELM_WRN("path is a read-only attribute!");
}

Handle<Value> CElmThumb::path_get() const
{
   const char *p = NULL;

   elm_thumb_path_get(eo, &p, NULL);
   if (p)
     return String::New(p);
   else
     return Null();
}

Handle<Value> CElmThumb::animate_get() const
{
   int orient = elm_thumb_animate_get(eo);
   return Integer::New(orient);
}

void CElmThumb::animate_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_thumb_animate_set(eo,
        (Elm_Thumb_Animation_Setting)val->IntegerValue());
}

Handle<Value> CElmThumb::editable_get() const
{
   return Boolean::New(elm_thumb_editable_get(eo));
}

void CElmThumb::editable_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_thumb_editable_set(eo, val->BooleanValue());
}

void CElmThumb::OnGenerateStart(void *)
{
   Handle<Function> callback(Function::Cast(*cb.generate_start));
   Handle<Value> args[1] = { jsObject };

   callback->Call(jsObject, 1, args);
}

void CElmThumb::OnGenerateStartWrapper(void *data, Evas_Object *, void *event_info)
{
   static_cast<CElmThumb*>(data)->OnGenerateStart(event_info);
}

Handle<Value> CElmThumb::on_generate_start_get() const
{
   return cb.generate_start;
}

void CElmThumb::on_generate_start_set(Handle<Value> val)
{
   if (!cb.generate_start.IsEmpty())
     {
        evas_object_smart_callback_del(eo, "generate,start", &OnGenerateStartWrapper);
        cb.generate_start.Dispose();
        cb.generate_start.Clear();
     }

   if (!val->IsFunction())
     return;

   cb.generate_start = Persistent<Value>::New(val);
   evas_object_smart_callback_add(eo, "generate,start", &OnGenerateStartWrapper, this);
}

void CElmThumb::OnGenerateStop(void *)
{
   Handle<Function> callback(Function::Cast(*cb.generate_stop));
   Handle<Value> args[1] = { jsObject };

   callback->Call(jsObject, 1, args);
}

void CElmThumb::OnGenerateStopWrapper(void *data, Evas_Object *, void *event_info)
{
   static_cast<CElmThumb*>(data)->OnGenerateStop(event_info);
}

Handle<Value> CElmThumb::on_generate_stop_get() const
{
   return cb.generate_stop;
}

void CElmThumb::on_generate_stop_set(Handle<Value> val)
{
   if (!cb.generate_stop.IsEmpty())
     {
        evas_object_smart_callback_del(eo, "generate,stop", &OnGenerateStopWrapper);
        cb.generate_stop.Dispose();
        cb.generate_stop.Clear();
     }

   if (!val->IsFunction())
     return;

   cb.generate_stop = Persistent<Value>::New(val);
   evas_object_smart_callback_add(eo, "generate,stop", &OnGenerateStopWrapper, this);
}

void CElmThumb::OnGenerateError(void *)
{
   Handle<Function> callback(Function::Cast(*cb.generate_error));
   Handle<Value> args[1] = { jsObject };

   callback->Call(jsObject, 1, args);
}

void CElmThumb::OnGenerateErrorWrapper(void *data, Evas_Object *, void *event_info)
{
   static_cast<CElmThumb*>(data)->OnGenerateError(event_info);
}

Handle<Value> CElmThumb::on_generate_error_get() const
{
   return cb.generate_error;
}

void CElmThumb::on_generate_error_set(Handle<Value> val)
{
   if (!cb.generate_error.IsEmpty())
     {
        evas_object_smart_callback_del(eo, "generate,error", &OnGenerateErrorWrapper);
        cb.generate_error.Dispose();
        cb.generate_error.Clear();
     }

   if (!val->IsFunction())
     return;

   cb.generate_error = Persistent<Value>::New(val);
   evas_object_smart_callback_add(eo, "generate,error", &OnGenerateErrorWrapper, this);
}

void CElmThumb::OnLoadError(void *)
{
   Handle<Function> callback(Function::Cast(*cb.load_error));
   Handle<Value> args[1] = { jsObject };

   callback->Call(jsObject, 1, args);
}

void CElmThumb::OnLoadErrorWrapper(void *data, Evas_Object *, void *event_info)
{
   static_cast<CElmThumb*>(data)->OnLoadError(event_info);
}

Handle<Value> CElmThumb::on_load_error_get() const
{
   return cb.load_error;
}

void CElmThumb::on_load_error_set(Handle<Value> val)
{
   if (!cb.load_error.IsEmpty())
     {
        evas_object_smart_callback_del(eo, "load,error", &OnLoadErrorWrapper);
        cb.load_error.Dispose();
        cb.load_error.Clear();
     }

   if (!val->IsFunction())
     return;

   cb.load_error = Persistent<Value>::New(val);
   evas_object_smart_callback_add(eo, "load,error", &OnLoadErrorWrapper, this);
}

}
