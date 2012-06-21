#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

Persistent<FunctionTemplate> CElmObject::tmpl;

GENERATE_PROPERTY_CALLBACKS(CElmObject, x);
GENERATE_PROPERTY_CALLBACKS(CElmObject, y);
GENERATE_PROPERTY_CALLBACKS(CElmObject, width);
GENERATE_PROPERTY_CALLBACKS(CElmObject, height);
GENERATE_PROPERTY_CALLBACKS(CElmObject, align);
GENERATE_PROPERTY_CALLBACKS(CElmObject, weight);
GENERATE_PROPERTY_CALLBACKS(CElmObject, text);
GENERATE_PROPERTY_CALLBACKS(CElmObject, scale);
GENERATE_PROPERTY_CALLBACKS(CElmObject, style);
GENERATE_PROPERTY_CALLBACKS(CElmObject, visible);
GENERATE_PROPERTY_CALLBACKS(CElmObject, enabled);
GENERATE_PROPERTY_CALLBACKS(CElmObject, hint_min);
GENERATE_PROPERTY_CALLBACKS(CElmObject, hint_max);
GENERATE_PROPERTY_CALLBACKS(CElmObject, hint_req);
GENERATE_PROPERTY_CALLBACKS(CElmObject, focus);
GENERATE_PROPERTY_CALLBACKS(CElmObject, layer);
GENERATE_PROPERTY_CALLBACKS(CElmObject, label);
GENERATE_PROPERTY_CALLBACKS(CElmObject, padding);
GENERATE_PROPERTY_CALLBACKS(CElmObject, pointer_mode);
GENERATE_PROPERTY_CALLBACKS(CElmObject, antialias);
GENERATE_PROPERTY_CALLBACKS(CElmObject, static_clip);
GENERATE_PROPERTY_CALLBACKS(CElmObject, size_hint_aspect);
GENERATE_PROPERTY_CALLBACKS(CElmObject, name);
GENERATE_PROPERTY_CALLBACKS(CElmObject, resize);
GENERATE_PROPERTY_CALLBACKS(CElmObject, pointer);
GENERATE_PROPERTY_CALLBACKS(CElmObject, on_animate);
GENERATE_PROPERTY_CALLBACKS(CElmObject, on_click);
GENERATE_PROPERTY_CALLBACKS(CElmObject, on_key_down);

static Handle<Value> Callback_elements_get(Local<String>, const AccessorInfo &info)
{
   HandleScope scope;
   return info.This()->GetHiddenValue(String::NewSymbol("elm::elements"));
}

static void Callback_elements_set(Local<String>, Local<Value> value, const AccessorInfo &info)
{
   HandleScope scope;

   Local<Object> obj = value->ToObject();
   Local<Array> props = obj->GetOwnPropertyNames();

   Handle<Object> items = Object::New();

   for (unsigned int i = 0; i < props->Length(); i++)
     {
        HandleScope handle_scope;
        Local<String> key = props->Get(i)->ToString();
        Local<Object> val = obj->Get(key)->ToObject();

        items->Set(key, CElmObject::Realise(val, info.This()));
     }

   Local<ObjectTemplate> tmpl = ObjectTemplate::New();
   tmpl->SetNamedPropertyHandler(CElmObject::ElementGet< Local<String> >,
                                 CElmObject::ElementSet< Local<String> >,
                                 NULL, CElmObject::ElementDel< Local<String> >,
                                 CElmObject::ElementEnum);

   tmpl->SetIndexedPropertyHandler(CElmObject::ElementGet<uint32_t>,
                                   CElmObject::ElementSet<uint32_t>,
                                   NULL, CElmObject::ElementDel<uint32_t>,
                                   CElmObject::ElementEnum);

   Handle<Object> elements = tmpl->NewInstance();
   elements->SetHiddenValue(String::NewSymbol("elm::items"), items);
   elements->SetHiddenValue(String::NewSymbol("elm::parent"), info.This());
   info.This()->SetHiddenValue(String::NewSymbol("elm::elements"), elements);
}

void CElmObject::Delete(Persistent<Value>, void *parameter)
{
   CElmObject *self = static_cast<CElmObject *>(parameter);

   evas_object_data_set(self->eo, "deleted-from-gc", (void *)1);
   delete static_cast<CElmObject *>(self);
}

void CElmObject::EvasFreeEvent(void *data, Evas *, void *)
{
   CElmObject *self = static_cast<CElmObject *>(data);

   if (evas_object_data_get(self->eo, "deleted-from-gc"))
     return;

   ELM_DBG("Received EVAS_CALLBACK_FREE event for CElmObject @ %p (%s). " \
           "Nullifying pointer to Evas_Object: expect breakage.",
           data, *String::Utf8Value(self->jsObject->ObjectProtoToString()));

   self->eo = NULL;
}

CElmObject::CElmObject(Local<Object> _jsObject, Evas_Object *_eo)
   : eo(_eo)
   , current_animator(NULL)
{
   jsObject = Persistent<Object>::New(_jsObject);
   jsObject->SetPointerInInternalField(0, this);
   evas_event_callback_add(evas_object_evas_get(eo),
                           EVAS_CALLBACK_FREE, CElmObject::EvasFreeEvent, this);
}

CElmObject::~CElmObject()
{
   on_animate_set(Undefined());
   on_click_set(Undefined());
   on_key_down_set(Undefined());

   Local<Value> packer = jsObject->GetHiddenValue(String::NewSymbol("elm::packer"));

   if (!packer.IsEmpty())
     GetObjectFromJavascript(packer)->Unpack(jsObject);

   evas_object_del(eo);

   jsObject.Dispose();
   jsObject.Clear();
}

void CElmObject::ApplyProperties(Handle<Object> obj)
{
   HandleScope scope;

   Local<Array> props = obj->GetOwnPropertyNames();
   for (unsigned int i = 0; i < props->Length(); i++)
     {
        Local<String> key = props->Get(i)->ToString();
        jsObject->Set(key, obj->Get(key));
     }
}

void CElmObject::Initialize(Handle<Object> target)
{
   target->Set(String::NewSymbol("realise"),
               FunctionTemplate::New(Realise)->GetFunction());
}

Handle<FunctionTemplate> CElmObject::GetTemplate()
{
   if (!tmpl.IsEmpty())
     return tmpl;

   HandleScope scope;
   tmpl = Persistent<FunctionTemplate>::New(FunctionTemplate::New());
   tmpl->InstanceTemplate()->SetInternalFieldCount(1);

   RegisterProperties(tmpl->PrototypeTemplate(),
                      PROPERTY(x),
                      PROPERTY(y),
                      PROPERTY(width),
                      PROPERTY(height),
                      PROPERTY(align),
                      PROPERTY(weight),
                      PROPERTY(text),
                      PROPERTY(scale),
                      PROPERTY(style),
                      PROPERTY(visible),
                      PROPERTY(enabled),
                      PROPERTY(hint_min),
                      PROPERTY(hint_max),
                      PROPERTY(hint_req),
                      PROPERTY(focus),
                      PROPERTY(layer),
                      PROPERTY(label),
                      PROPERTY(padding),
                      PROPERTY(pointer_mode),
                      PROPERTY(antialias),
                      PROPERTY(static_clip),
                      PROPERTY(size_hint_aspect),
                      PROPERTY(name),
                      PROPERTY(resize),
                      PROPERTY(pointer),
                      PROPERTY(on_animate),
                      PROPERTY(on_click),
                      PROPERTY(on_key_down),
                      PROPERTY(elements),
                      NULL);

   return tmpl;
}

// Getters and Settters

Handle<Value> CElmObject::x_get() const
{
   int x;
   evas_object_geometry_get(eo, &x, NULL, NULL, NULL);
   return Integer::New(x);
}

void CElmObject::x_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;
   int y;
   evas_object_geometry_get(eo, NULL, &y, NULL, NULL);
   evas_object_move(eo, val->ToInt32()->Value(), y);
}

Handle<Value> CElmObject::y_get() const
{
   int y;
   evas_object_geometry_get(eo, NULL, &y, NULL, NULL);
   return Integer::New(y);
}

void CElmObject::y_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;
   int x;
   evas_object_geometry_get(eo, &x, NULL, NULL, NULL);
   evas_object_move(eo, x, val->ToInt32()->Value());
}

Handle<Value> CElmObject::width_get() const
{
   int width;
   evas_object_geometry_get(eo, NULL, NULL, &width, NULL);
   return Integer::New(width);
}

void CElmObject::width_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;
   int height;
   evas_object_geometry_get(eo, NULL, NULL, NULL, &height);
   evas_object_resize(eo, val->ToInt32()->Value(), height);
}

Handle<Value> CElmObject::height_get() const
{
   int height;
   evas_object_geometry_get(eo, NULL, NULL, NULL, &height);
   return Integer::New(height);
}

void CElmObject::height_set(Handle<Value> val)
{
   if (!val->IsNumber())
     return;
   int width;
   evas_object_geometry_get(eo, NULL, NULL, &width, NULL);
   evas_object_resize(eo, width, val->ToInt32()->Value());
}

Handle<Value> CElmObject::text_get() const
{
   const char *s = elm_object_text_get(eo);
   return s ? String::New(s) : Undefined();
}

void CElmObject::text_set(Handle<Value> val)
{
   if (!val->IsString())
     return;

   char *text = elm_entry_utf8_to_markup(*String::Utf8Value(val));
   elm_object_text_set(eo, text);
   free(text);
}

Handle<Value> CElmObject::scale_get() const
{
   return Number::New(elm_object_scale_get(eo));
}

void CElmObject::scale_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_object_scale_set(eo, val->NumberValue());
}

void CElmObject::style_set(Handle<Value> val)
{
   if (val->IsString())
     elm_object_style_set(eo, *String::Utf8Value(val));
}

Handle<Value> CElmObject::style_get(void) const
{
   const char *s = elm_object_style_get(eo);
   return s ? String::New(s) : Undefined();
}

Handle<Value> CElmObject::align_get() const
{
   double x, y;
   evas_object_size_hint_align_get(eo, &x, &y);
   Local<Object> align = Object::New();
   align->Set(String::NewSymbol("x"), Number::New(x));
   align->Set(String::NewSymbol("y"), Number::New(y));
   return align;
}

void CElmObject::align_set(Handle<Value> value)
{
   if (!value->IsObject())
     return;

   Local<Object> align = value->ToObject();
   evas_object_size_hint_align_set(eo,
        align->Get(String::NewSymbol("x"))->ToNumber()->Value(),
        align->Get(String::NewSymbol("y"))->ToNumber()->Value());
}

Handle<Value> CElmObject::weight_get() const
{
   double x, y;
   evas_object_size_hint_weight_get(eo, &x, &y);
   Local<Object> align = Object::New();
   align->Set(String::NewSymbol("x"), Number::New(x));
   align->Set(String::NewSymbol("y"), Number::New(y));
   return align;
}

void CElmObject::weight_set(Handle<Value> value)
{
   if (!value->IsObject())
     return;

   Local<Object> align = value->ToObject();
   evas_object_size_hint_weight_set(eo,
        align->Get(String::NewSymbol("x"))->ToNumber()->Value(),
        align->Get(String::NewSymbol("y"))->ToNumber()->Value());
}

Handle<Value> CElmObject::visible_get() const
{
   return Boolean::New(evas_object_visible_get(eo));
}

void CElmObject::visible_set(Handle<Value> val)
{
   ((val->IsBoolean() && val->BooleanValue()) ? evas_object_show : evas_object_hide)(eo);
}

Handle<Value> CElmObject::enabled_get() const
{
   return Boolean::New(!elm_object_disabled_get(eo));
}

void CElmObject::enabled_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_object_disabled_set(eo, !val->BooleanValue());
}

Handle<Value> CElmObject::hint_min_get() const
{
   Evas_Coord w, h;

   evas_object_size_hint_min_get(eo,  &w, &h);

   Local<Object> obj = Object::New();
   obj->Set(String::New("width"), Number::New(w));
   obj->Set(String::New("height"), Number::New(h));

   return obj;
}

void CElmObject::hint_min_set(Handle<Value> val)
{
   if (!val->IsObject())
    return;
   Local<Object> obj = val->ToObject();
   Local<Value> w = obj->Get(String::New("x"));
   Local<Value> h = obj->Get(String::New("y"));
   if (w->IsInt32() && h->IsInt32())
     evas_object_size_hint_min_set(eo, w->Int32Value(), h->Int32Value());
}

Handle<Value> CElmObject::hint_max_get() const
{
   Evas_Coord w, h;

   evas_object_size_hint_max_get(eo, &w, &h);

   Local<Object> obj = Object::New();
   obj->Set(String::New("width"), Number::New(w));
   obj->Set(String::New("height"), Number::New(h));

   return obj;
}

void CElmObject::hint_max_set(Handle<Value> val)
{
   Local<Object> obj = val->ToObject();
   Local<Value> w = obj->Get(String::New("x"));
   Local<Value> h = obj->Get(String::New("y"));
   if (w->IsInt32() && h->IsInt32())
     evas_object_size_hint_max_set(eo, w->Int32Value(), h->Int32Value());
}

Handle<Value> CElmObject::hint_req_get() const
{
   Evas_Coord w, h;

   evas_object_size_hint_request_get(eo, &w, &h);

   Local<Object> obj = Object::New();
   obj->Set(String::New("width"), Number::New(w));
   obj->Set(String::New("height"), Number::New(h));

   return obj;
}

void CElmObject::hint_req_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;

   Local<Object> obj = val->ToObject();
   Local<Value> w = obj->Get(String::New("x"));
   Local<Value> h = obj->Get(String::New("y"));
   if (w->IsInt32() && h->IsInt32())
     evas_object_size_hint_request_set(eo, w->Int32Value(), h->Int32Value());
}

Handle<Value> CElmObject::focus_get() const
{
   return Boolean::New(evas_object_focus_get(eo));
}

void CElmObject::focus_set(Handle<Value> val)
{
   if (val->IsBoolean())
     evas_object_focus_set(eo, val->BooleanValue());
}

Handle<Value> CElmObject::layer_get() const
{
   return Number::New(evas_object_layer_get(eo));
}

void CElmObject::layer_set(Handle<Value> val)
{
   if (val->IsNumber())
     evas_object_layer_set(eo, val->NumberValue());
}

Handle<Value> CElmObject::label_get() const
{
   const char *s = elm_object_text_get(eo);
   return s ? String::New(s) : Undefined();
}

void CElmObject::label_set(Handle<Value> val)
{
   if (val->IsString() || val->IsNumber())
     elm_object_text_set(eo, elm_entry_utf8_to_markup(*String::Utf8Value(val)));
}

Handle<Value> CElmObject::padding_get() const
{
   Evas_Coord l, r, t, b;

   evas_object_size_hint_padding_get (eo, &l, &r, &t, &b);

   Local<Object> obj = Object::New();
   obj->Set(String::New("left"), Number::New(l));
   obj->Set(String::New("right"), Number::New(r));
   obj->Set(String::New("top"), Number::New(t));
   obj->Set(String::New("bottom"), Number::New(b));

   return obj;
}

void CElmObject::padding_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;
   Local<Object> obj = val->ToObject();
   Local<Value> left = obj->Get(String::New("left"));
   Local<Value> right = obj->Get(String::New("right"));
   Local<Value> top = obj->Get(String::New("top"));
   Local<Value> bottom = obj->Get(String::New("bottom"));
   evas_object_size_hint_padding_set (eo, left->Int32Value(),
                                      right->Int32Value(), top->Int32Value(),
                                      bottom->Int32Value());
}

Handle<Value> CElmObject::pointer_mode_get() const
{
   const char *mode_to_string[] = { "autograb", "nograb", "nograb-norepeat-updown" };
   return String::New(mode_to_string[evas_object_pointer_mode_get(eo)]);
}

void CElmObject::pointer_mode_set(Handle<Value> val)
{
   if (!val->IsString())
     return;

   String::Utf8Value newmode(val);
   Evas_Object_Pointer_Mode mode;
   if (!strcmp(*newmode, "autograb"))
     mode = EVAS_OBJECT_POINTER_MODE_AUTOGRAB;
   else if (!strcmp(*newmode, "nograb"))
     mode = EVAS_OBJECT_POINTER_MODE_NOGRAB;
   else if (!strcmp(*newmode, "nograb-norepeat-updown"))
     mode = EVAS_OBJECT_POINTER_MODE_NOGRAB_NO_REPEAT_UPDOWN;
   else
     return;

   evas_object_pointer_mode_set(eo, mode);
}

Handle<Value> CElmObject::antialias_get() const
{
   return Boolean::New(evas_object_anti_alias_get(eo));
}

void CElmObject::antialias_set(Handle<Value> val)
{
   if (val->IsBoolean())
     evas_object_anti_alias_set(eo, val->BooleanValue());
}

Handle<Value> CElmObject::static_clip_get() const
{
   return Boolean::New(evas_object_static_clip_get(eo));
}

void CElmObject::static_clip_set(Handle<Value> val)
{
   if (val->IsBoolean())
     evas_object_static_clip_set(eo, val->BooleanValue());
}

Handle<Value> CElmObject::size_hint_aspect_get() const
{
   Local<Object> obj = Object::New();
   Evas_Aspect_Control aspect;
   Evas_Coord w, h;

   evas_object_size_hint_aspect_get(eo, &aspect, &w, &h);
   const char *aspect_to_string[] = { "none", "neither", "horizontal",
                                      "vertical", "both" };
   obj->Set(String::NewSymbol("aspect"),
            String::NewSymbol(aspect_to_string[aspect]));
   obj->Set(String::NewSymbol("width"), Number::New(w));
   obj->Set(String::NewSymbol("height"), Number::New(h));

   return obj;
}

void CElmObject::size_hint_aspect_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;

   Local<Object> obj = val->ToObject();
   Local<Value> w = obj->Get(String::New("width"));
   Local<Value> h = obj->Get(String::New("height"));

   Evas_Aspect_Control aspect;
   Local<Value> aspectValue = obj->Get(String::New("aspect"));
   if (!aspectValue->IsString())
     aspect = EVAS_ASPECT_CONTROL_NONE;
   else
     {
        String::Utf8Value a(aspectValue->ToString());
        if (!strcmp(*a, "horizontal"))
          aspect = EVAS_ASPECT_CONTROL_HORIZONTAL;
        else if (!strcmp(*a, "vertical"))
          aspect = EVAS_ASPECT_CONTROL_VERTICAL;
        else if (!strcmp(*a, "both"))
          aspect = EVAS_ASPECT_CONTROL_BOTH;
        else
          aspect = EVAS_ASPECT_CONTROL_NEITHER;
     }

   evas_object_size_hint_aspect_set(eo, aspect, w->Int32Value(),
                                    h->Int32Value());
}

Handle<Value> CElmObject::name_get() const
{
   const char *s = evas_object_name_get(eo);
   return s ? String::New(s) : Undefined();
}

void CElmObject::name_set(Handle<Value> val)
{
   if (val->IsString())
     evas_object_name_set(eo, *String::Utf8Value(val));
}

Handle<Value> CElmObject::resize_get() const
{
   return Boolean::New(cached.isResize);
}

void CElmObject::resize_set(Handle<Value> val)
{
   if (!val->IsBoolean())
     return;

   Evas_Object *top = elm_object_top_widget_get(eo);
   if (!top)
     return;

   cached.isResize = val->BooleanValue();
   if (cached.isResize)
     elm_win_resize_object_add(top, eo);
   else
     elm_win_resize_object_del(top, eo);
}

Handle<Value> CElmObject::pointer_get() const
{
   Evas_Coord x, y;

   evas_pointer_canvas_xy_get(evas_object_evas_get(eo), &x, &y);
   Local<Object> obj = Object::New();
   obj->Set(String::New("x"), Integer::New(x));
   obj->Set(String::New("y"), Integer::New(y));

   return obj;
}

void CElmObject::pointer_set(Handle<Value>)
{
}

void CElmObject::OnAnimate()
{
   Handle<Function> callback(Function::Cast(*cb.animate));
   Handle<Value> arguments[1] = { jsObject };
   callback->Call(jsObject, 1, arguments);
}

Eina_Bool CElmObject::OnAnimateWrapper(void *data)
{
   static_cast<CElmObject *>(data)->OnAnimate();
   return ECORE_CALLBACK_RENEW;
}

Handle<Value> CElmObject::on_animate_get() const
{
   return cb.animate;
}

void CElmObject::on_animate_set(Handle<Value> val)
{
   if (!cb.animate.IsEmpty())
     {
        ecore_animator_del(current_animator);
        current_animator = NULL;
        cb.animate.Dispose();
        cb.animate.Clear();
     }

   if (!val->IsFunction())
     return;

   cb.animate = Persistent<Value>::New(val);
   current_animator = ecore_animator_add(&OnAnimateWrapper, this);
}

void CElmObject::OnClick(void *event_info)
{
   Handle<Function> callback(Function::Cast(*cb.click));

   if (event_info)
     {
        Evas_Event_Mouse_Down *ev = static_cast<Evas_Event_Mouse_Down*>(event_info);
        Handle<Value> args[3] = { jsObject, Number::New(ev->canvas.x),
                                  Number::New(ev->canvas.y) };
        callback->Call(jsObject, 3, args);
     }
   else
     {
        Handle<Value> args[1] = { jsObject };
        callback->Call(jsObject, 1, args);
     }
}

void CElmObject::OnClickWrapper(void *data, Evas_Object *, void *event_info)
{
   static_cast<CElmObject*>(data)->OnClick(event_info);
}

Handle<Value> CElmObject::on_click_get() const
{
   return cb.click;
}

void CElmObject::on_click_set(Handle<Value> val)
{
   if (!cb.click.IsEmpty())
     {
        evas_object_smart_callback_del(eo, "clicked", &OnClickWrapper);
        cb.click.Dispose();
        cb.click.Clear();
     }

   if (!val->IsFunction())
     return;

   cb.click = Persistent<Value>::New(val);
   evas_object_smart_callback_add(eo, "clicked", &OnClickWrapper, this);
}

void CElmObject::OnKeyDown(Evas_Event_Key_Down *event)
{
   Handle<Function> callback(Function::Cast(*cb.key_down));
   Handle<Value> args[2] = { jsObject, String::New(event->keyname) };
   callback->Call(jsObject, 2, args);
}

void CElmObject::OnKeyDownWrapper(void *data, Evas *, Evas_Object *, void *event_info)
{
   static_cast<CElmObject *>(data)->OnKeyDown(static_cast<Evas_Event_Key_Down *>(event_info));
}

Handle<Value> CElmObject::on_key_down_get() const
{
   return cb.key_down;
}

void CElmObject::on_key_down_set(Handle<Value> val)
{
   if (!cb.key_down.IsEmpty())
     {
        evas_object_event_callback_del(eo, EVAS_CALLBACK_KEY_DOWN,
                                       &OnKeyDownWrapper);
        cb.key_down.Dispose();
        cb.key_down.Clear();
     }

   if (!val->IsFunction())
     return;

   cb.key_down = Persistent<Value>::New(val);
   evas_object_event_callback_add(eo, EVAS_CALLBACK_KEY_DOWN,
                                  &OnKeyDownWrapper, this);
}

Local<Object> CElmObject::Realise(Handle<Value> descValue, Handle<Value> parent)
{
   HandleScope scope;

   Local<String> type_str = String::NewSymbol("type");
   Local<Object> desc = descValue->ToObject();
   Local<Object> realised;

   if (tmpl->HasInstance(desc))
     return scope.Close(desc);

   if (desc->GetHiddenValue(type_str).IsEmpty())
     {
        realised = desc->Clone();
     }
   else
     {
        Handle<Value> params[] = { desc, parent };
        Local<Value> func = desc->GetHiddenValue(type_str);
        realised = Local<Function>::Cast(func)->NewInstance(2, params);

        Local<Array> props = desc->GetOwnPropertyNames();
        for (unsigned int i = 0; i < props->Length(); i++)
          {
             Local<String> key = props->Get(i)->ToString();
             Local<Value> val = desc->Get(key);
             realised->Set(key, val);
          }

        Local<String> visible = String::NewSymbol("visible");
        if (desc->Get(visible)->IsUndefined())
          realised->Set(visible, Boolean::New(true));
     }

   if (!parent->IsUndefined())
     {
        realised->SetHiddenValue(String::NewSymbol("elm::packer"), parent);
        GetObjectFromJavascript(parent)->DidRealiseElement(realised);
     }

   return scope.Close(realised);
}

Handle<Value> CElmObject::Realise(const Arguments& args)
{
   if (args.Length() != 1)
     {
        ELM_ERR("Realise needs object description");
        return Undefined();
     }

   return Realise(args[0], Undefined());
}

}
