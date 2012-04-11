Persistent<FunctionTemplate> CElmObject::tmpl;
GENERATE_PROPERTY_CALLBACKS(CElmObject, align);
GENERATE_PROPERTY_CALLBACKS(CElmObject, antialias);
GENERATE_PROPERTY_CALLBACKS(CElmObject, enabled);
GENERATE_PROPERTY_CALLBACKS(CElmObject, focus);
GENERATE_PROPERTY_CALLBACKS(CElmObject, height);
GENERATE_PROPERTY_CALLBACKS(CElmObject, hint_max);
GENERATE_PROPERTY_CALLBACKS(CElmObject, hint_min);
GENERATE_PROPERTY_CALLBACKS(CElmObject, hint_req);
GENERATE_PROPERTY_CALLBACKS(CElmObject, layer);
GENERATE_PROPERTY_CALLBACKS(CElmObject, name);
GENERATE_PROPERTY_CALLBACKS(CElmObject, on_animate);
GENERATE_PROPERTY_CALLBACKS(CElmObject, on_clicked);
GENERATE_PROPERTY_CALLBACKS(CElmObject, on_key_down);
GENERATE_PROPERTY_CALLBACKS(CElmObject, padding);
GENERATE_PROPERTY_CALLBACKS(CElmObject, pointer);
GENERATE_PROPERTY_CALLBACKS(CElmObject, pointer_mode);
GENERATE_PROPERTY_CALLBACKS(CElmObject, size_hint_aspect);
GENERATE_PROPERTY_CALLBACKS(CElmObject, static_clip);
GENERATE_PROPERTY_CALLBACKS(CElmObject, visible);
GENERATE_PROPERTY_CALLBACKS(CElmObject, weight);
GENERATE_PROPERTY_CALLBACKS(CElmObject, width);
GENERATE_PROPERTY_CALLBACKS(CElmObject, x);
GENERATE_PROPERTY_CALLBACKS(CElmObject, y);

Handle<Value>
CElmObject::New(const Arguments& args)
{
   HandleScope scope;

   Local<Object> self = args.This();
   CElmObject *evas_object = new CElmObject;

   Persistent<Object>::New(self).MakeWeak(evas_object, Dispose);
   self->SetPointerInInternalField(0, evas_object);
   return Undefined();
}

void
CElmObject::Dispose(Persistent<Value> object, void *parameter)
{
   delete static_cast<CElmObject*>(parameter);
   object.Dispose();
   object.Clear();
}

Handle<FunctionTemplate>
CElmObject::GetTemplate()
{
   if (!tmpl.IsEmpty())
     return tmpl;

   HandleScope scope;
   tmpl = Persistent<FunctionTemplate>::New(FunctionTemplate::New(CElmObject::New));
   tmpl->InstanceTemplate()->SetInternalFieldCount(1);
   RegisterProperties(tmpl->PrototypeTemplate(),
      PROPERTY(x),
      PROPERTY(y),
      PROPERTY(enabled),
      PROPERTY(width),
      PROPERTY(height),
      PROPERTY(align),
      PROPERTY(weight),
      PROPERTY(on_animate),
      PROPERTY(on_clicked),
      PROPERTY(on_keydown),
      PROPERTY(pointer),
      PROPERTY(visible),
      PROPERTY(hint_min),
      PROPERTY(hint_max),
      PROPERTY(focus),
      PROPERTY(layer),
      PROPERTY(name),
      PROPERTY(hint_req),
      PROPERTY(padding),
      PROPERTY(pointer_mode),
      PROPERTY(antialias),
      PROPERTY(static_clip),
      PROPERTY(size_hint_aspect),
      NULL);

   return scope.Close(tmpl);
}

Handle<Value>
CElmObject::Getx() const
{
   int x;
   evas_object_geometry_get(evas_object, &x, NULL, NULL, NULL);
   return Integer::New(x);
}

void
CElmObject::Setx(Handle<Value> val)
{
   if (!val->IsNumber())
     return;
   int y;
   evas_object_geometry_get(evas_object, NULL, &y, NULL, NULL);
   evas_object_move(evas_object, val->ToInt32()->Value(), y);
}

Handle<Value>
CElmObject::Gety() const
{
   int y;
   evas_object_geometry_get(evas_object, NULL, &y, NULL, NULL);
   return Integer::New(y);
}

void
CElmObject::Sety(Handle<Value> val)
{
   if (!val->IsNumber())
     return;
   int x;
   evas_object_geometry_get(evas_object, &x, NULL, NULL, NULL);
   evas_object_move(evas_object, x, val->ToInt32()->Value());
}

void
CElmObject::Setenabled(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_object_disabled_set(evas_object, !val->BooleanValue());
}

Handle<Value>
CElmObject::Getenabled() const
{
   return Boolean::New(!elm_object_disabled_set(evas_object));
}

Handle<Value>
CElmObject::Getwidth() const
{
   int width;
   evas_object_geometry_get(evas_object, NULL, NULL, &width, NULL);
   return Integer::New(width);
}

void
CElmObject::Setwidth(Handle<Value> val)
{
   if (!val->IsNumber())
     return;
   int height;
   evas_object_geometry_get(evas_object, NULL, NULL, NULL, &height);
   evas_object_resize(evas_object, val->ToInt32()->Value(), height);
}

Handle<Value>
CElmObject::Getheight() const
{
   int height;
   evas_object_geometry_get(evas_object, NULL, NULL, NULL, &height);
   return Integer::New(height);
}

void
CElmObject::Setheight(Handle<Value> val)
{
   if (!val->IsNumber())
     return;
   int width;
   evas_object_geometry_get(evas_object, NULL, NULL, &width, NULL);
   evas_object_resize(evas_object, width, val->ToInt32()->Value());
}

Handle<Value>
CElmObject::Getalign() const
{
   HandleScope scope;
   double x, y;
   evas_object_size_hint_align_get(evas_object, &x, &y);
   Local<Object> align = Object::New();
   align->Set(String::NewSymbol("x"), Number::New(x));
   align->Set(String::NewSymbol("y"), Number::New(y));
   return scope.Close(align);
}

void
CElmObject::Setalign(Handle<Value> align)
{
   evas_object_size_hint_align_set(evas_object,
        align->Get(String::NewSymbol("x"))->ToNumber()->Value(),
        align->Get(String::NewSymbol("y"))->ToNumber()->Value());
}

Handle<Value>
CElmObject::Getweight() const
{
   HandleScope scope;
   double x, y;
   evas_object_size_hint_weight_get(evas_object, &x, &y);
   Local<Object> align = Object::New();
   align->Set(String::NewSymbol("x"), Number::New(x));
   align->Set(String::NewSymbol("y"), Number::New(y));
   return scope.Close(align);
}

void
CElmObject::Setweight(Handle<Value> align)
{
   evas_object_size_hint_weight_set(evas_object,
        align->Get(String::NewSymbol("x"))->ToNumber()->Value(),
        align->Get(String::NewSymbol("y"))->ToNumber()->Value());
}

Handle<Value>
CElmObject::Getvisible() const
{
   return Boolean::New(evas_object_visible_get(evas_object));
}

void
CElmObject::Setvisible(Handle<Value> val)
{
   ((val->IsBoolean() && val->BooleanValue()) ? evas_object_show : evas_object_hide)(evas_object);
}

void
CElmObject::Sethint_min(Handle<Value> val)
{
   if (!val->IsObject())
    return;
   Local<Object> obj = val->ToObject();
   Local<Value> w = obj->Get(String::New("x"));
   Local<Value> h = obj->Get(String::New("y"));
   if (w->IsInt32() && h->IsInt32())
     evas_object_size_hint_min_set(evas_object, w->Int32Value(), h->Int32Value(),);
}

Handle<Value>
CElmObject::Gethint_min() const
{
   HandleScope scope;
   Evas_Coord w, h;

   evas_object_size_hint_min_get(evas_object,  &w, &h);

   Local<Object> obj = Object::New();
   obj->Set(String::New("width"), Number::New(w));
   obj->Set(String::New("height"), Number::New(h));

   return scope.Close(obj);
}

void
CElmObject::Sethint_max(Handle<Value> val)
{
   Local<Object> obj = val->ToObject();
   Local<Value> w = obj->Get(String::New("x"));
   Local<Value> h = obj->Get(String::New("y"));
   if (w->IsInt32() && h->IsInt32())
     evas_object_size_hint_max_set(evas_object, w->Int32Value(), h->Int32Value());
}

Handle<Value>
CElmObject::Gethint_max() const
{
   HandleScope scope;
   Evas_Coord w, h;

   evas_object_size_hint_max_get(evas_object, &w, &h);

   Local<Object> obj = Object::New();
   obj->Set(String::New("width"), Number::New(w));
   obj->Set(String::New("height"), Number::New(h));

   return scope.Close(obj);
}

void
CElmObject::Setfocus(Handle<Value> val)
{
   if (val->IsBoolean())
     evas_object_focus_set(evas_object, val->BooleanValue());
}

Handle<Value>
CElmObject::Getfocus() const
{
   return Boolean::New(evas_object_focus_get(evas_object));
}

Handle<Value>
CElmObject::Getlayer() const
{
   return Number::New(evas_object_layer_get(evas_object));
}

void
CElmObject::Setlayer(Handle<Value> val)
{
   if (val->IsNumber())
     evas_object_layer_set(evas_object, val->NumberValue());
}

void
CElmObject::Sethint_req(Handle<Value> val)
{
   if (!val->IsObject())
     return;

   Local<Object> obj = val->ToObject();
   Local<Value> w = obj->Get(String::New("x"));
   Local<Value> h = obj->Get(String::New("y"));
   if (w->IsInt32() && h->IsInt32())
     evas_object_size_hint_request_set(evas_object, w->ToInt32(), h->ToInt32());
}

Handle<Value>
CElmObject::Gethint_req() const
{
   HandleScope scope;
   Evas_Coord w, h;

   evas_object_size_hint_request_get(evas_object, &w, &h);

   Local<Object> obj = Object::New();
   obj->Set(String::New("width"), Number::New(w));
   obj->Set(String::New("height"), Number::New(h));

   return scope.Close(obj);
}

void
CElmObject::padding_set(Handle<Value> val)
{
   if (!val->IsObject())
     return;
   Local<Object> obj = val->ToObject();
   Local<Value> left = obj->Get(String::New("left"));
   Local<Value> right = obj->Get(String::New("right"));
   Local<Value> top = obj->Get(String::New("top"));
   Local<Value> bottom = obj->Get(String::New("bottom"));
   evas_object_size_hint_padding_set (eo, left->ToInt32(), right->ToInt32(), top->ToInt32(), bottom->ToInt32());
}

Handle<Value>
CElmObject::Getpadding() const
{
   HandleScope scope;
   Evas_Coord l, r, t, b;

   evas_object_size_hint_padding_get (evas_object, &l, &r, &t, &b);

   Local<Object> obj = Object::New();
   obj->Set(String::New("left"), Number::New(l));
   obj->Set(String::New("right"), Number::New(r));
   obj->Set(String::New("top"), Number::New(t));
   obj->Set(String::New("bottom"), Number::New(b));

   return scope.Close(obj);
}

Handle<Value>
CElmObject::Getpointer_mode() const
{
   const char *mode_to_string[] = { "autograb", "nograb", "nograb-norepeat-updown" };
   return String::New(mode_to_string[evas_object_pointer_mode_get(evas_object)]);
}

void
CElmObject::Setpointer_mode(Handle<Value> val)
{
   if (!val->IsString())
     return;
   String::Utf8Value newMode(val->ToString());
   Evas_Object_Pointer_Mode mode;
   if (!strcmp(*newmode, "autograb"))
     mode = EVAS_OBJECT_POINTER_MODE_AUTOGRAB;
   else if (!strcmp(*newmode, "nograb"))
     mode = EVAS_OBJECT_POINTER_MODE_NOGRAB;
   else if (!strcmp(*newmode, "nograb-norepeat-updown"))
     mode = EVAS_OBJECT_POINTER_MODE_NOGRAB_NO_REPEAT_UPDOWN;
   else
     return;
   evas_object_pointer_mode_set(evas_object, mode);
}

void
CElmObject::Setantialias(Handle<Value> val)
{
   if (val->IsBoolean())
     evas_object_anti_alias_set(evas_object, val->BooleanValue());
}

Handle<Value>
CElmObject::Getantialias() const
{
   return Boolean::New(evas_object_anti_alias_get(evas_object));
}

void
CElmObject::Setstatic_clip(Handle<Value> val)
{
   if (val->IsBoolean())
     evas_object_static_clip_set(evas_object, val->BooleanValue());
}

Handle<Value>
CElmObject::Getstatic_clip() const
{
   return Boolean::New(evas_object_static_clip_get(evas_object));
}

void
CElmObject::Setsize_hint_aspect(Handle<Value> val)
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

   evas_object_size_hint_aspect_set(evas_object, aspect, w->Int32Value(), h->Int32Value());
}

Handle<Value>
CElmObject::Getsize_hint_aspect() const
{
   HandleScope scope;
   Local<Object> obj = Object::New();
   Evas_Aspect_Control aspect;
   Evas_Coord w, h;

   evas_object_size_hint_aspect_get(evas_object, &a, &w, &h);
   const char *aspect_to_string[] = { "none", "neither", "horizontal", "vertical", "both" };
   obj->Set(String::New("aspect"), String::New(aspect_to_string[a]));
   obj->Set(String::New("width"), Number::New(w));
   obj->Set(String::New("height"), Number::New(h));

   return scope.Close(obj);
}

void
CElmObject::Setname(Handle<Value> val)
{
   if (val->IsString())
     evas_object_name_set(evas_object, *String::Utf8Value(val));
}

Handle<Value>
CElmObject::Getname() const
{
   return String::New(evas_object_name_get(evas_object));
}

void
CElmObject::Setpointer(Handle<Value>)
{
}

Handle<Value>
CElmObject::Getpointer() const
{
   HandleScope scope;
   Evas_Coord x, y;

   evas_pointer_canvas_xy_get(evas_object_evas_get(evas_object), &x, &y);
   Local<Object> obj = Object::New();
   obj->Set(String::New("x"), Integer::New(x));
   obj->Set(String::New("y"), Integer::New(y));

   return scope.Close(obj);
}

void
CElmObject::OnAnimate()
{
   if (!on_animate.IsFunction())
     return;

   HandleScope scope;
   Handle<Object> obj = GetJSObject();
   Handle<Function> callback(Function::Cast(*on_animate));
   Handle<Value> arguments[1] = { obj };
   callback->Call(obj, 1, arguments);
}

void
CElmObject::OnAnimateWrapper(void *data)
{
   static_cast<CElmObject *>(data)->OnAnimate();
   return ECORE_CALLBACK_RENEW;
}

void
CElmObject::Seton_animate(Handle<Value> val)
{
   on_animate.Dispose();
   on_animate = Persistent<Value>::New(val);
   if (val->IsFunction())
     current_animator = ecore_animator_add(&OnAnimateWrapper, this);
   else if (current_animator)
     {
        ecore_animator_del(current_animator);
        current_animator = NULL;
     }
}

Handle<Value>
CElmObject::Geton_animate() const
{
   return on_animate;
}

void
CElmObject::OnClick(void *event_info)
{
   if (!on_click->IsFunction())
     return;

   HandleScope scope;
   Handle<Object> obj = GetJSObject();
   Handle<Function> callback(Function::Cast(*on_click));

   if (event_info)
     {
        Evas_Event_Mouse_Down *ev = static_cast<Evas_Event_Mouse_Down*>(event_info);
        Handle<Value> args[3] = { obj, Number::New(ev->canvas.x), Number::New(ev->canvas.y) };
        callback->Call(obj, 3, args);
     }
   else
     {
        Handle<Value> args[1] = obj;
        callback->Call(obj, 1, args);
     }
}

void
CElmObject::OnClickWrapper(void *data)
{
   static_cast<CElmObject*>(data)->OnClick();
}

void
CElmObject::Seton_click(Handle<Value> val)
{
   on_click.Dispose();
   on_click = Persistent<Value>::New(val);
   if (val->IsFunction())
     evas_object_smart_callback_add(evas_object, "clicked", &OnClickWrapper, this);
   else
     evas_object_smart_callback_del(evas_object, "clicked", &OnClickWrapper);
}

Handle<Value>
CElmObject::Geton_click() const
{
   return on_click;
}

void
CElmObject::OnKeyDown(Evas_Event_Key_Down *event)
{
   if (!on_key_down->IsFunction())
     return;

   HandleScope scope;
   Handle<Value> obj = GetJSObject();
   Handle<Function> callback(Function::Cast(*on_key_down));
   Handle<Value> args[2] = { obj, String::New(event->keyname) };
   callback->Call(obj, 2, args);
}

void
CElmObject::OnKeyDownWrapper(void *data, Evas *, Evas_Object *, void *event_info)
{
   static_cast<CElmObject *>(data)->OnKeyDown(static_cast<Evas_Event_Key_Down *>(event_info));
}

void
CElmObject::Seton_key_down(Handle<Value> val)
{
   on_key_down.Dispose();
   on_key_down = Persistent<Value>::New(val);
   if (val->IsFunction())
     evas_object_event_callback_add(evas_object, EVAS_CALLBACK_KEY_DOWN, &OnKeyDownWrapper, this);
   else
     evas_object_event_callback_del(evas_object, EVAS_CALLBACK_KEY_DOWN, &OnKeyDownWrapper);
}

Handle<Value>
CElmObject::Geton_key_down() const
{
   return on_key_down;
}
