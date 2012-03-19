#include <Eina.h>
#include <Evas.h>

#include "elev8_elm.h"
#include "CEvasObject.h"

using namespace v8;

void CEvasObject::init_factory()
{
   constructor_map = eina_hash_string_superfast_new(NULL);
}

void CEvasObject::register_widget(const char *type, WidgetConstructor constructor)
{
   eina_hash_add(constructor_map, type, (void *)constructor);
}

CEvasObject *CEvasObject::make(const char *type, CEvasObject *parent, Local<Object> description)
{
   WidgetConstructor widget_factory = (WidgetConstructor)eina_hash_find(constructor_map, type);
   return widget_factory ? widget_factory(parent, description) : 0;
}

CEvasObject::CEvasObject() :
       eo(NULL),
       current_animator(NULL),
       is_resize(false),
     /* the NULL below is the end of a linked list built up in prop_handler's constructor */
       property_list_base(NULL),
       prop_handler(property_list_base)
{
}

void CEvasObject::construct(Evas_Object *_eo, Local<Object> obj)
{
   eo = _eo;
   assert(eo != NULL);

   evas_object_data_set(eo, "cppobj", this);

   Handle<Object> out = get_object();

   /* copy properties, one by one */
   Handle<Array> props = obj->GetOwnPropertyNames();
   for (unsigned int i = 0; i < props->Length(); i++)
     {
        Local<Value> name = props->Get(Integer::New(i));
        String::Utf8Value name_str(name);

        /* skip the type property */
        if (!strcmp(*name_str, "type"))
          continue;

        Local<Value> value = obj->Get(name->ToString());
        init_property(out, name, value);
     }

   Local<Value> val = obj->Get(String::New("visible"));
   if ((val->IsTrue()) || (val->IsUndefined()))
     show(true);
   else
     show(false);
}

CEvasObject *CEvasObject::get_parent() const
{
   CEvasObject *parent = NULL;
   Evas_Object *win;

   win = elm_object_parent_widget_get(eo);
   if (win)
     {
        void *data = evas_object_data_get(win, "cppobj");
        if (data)
          parent = reinterpret_cast<CEvasObject*>(data);
     }
   return parent;
}

void CEvasObject::object_set_eo(Handle<Object> obj, CEvasObject *leo)
{
   obj->Set(String::New("_eo"), External::Wrap(leo));
}

CEvasObject *CEvasObject::eo_from_info(Handle<Object> obj)
{
   Handle<Value> val = obj->Get(String::New("_eo"));
   return static_cast<CEvasObject *>(External::Unwrap(val));
}

void CEvasObject::eo_setter(Local<String> property, Local<Value> value, const AccessorInfo& info)
{
   CEvasObject *eo = eo_from_info(info.This());
   String::Utf8Value prop_name(property);
   eo->prop_set(*prop_name, value);
   String::Utf8Value val(value->ToString());
}

Handle<Value> CEvasObject::eo_getter(Local<String> property, const AccessorInfo& info)
{
   CEvasObject *eo = eo_from_info(info.This());
   String::Utf8Value prop_name(property);
   return eo->prop_get(*prop_name);
}

/* setup the property on construction */
void CEvasObject::init_property(Handle<Object> out, Handle<Value> name, Handle<Value> value)
{
   String::Utf8Value name_str(name);

   /* set or copy the property */
   if (!prop_set(*name_str, value))
     out->Set(name, value);
}

Handle<Object> CEvasObject::get_object(void)
{
   if (the_object.IsEmpty())
     {
        Handle<ObjectTemplate> ot = get_template();
        the_object = Persistent<Object>::New(ot->NewInstance());
        object_set_eo(the_object, this);

        /* FIXME: make handle a weak handle, and detect destruction */
     }
   return the_object;
}

Handle<ObjectTemplate> CEvasObject::get_template(void)
{
   /* FIXME: only need to create one template per object class */
   the_template = Persistent<ObjectTemplate>::New(ObjectTemplate::New());

   property_list_base->fill_template(the_template);

   return the_template;
}

CEvasObject::~CEvasObject()
{
   evas_object_unref(eo);
   the_object.Dispose();
   the_template.Dispose();
   on_animate_val.Dispose();
   on_clicked_val.Dispose();
   on_keydown_val.Dispose();
   eo = NULL;
}

void CEvasObject::x_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        Evas_Coord x, y, width, height;
        evas_object_geometry_get(eo, &x, &y, &width, &height);
        x = val->ToInt32()->Value();
        evas_object_move(eo, x, y);
     }
}

Handle<Value> CEvasObject::x_get(void) const
{
   Evas_Coord x, y, width, height;
   evas_object_geometry_get(eo, &x, &y, &width, &height);
   return Number::New(x);
}

void CEvasObject::y_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        Evas_Coord x, y, width, height;
        evas_object_geometry_get(eo, &x, &y, &width, &height);
        y = val->ToInt32()->Value();
        evas_object_move(eo, x, y);
     }
}

Handle<Value> CEvasObject::y_get(void) const
{
   Evas_Coord x, y, width, height;
   evas_object_geometry_get(eo, &x, &y, &width, &height);
   return Number::New(y);
}

void CEvasObject::height_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        Evas_Coord x, y, width, height;
        evas_object_geometry_get(eo, &x, &y, &width, &height);
        height = val->ToInt32()->Value();
        evas_object_resize(eo, width, height);
     }
}

Handle<Value> CEvasObject::height_get(void) const
{
   Evas_Coord x, y, width, height;
   evas_object_geometry_get(eo, &x, &y, &width, &height);
   return Number::New(height);
}

void CEvasObject::width_set(Handle<Value> val)
{
   if (val->IsNumber())
     {
        Evas_Coord x, y, width, height;
        evas_object_geometry_get(eo, &x, &y, &width, &height);
        width = val->ToInt32()->Value();
        evas_object_resize(eo, width, height);
     }
}

Handle<Value> CEvasObject::width_get(void) const
{
   Evas_Coord x, y, width, height;
   evas_object_geometry_get(eo, &x, &y, &width, &height);
   return Number::New(width);
}

void CEvasObject::move(Local<Value> x, Local<Value> y)
{
   if (x->IsNumber() && y->IsNumber())
     evas_object_move(eo, x->Int32Value(), y->Int32Value());
}

void CEvasObject::on_click(void *event_info)
{
   Handle<Object> obj = get_object();
   HandleScope handle_scope;
   Handle<Value> val = on_clicked_val;

   // also provide x and y positions where it was clicked
   //
   if (event_info!=NULL)
     {
        Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down *)event_info;
        Handle<Number> ox = Number::New(ev->canvas.x);
        Handle<Number> oy = Number::New(ev->canvas.y);
        // FIXME: pass event_info to the callback
        // FIXME: turn the pieces below into a do_callback method
        Handle<Value> args[3] = { obj, ox, oy };
        assert(val->IsFunction());
        Handle<Function> fn(Function::Cast(*val));
        fn->Call(obj, 3, args);
     }
   else
     {
        // FIXME: pass event_info to the callback
        // FIXME: turn the pieces below into a do_callback method
        Handle<Value> args[1] = { obj };
        assert(val->IsFunction());
        Handle<Function> fn(Function::Cast(*val));
        fn->Call(obj, 1, args);
     }

}

void CEvasObject::eo_on_click(void *data, Evas_Object *, void *event_info)
{
   CEvasObject *clicked = static_cast<CEvasObject*>(data);

   clicked->on_click(event_info);
}

void CEvasObject::on_animate_set(Handle<Value> val)
{
   on_animate_val.Dispose();
   on_animate_val = Persistent<Value>::New(val);
   if (val->IsFunction())
     current_animator = ecore_animator_add(&eo_on_animate, this);
   else if (current_animator)
     {
        ecore_animator_del(current_animator);
        current_animator = NULL;
     }
}

Handle<Value> CEvasObject::on_animate_get(void) const
{
   return on_animate_val;
}

void CEvasObject::on_clicked_set(Handle<Value> val)
{
   on_clicked_val.Dispose();
   on_clicked_val = Persistent<Value>::New(val);
   if (val->IsFunction())
     evas_object_smart_callback_add(eo, "clicked", &eo_on_click, this);
   else
     evas_object_smart_callback_del(eo, "clicked", &eo_on_click);
}

Handle<Value> CEvasObject::on_clicked_get(void) const
{
   return on_clicked_val;
}

void CEvasObject::on_keydown(Evas_Event_Key_Down *info)
{
   Handle<Object> obj = get_object();
   HandleScope handle_scope;
   Handle<Value> val = on_keydown_val;
   // FIXME: pass event_info to the callback
   // FIXME: turn the pieces below into a do_callback method
   assert(val->IsFunction());
   Handle<Function> fn(Function::Cast(*val));
   Handle<Value> args[2] = { obj, v8::String::New(info->keyname)};
   fn->Call(obj, 2,args);
}

void CEvasObject::eo_on_keydown(void *data, Evas *, Evas_Object *, void *event_info)
{
   CEvasObject *self = static_cast<CEvasObject*>(data);

   self->on_keydown(static_cast<Evas_Event_Key_Down*>(event_info));
}

void CEvasObject::on_keydown_set(Handle<Value> val)
{
   on_keydown_val.Dispose();
   on_keydown_val = Persistent<Value>::New(val);
   if (val->IsFunction())
     evas_object_event_callback_add(eo, EVAS_CALLBACK_KEY_DOWN, &eo_on_keydown, this);
   else
     evas_object_event_callback_del(eo, EVAS_CALLBACK_KEY_DOWN, &eo_on_keydown);
}

Handle<Value> CEvasObject::on_keydown_get(void) const
{
   return on_keydown_val;
}

void CEvasObject::on_animate(void)
{
   Handle<Object> obj = get_object();
   HandleScope handle_scope;
   Handle<Value> val = on_animate_val;
   assert(val->IsFunction());
   Handle<Function> fn(Function::Cast(*val));
   Handle<Value> args[1] = { obj };
   fn->Call(obj, 1, args);
}

Eina_Bool CEvasObject::eo_on_animate(void *data)
{
   CEvasObject *clicked = static_cast<CEvasObject*>(data);

   clicked->on_animate();

   return ECORE_CALLBACK_RENEW;
}

Handle<Value> CEvasObject::label_get() const
{
   return String::New(elm_object_text_get(eo));
}

Handle<Value> CEvasObject::text_get() const
{
   return label_get();
}

void CEvasObject::label_set(Handle<Value> val)
{
   if (val->IsString() || val->IsNumber())
     {
        String::Utf8Value str(val);
        label_set(*str);
     }
}

void CEvasObject::text_set(Handle<Value> val)
{
   label_set(val);
}

void CEvasObject::label_set(const char *str)
{
   elm_object_text_set(eo, elm_entry_utf8_to_markup(str));
}

Handle<Value> CEvasObject::CEvasObject::disabled_get() const
{
   return Boolean::New(elm_object_disabled_get(eo));
}

void CEvasObject::disabled_set(Handle<Value> val)
{
   if (val->IsBoolean())
     elm_object_disabled_set(eo, val->BooleanValue());
}

Handle<Value> CEvasObject::scale_get() const
{
   return Number::New(elm_object_scale_get(eo));
}

void CEvasObject::scale_set(Handle<Value> val)
{
   if (val->IsNumber())
     elm_object_scale_set(eo, val->NumberValue());
}

bool CEvasObject::get_xy_from_object(Handle<Value> val, double &x_out, double &y_out)
{
   HandleScope handle_scope;
   if (!val->IsObject())
     return false;
   Local<Object> obj = val->ToObject();
   Local<Value> x = obj->Get(String::New("x"));
   Local<Value> y = obj->Get(String::New("y"));
   if (!x->IsNumber() || !y->IsNumber())
     return false;
   x_out = x->NumberValue();
   y_out = y->NumberValue();
   return true;
}

bool CEvasObject::get_xy_from_object(Handle<Value> val, bool &x_out, bool &y_out)
{
   HandleScope handle_scope;
   if (!val->IsObject())
     return false;
   Local<Object> obj = val->ToObject();
   Local<Value> x = obj->Get(String::New("x"));
   Local<Value> y = obj->Get(String::New("y"));
   if (!x->IsBoolean() || !y->IsBoolean())
     return false;
   x_out = x->BooleanValue();
   y_out = y->BooleanValue();
   return true;
}

bool CEvasObject::get_xy_from_object(Handle<Value> val, int &x_out, int &y_out)
{
   HandleScope handle_scope;
   if (!val->IsObject())
     return false;
   Local<Object> obj = val->ToObject();
   Local<Value> x = obj->Get(String::New("x"));
   Local<Value> y = obj->Get(String::New("y"));
   if (!x->IsInt32() || !y->IsInt32())
     return false;
   x_out = x->Int32Value();
   y_out = y->Int32Value();
   return true;
}

bool CEvasObject::get_xy_from_object(Handle<Value> val,
                        Handle<Value> &x_val,
                        Handle<Value> &y_val)
{
   HandleScope handle_scope;
   if (!val->IsObject())
     return false;
   Local<Object> obj = val->ToObject();
   x_val = obj->Get(String::New("x"));
   y_val = obj->Get(String::New("y"));
   return true;
}

void CEvasObject::weight_set(Handle<Value> weight)
{
   double x, y;
   if (get_xy_from_object(weight, x, y))
     evas_object_size_hint_weight_set(eo, x, y);
}

Handle<Value> CEvasObject::weight_get(void) const
{
   double x = 0.0, y = 0.0;
   evas_object_size_hint_weight_get(eo, &x, &y);
   Local<Object> obj = Object::New();
   obj->Set(String::New("x"), Number::New(x));
   obj->Set(String::New("y"), Number::New(y));
   return obj;
}

void CEvasObject::align_set(Handle<Value> align)
{
   double x, y;
   if (get_xy_from_object(align, x, y))
     {
        evas_object_size_hint_align_set(eo, x, y);
     }
}

Handle<Value> CEvasObject::align_get(void) const
{
   double x, y;
   evas_object_size_hint_align_get(eo, &x, &y);
   Local<Object> obj = Object::New();
   obj->Set(String::New("x"), Number::New(x));
   obj->Set(String::New("y"), Number::New(y));
   return obj;
}

void CEvasObject::image_set(Handle<Value> val)
{
   if (val->IsString())
     ELM_ERR( "no image set");
}

Handle<Value> CEvasObject::image_get(void) const
{
   return Undefined();
}

void CEvasObject::show(bool enabled)
{
   if (enabled)
     {
        evas_object_show(eo);
     }
   else
     evas_object_hide(eo);
}

/* returns a list of children in an object */
Handle<Object> CEvasObject::realize_objects(Handle<Value> val, Handle<Object> &out)
{
   if (!val->IsObject())
     {
        ELM_ERR( "not an object!");
        return out;
     }

   Handle<Object> in = val->ToObject();
   Handle<Array> props = in->GetOwnPropertyNames();

   /* iterate through elements and instantiate them */
   for (unsigned int i = 0; i < props->Length(); i++)
     {

        Handle<Value> x = props->Get(Integer::New(i));

        CEvasObject *child = make_or_get(this, in->Get(x->ToString()));
        if (!child)
          continue;
        add_child(child);

        Handle<Object> child_obj = child->get_object();
        out->Set(x, child_obj);
     }

   return out;
}

/* resize this object when the parent resizes? */
void CEvasObject::resize_set(Handle<Value> val)
{
   if (val->IsBoolean())
     {
        Evas_Object *parent = elm_object_parent_widget_get(eo);
        if (!parent)
          ELM_ERR( "resize object has no parent!");
        else
          {
             is_resize = val->BooleanValue();
             if (is_resize)
               elm_win_resize_object_add(parent, eo);
             else
               elm_win_resize_object_del(parent, eo);
          }
     }
   else
     ELM_ERR( "Resize value not boolean!");
}

Handle<Value> CEvasObject::resize_get(void) const
{
   return Boolean::New(is_resize);
}

Handle<Value> CEvasObject::pointer_get(void) const
{
   Evas_Coord x, y;
   evas_pointer_canvas_xy_get(evas_object_evas_get(eo), &x, &y);
   Local<Object> obj = Object::New();
   obj->Set(String::New("x"), Integer::New(x));
   obj->Set(String::New("y"), Integer::New(y));
   return obj;
}

void CEvasObject::style_set(Handle<Value> val)
{
   if (val->IsString())
     {
        String::Utf8Value str(val);
        elm_object_style_set(eo, *str);
     }
}

Handle<Value> CEvasObject::style_get(void) const
{
   return String::New(elm_object_style_get(eo));
}

void CEvasObject::visible_set(Handle<Value> val)
{
   if (val->IsBoolean())
     {
        if (val->BooleanValue())
          evas_object_show(eo);
        else
          evas_object_hide(eo);
     }
}

Handle<Value> CEvasObject::visible_get(void) const
{
   return Boolean::New(evas_object_visible_get(eo));
}

void CEvasObject::hint_min_set(Handle<Value> val)
{
   if (!val->IsObject())
     return ;
   Evas_Coord width, height;
   Local<Object> obj = val->ToObject();
   Local<Value> w = obj->Get(String::New("x"));
   Local<Value> h = obj->Get(String::New("y"));
   if (!w->IsInt32() || !h->IsInt32())
     return;
   width = w->Int32Value();
   height = h->Int32Value();
   evas_object_size_hint_min_set (eo,  width, height);
}

Handle<Value> CEvasObject::hint_min_get(void) const
{
   Evas_Coord w, h;
   evas_object_size_hint_min_get (eo,  &w, &h);
   Local<Object> obj = Object::New();
   obj->Set(String::New("w"), Number::New(w));
   obj->Set(String::New("h"), Number::New(h));
   return obj;
}

void CEvasObject::hint_max_set(Handle<Value> val)
{
   Evas_Coord width, height;
   Local<Object> obj = val->ToObject();
   Local<Value> w = obj->Get(String::New("x"));
   Local<Value> h = obj->Get(String::New("y"));
   if (!w->IsInt32() || !h->IsInt32())
     return;
   width = w->Int32Value();
   height = h->Int32Value();
   evas_object_size_hint_max_set (eo, width, height);
}

Handle<Value> CEvasObject::hint_max_get(void) const
{
   Evas_Coord w, h;
   evas_object_size_hint_max_get (eo,  &w, &h);
   Local<Object> obj = Object::New();
   obj->Set(String::New("w"), Number::New(w));
   obj->Set(String::New("h"), Number::New(h));
   return obj;
}

void CEvasObject::focus_set(Handle<Value> val)
{
   if (val->IsBoolean())
     {
        evas_object_focus_set(eo, val->BooleanValue());
     }
}

Handle<Value> CEvasObject::focus_get(void) const
{
   return Boolean::New(evas_object_focus_get(eo));
}

Handle<Value> CEvasObject::layer_get() const
{
   return Number::New(evas_object_layer_get(eo));
}

void CEvasObject::layer_set(Handle<Value> val)
{
   if (val->IsNumber())
     evas_object_layer_set(eo, val->NumberValue());
}

void CEvasObject::name_set(Handle<Value> val)
{
   if (val->IsString())
     {
        String::Utf8Value str(val);
        evas_object_name_set(eo, *str);
     }
}

Handle<Value> CEvasObject::name_get(void) const
{
   return String::New(evas_object_name_get(eo));
}

void CEvasObject::hint_req_set(Handle<Value> val)
{
   if (!val->IsObject())
     return ;
   Evas_Coord width, height;
   Local<Object> obj = val->ToObject();
   Local<Value> w = obj->Get(String::New("x"));
   Local<Value> h = obj->Get(String::New("y"));
   if (!w->IsInt32() || !h->IsInt32())
     return;
   width = w->Int32Value();
   height = h->Int32Value();
   evas_object_size_hint_request_set (eo,  width, height);
}

Handle<Value> CEvasObject::hint_req_get(void) const
{
   Evas_Coord w, h;
   evas_object_size_hint_request_get (eo,  &w, &h);
   Local<Object> obj = Object::New();
   obj->Set(String::New("w"), Number::New(w));
   obj->Set(String::New("h"), Number::New(h));
   return obj;
}

void CEvasObject::padding_set(Handle<Value> val)
{
   if (!val->IsObject())
     return ;
   Evas_Coord l, r, t, b;
   Local<Object> obj = val->ToObject();
   Local<Value> ll = obj->Get(String::New("l"));
   Local<Value> rr = obj->Get(String::New("r"));
   Local<Value> tt = obj->Get(String::New("t"));
   Local<Value> bb = obj->Get(String::New("b"));
   l = ll->Int32Value();
   r = rr->Int32Value();
   t = tt->Int32Value();
   b = bb->Int32Value();
   evas_object_size_hint_padding_set (eo, l, r, t, b);
}

Handle<Value> CEvasObject::padding_get(void) const
{
   Evas_Coord l, r, t, b;
   evas_object_size_hint_padding_get (eo,  &l, &r, &t, &b);
   Local<Object> obj = Object::New();
   obj->Set(String::New("l"), Number::New(l));
   obj->Set(String::New("r"), Number::New(r));
   obj->Set(String::New("t"), Number::New(t));
   obj->Set(String::New("b"), Number::New(b));
   return obj;
}

Handle<Value> CEvasObject::pointer_mode_get() const
{
   return Number::New(evas_object_pointer_mode_get(eo));
}

void CEvasObject::pointer_mode_set(Handle<Value> val)
{
   if (val->IsNumber())
     evas_object_pointer_mode_set(eo, (Evas_Object_Pointer_Mode)val->NumberValue());
}

void CEvasObject::antialias_set(Handle<Value> val)
{
   if (val->IsBoolean())
     {
        evas_object_anti_alias_set(eo, val->BooleanValue());
     }
}

Handle<Value> CEvasObject::antialias_get(void) const
{
   return Boolean::New(evas_object_anti_alias_get(eo));
}

void CEvasObject::static_clip_set(Handle<Value> val)
{
   if (val->IsBoolean())
     {
        evas_object_static_clip_set(eo, val->BooleanValue());
     }
}

Handle<Value> CEvasObject::static_clip_get(void) const
{
   return Boolean::New(evas_object_static_clip_get(eo));
}

void CEvasObject::size_hint_aspect_set(Handle<Value> val)
{
   if (!val->IsObject())
     return ;
   Evas_Aspect_Control a;
   Evas_Coord w, h;
   Local<Object> obj = val->ToObject();
   Local<Value> aa = obj->Get(String::New("a"));
   Local<Value> ww = obj->Get(String::New("w"));
   Local<Value> hh = obj->Get(String::New("h"));
   a = (Evas_Aspect_Control)aa->Int32Value();
   w = ww->Int32Value();
   h = hh->Int32Value();
   evas_object_size_hint_aspect_set (eo, a, w, h);
}

Handle<Value> CEvasObject::size_hint_aspect_get(void) const
{
   Evas_Aspect_Control a;
   Evas_Coord w, h;
   evas_object_size_hint_aspect_get (eo,  &a, &w, &h);
   Local<Object> obj = Object::New();
   obj->Set(String::New("a"), Number::New(a));
   obj->Set(String::New("w"), Number::New(w));
   obj->Set(String::New("h"), Number::New(h));
   return obj;
}

Eina_Hash *CEvasObject::constructor_map;

PROPERTIES_OF(CEvasObject) = {
     PROP_HANDLER(CEvasObject, x),
     PROP_HANDLER(CEvasObject, y),
     PROP_HANDLER(CEvasObject, disabled),
     PROP_HANDLER(CEvasObject, width),
     PROP_HANDLER(CEvasObject, height),
     PROP_HANDLER(CEvasObject, image),
     PROP_HANDLER(CEvasObject, label),
     PROP_HANDLER(CEvasObject, text),
     PROP_HANDLER(CEvasObject, type),
     PROP_HANDLER(CEvasObject, resize),
     PROP_HANDLER(CEvasObject, align),
     PROP_HANDLER(CEvasObject, weight),
     PROP_HANDLER(CEvasObject, on_animate),
     PROP_HANDLER(CEvasObject, on_clicked),
     PROP_HANDLER(CEvasObject, on_keydown),
     PROP_HANDLER(CEvasObject, scale),
     PROP_HANDLER(CEvasObject, pointer),
     PROP_HANDLER(CEvasObject, style),
     PROP_HANDLER(CEvasObject, visible),
     PROP_HANDLER(CEvasObject, hint_min),
     PROP_HANDLER(CEvasObject, hint_max),
     PROP_HANDLER(CEvasObject, focus),
     PROP_HANDLER(CEvasObject, layer),
     PROP_HANDLER(CEvasObject, name),
     PROP_HANDLER(CEvasObject, hint_req),
     PROP_HANDLER(CEvasObject, padding),
     PROP_HANDLER(CEvasObject, pointer_mode),
     PROP_HANDLER(CEvasObject, antialias),
     PROP_HANDLER(CEvasObject, static_clip),
     PROP_HANDLER(CEvasObject, size_hint_aspect),
     { NULL }
};
