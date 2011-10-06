/*
 * elev8 - javascript for EFL
 *
 * The script's job is to prepare for the main loop to run
 * then exit
 */

#include <Elementary.h>
#include <v8.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

using namespace v8;

// FIXME: split CElmObject from CEvasObject

/* CEvasObject is a virtual class, representing an evas object */
class CEvasObject;
CEvasObject *realize_one(CEvasObject *parent, Handle<Value> obj);

class CEvasObject {
   /* realize_one is a factory for our class */
   friend CEvasObject *realize_one(CEvasObject *parent, Handle<Value> obj);
protected:
   Evas_Object *eo;
   Persistent<ObjectTemplate> the_template;
   Persistent<Object> the_object;

   /*
    * Callbacks
    *
    * We could set every callback for every object, then check for existence
    * of the relevant callback function, then call it.  That would mean
    * a lot of callbacks from evas for functions that don't exist.
    *
    * Instead, manage setting and getting of the callback by ourselves.
    * When a callback is set, the apropriate Evas callback will be set or cleared.
    */

   /* the on_clicked function */
   Persistent<Value> on_clicked_val;

   /* function to call when a key is pressed */
   Persistent<Value> on_keydown_val;

   /* the animator function and its hook into ecore */
   Persistent<Value> on_animate_val;
   Ecore_Animator *current_animator;

   /* object is resized to the size of the parent (usually the main window) */
   bool is_resize;

   /*
    * List of properties that can be got and set.
    * It's a template because we don't want all properties to be in CEvasObject.
    * The idea is to allow declaring a list of properties
    * that can be get and set, then the methods to do the setting
    * and leave the rest up to this template.
    */
   template<class X> class CPropHandler {
   public:
     typedef Handle<Value> (X::*prop_getter)(void) const;
     typedef void (X::*prop_setter)(Handle<Value> val);

   private:
     struct property_list {
       const char *name;
       prop_getter get;
       prop_setter set;
     };

     static property_list list[];
   public:
     /*
      * the get method to get a property we know about
      */
     prop_getter get_getter(const char *prop_name) const
       {
          for (property_list *prop = list; prop->name; prop++)
            {
               if (!strcmp(prop->name, prop_name))
                 {
                    return prop->get;
                 }
            }
          return NULL;
       }

     /*
      * the get method to set a property we know about
      */
     prop_setter get_setter(const char *prop_name)
       {
          for (property_list *prop = list; prop->name; prop++)
            {
               if (!strcmp(prop->name, prop_name))
                 {
                    return prop->set;
                 }
            }
          return NULL;
       }

     /*
      * Add an interceptor on a property on the given V8 object template
      */
     void fill_template(Handle<ObjectTemplate> &ot)
       {
          for (property_list *prop = list; prop->name; prop++)
            ot->SetAccessor(String::New(prop->name), &eo_getter, &eo_setter);
       }
   };

#define PROP_HANDLER(cls, foo) { #foo, &cls::foo##_get, &cls::foo##_set }
   static CPropHandler<CEvasObject> prop_handler;

protected:
   explicit CEvasObject() :
       eo(NULL),
       current_animator(NULL),
       is_resize(false)
     {
     }

   /*
    * Two phase constructor required because Evas_Object type needs
    * to be known to be created.
    */
   void construct(Evas_Object *_eo, Local<Object> obj)
     {
        eo = _eo;
        assert(eo != NULL);

        evas_object_data_set(eo, "cppobj", this);

        Handle<Object> out = get_object();

        /* copy properties, one by one */
        Handle<Array> props = obj->GetPropertyNames();
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

   virtual void add_child(CEvasObject *child)
     {
     }

   CEvasObject *get_parent() const
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

   void object_set_eo(Handle<Object> obj, CEvasObject *eo)
     {
        obj->Set(String::New("_eo"), External::Wrap(eo));
     }

   static CEvasObject *eo_from_info(Handle<Object> obj)
     {
        Handle<Value> val = obj->Get(String::New("_eo"));
        return static_cast<CEvasObject *>(External::Unwrap(val));
     }

   static void eo_setter(Local<String> property,
                         Local<Value> value,
                         const AccessorInfo& info)
     {
        CEvasObject *eo = eo_from_info(info.This());
        String::Utf8Value prop_name(property);
        eo->prop_set(*prop_name, value);
        String::Utf8Value val(value->ToString());
     }

   static Handle<Value> eo_getter(Local<String> property,
                                          const AccessorInfo& info)
     {
        CEvasObject *eo = eo_from_info(info.This());
        String::Utf8Value prop_name(property);
        return eo->prop_get(*prop_name);
     }

   /* setup the property on construction */
   virtual void init_property(Handle<Object> out,
			 Handle<Value> name,
			 Handle<Value> value)
     {
        String::Utf8Value name_str(name);

        /* set or copy the property */
        if (!prop_set(*name_str, value))
          out->Set(name, value);
     }

public:
   virtual Handle<ObjectTemplate> get_template(void)
     {
        /* FIXME: only need to create one template per object class */
        the_template = Persistent<ObjectTemplate>::New(ObjectTemplate::New());

        prop_handler.fill_template(the_template);

        return the_template;
     }

   virtual Handle<Object> get_object(void)
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

   virtual Handle<Value> type_get(void) const
     {
        fprintf(stderr, "undefined object type!\n");
        return Undefined();
     }

   virtual void type_set(Handle<Value> value)
     {
        fprintf(stderr, "type cannot be set!\n");
     }

   Evas_Object *get() const
     {
        return eo;
     }

   virtual CEvasObject *get_child(Handle<Value> name)
     {
        fprintf(stderr, "get_child undefined\n");
        return NULL;
     }

   /*
    * set a property
    * return true if successful, false if not
    */
   virtual bool prop_set(const char *prop_name, Handle<Value> value)
     {
        CPropHandler<CEvasObject>::prop_setter setter;

        setter = prop_handler.get_setter(prop_name);
        if (setter)
          {
             (this->*setter)(value);
             return true;
          }
        return false;
     }

   virtual Handle<Value> prop_get(const char *prop_name) const
     {
        CPropHandler<CEvasObject>::prop_getter getter;
        getter = prop_handler.get_getter(prop_name);
        if (getter)
          return (this->*getter)();
        return Undefined();
     }


   // FIXME: could add to the parent here... raster to figure out
   Evas_Object *top_widget_get() const
     {
        return elm_object_top_widget_get(eo);
     }

   virtual ~CEvasObject()
     {
        evas_object_unref(eo);
        the_object.Dispose();
        the_template.Dispose();
        on_animate_val.Dispose();
        on_clicked_val.Dispose();
        on_keydown_val.Dispose();
        eo = NULL;
     }

   virtual void x_set(Handle<Value> val)
     {
       if (val->IsNumber())
         {
            Evas_Coord x, y, width, height;
            evas_object_geometry_get(eo, &x, &y, &width, &height);
            x = val->ToInt32()->Value();
            evas_object_move(eo, x, y);
         }
     }

   virtual Handle<Value> x_get(void) const
     {
       Evas_Coord x, y, width, height;
       evas_object_geometry_get(eo, &x, &y, &width, &height);
       return Number::New(x);
     }

   virtual void y_set(Handle<Value> val)
     {
       if (val->IsNumber())
         {
            Evas_Coord x, y, width, height;
            evas_object_geometry_get(eo, &x, &y, &width, &height);
            y = val->ToInt32()->Value();
            evas_object_move(eo, x, y);
         }
     }

   virtual Handle<Value> y_get(void) const
     {
       Evas_Coord x, y, width, height;
       evas_object_geometry_get(eo, &x, &y, &width, &height);
       return Number::New(y);
     }

   virtual void height_set(Handle<Value> val)
     {
       if (val->IsNumber())
         {
           Evas_Coord x, y, width, height;
           evas_object_geometry_get(eo, &x, &y, &width, &height);
           height = val->ToInt32()->Value();
           evas_object_resize(eo, width, height);
         }
     }

   virtual Handle<Value> height_get(void) const
     {
       Evas_Coord x, y, width, height;
       evas_object_geometry_get(eo, &x, &y, &width, &height);
       return Number::New(height);
     }

   virtual void width_set(Handle<Value> val)
     {
       if (val->IsNumber())
         {
           Evas_Coord x, y, width, height;
           evas_object_geometry_get(eo, &x, &y, &width, &height);
           width = val->ToInt32()->Value();
           evas_object_resize(eo, width, height);
         }
     }

   virtual Handle<Value> width_get(void) const
     {
       Evas_Coord x, y, width, height;
       evas_object_geometry_get(eo, &x, &y, &width, &height);
       return Number::New(width);
     }

   void move(Local<Value> x, Local<Value> y)
     {
        if (x->IsNumber() && y->IsNumber())
          evas_object_move(eo, x->Int32Value(), y->Int32Value());
     }

   virtual void on_click(void *event_info)
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
             fn->Call(fn, 3, args);
          }
        else
          {
             // FIXME: pass event_info to the callback
             // FIXME: turn the pieces below into a do_callback method
             Handle<Value> args[1] = { obj };
             assert(val->IsFunction());
             Handle<Function> fn(Function::Cast(*val));
             fn->Call(fn, 1, args);
          }

     }

   static void eo_on_click(void *data, Evas_Object *eo, void *event_info)
     {
        CEvasObject *clicked = static_cast<CEvasObject*>(data);

        clicked->on_click(event_info);
     }

   virtual void on_animate_set(Handle<Value> val)
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

   virtual Handle<Value> on_animate_get(void) const
     {
        return on_animate_val;
     }

   virtual void on_clicked_set(Handle<Value> val)
     {
        on_clicked_val.Dispose();
        on_clicked_val = Persistent<Value>::New(val);
        if (val->IsFunction())
          evas_object_smart_callback_add(eo, "clicked", &eo_on_click, this);
        else
          evas_object_smart_callback_del(eo, "clicked", &eo_on_click);
     }

   virtual Handle<Value> on_clicked_get(void) const
     {
        return on_clicked_val;
     }

   virtual void on_keydown(Evas_Event_Key_Down *info)
     {
        Handle<Object> obj = get_object();
        HandleScope handle_scope;
        Handle<Value> val = on_keydown_val;
        // FIXME: pass event_info to the callback
        // FIXME: turn the pieces below into a do_callback method
        assert(val->IsFunction());
        Handle<Function> fn(Function::Cast(*val));
        Handle<Value> args[2] = { obj, v8::String::New(info->keyname)};
        fn->Call(fn, 2,args);
     }

   static void eo_on_keydown(void *data, Evas *e, Evas_Object *obj, void *event_info)
     {
        CEvasObject *self = static_cast<CEvasObject*>(data);

        self->on_keydown(static_cast<Evas_Event_Key_Down*>(event_info));
     }

   virtual void on_keydown_set(Handle<Value> val)
     {
        on_keydown_val.Dispose();
        on_keydown_val = Persistent<Value>::New(val);
        if (val->IsFunction())
          evas_object_event_callback_add(eo, EVAS_CALLBACK_KEY_DOWN, &eo_on_keydown, this);
        else
          evas_object_event_callback_del(eo, EVAS_CALLBACK_KEY_DOWN, &eo_on_keydown);
     }

   virtual Handle<Value> on_keydown_get(void) const
     {
        return on_keydown_val;
     }

   virtual void on_animate(void)
     {
        Handle<Object> obj = get_object();
        HandleScope handle_scope;
        Handle<Value> val = on_animate_val;
        assert(val->IsFunction());
        Handle<Function> fn(Function::Cast(*val));
        Handle<Value> args[1] = { obj };
        fn->Call(fn, 1, args);
     }

   static Eina_Bool eo_on_animate(void *data)
     {
        CEvasObject *clicked = static_cast<CEvasObject*>(data);

        clicked->on_animate();

        return ECORE_CALLBACK_RENEW;
     }

   virtual Handle<Value> label_get() const
     {
       return String::New(elm_object_text_get(eo));
     }

   virtual Handle<Value> text_get() const
     {
       return label_get();
     }

   virtual void label_set(Handle<Value> val)
     {
        if (val->IsString() || val->IsNumber())
          {
             String::Utf8Value str(val);
             label_set(*str);
          }
     }

   virtual void text_set(Handle<Value> val)
     {
        label_set(val);
     }

   virtual void label_set(const char *str)
     {
        elm_object_text_set(eo, str);
     }

   virtual Handle<Value> disabled_get() const
     {
        return Boolean::New(elm_object_disabled_get(eo));
     }

   virtual void disabled_set(Handle<Value> val)
     {
        if (val->IsBoolean())
          elm_object_disabled_set(eo, val->BooleanValue());
     }

   virtual Handle<Value> scale_get() const
     {
        return Number::New(elm_object_scale_get(eo));
     }

   virtual void scale_set(Handle<Value> val)
     {
        if (val->IsNumber())
          elm_object_scale_set(eo, val->NumberValue());
     }

   bool get_xy_from_object(Handle<Value> val, double &x_out, double &y_out)
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

   bool get_xy_from_object(Handle<Value> val, bool &x_out, bool &y_out)
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

   bool get_xy_from_object(Handle<Value> val, int &x_out, int &y_out)
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

    bool get_xy_from_object(Handle<Value> val,
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

   virtual void weight_set(Handle<Value> weight)
     {
        double x, y;
        if (get_xy_from_object(weight, x, y))
          evas_object_size_hint_weight_set(eo, x, y);
     }

   virtual Handle<Value> weight_get(void) const
     {
       double x = 0.0, y = 0.0;
       evas_object_size_hint_weight_get(eo, &x, &y);
       Local<Object> obj = Object::New();
       obj->Set(String::New("x"), Number::New(x));
       obj->Set(String::New("y"), Number::New(y));
       return obj;
     }

   virtual void align_set(Handle<Value> align)
     {
        double x, y;
        if (get_xy_from_object(align, x, y))
          {
             evas_object_size_hint_align_set(eo, x, y);
          }
     }

   virtual Handle<Value> align_get(void) const
     {
       double x, y;
       evas_object_size_hint_align_get(eo, &x, &y);
       Local<Object> obj = Object::New();
       obj->Set(String::New("x"), Number::New(x));
       obj->Set(String::New("y"), Number::New(y));
       return obj;
     }

   virtual void image_set(Handle<Value> val)
     {
        if (val->IsString())
          fprintf(stderr, "no image set\n");
     }

   virtual Handle<Value> image_get(void) const
     {
        return Undefined();
     }

   virtual void show(bool show)
     {
        if (show)
          {
             evas_object_show(eo);
          }
        else
             evas_object_hide(eo);
     }

   /* returns a list of children in an object */
   Handle<Object> realize_objects(Handle<Value> val, Handle<Object> &out)
     {
        if (!val->IsObject())
          {
             fprintf(stderr, "not an object!\n");
             return out;
          }

        Handle<Object> in = val->ToObject();
        Handle<Array> props = in->GetPropertyNames();

        /* iterate through elements and instantiate them */
        for (unsigned int i = 0; i < props->Length(); i++)
          {

             Handle<Value> x = props->Get(Integer::New(i));
             String::Utf8Value val(x);

             CEvasObject *child = realize_one(this, in->Get(x->ToString()));
             if (!child)
               continue;
             add_child(child);

             Handle<Object> child_obj = child->get_object();
             out->Set(x, child_obj);
          }

        return out;
     }

   /* resize this object when the parent resizes? */
   virtual void resize_set(Handle<Value> val)
     {
        if (val->IsBoolean())
          {
             Evas_Object *parent = elm_object_parent_widget_get(eo);
             if (!parent)
               fprintf(stderr, "resize object has no parent!\n");
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
          fprintf(stderr, "Resize value not boolean!\n");
     }

   virtual Handle<Value> resize_get(void) const
     {
        return Boolean::New(is_resize);
     }

   virtual void pointer_set(Handle<Value> val)
     {
        // FIXME: ignore this, or move the pointer?
     }

   virtual Handle<Value> pointer_get(void) const
     {
        Evas_Coord x, y;
        evas_pointer_canvas_xy_get(evas_object_evas_get(eo), &x, &y);
        Local<Object> obj = Object::New();
        obj->Set(String::New("x"), Integer::New(x));
        obj->Set(String::New("y"), Integer::New(y));
        return obj;
     }

   virtual void style_set(Handle<Value> val)
     {
       if (val->IsString())
         {
            String::Utf8Value str(val);
            elm_object_style_set(eo, *str);
         }
     }

   virtual Handle<Value> style_get(void) const
     {
        return String::New(elm_object_style_get(eo));
     }

   virtual void visible_set(Handle<Value> val)
     {
        if (val->IsBoolean())
          {
             if (val->BooleanValue())
               evas_object_show(eo);
             else
               evas_object_hide(eo);
          }
     }

   virtual Handle<Value> visible_get(void) const
     {
        return Boolean::New(evas_object_visible_get(eo));
     }
};

template<> CEvasObject::CPropHandler<CEvasObject>::property_list
CEvasObject::CPropHandler<CEvasObject>::list[] = {
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
     { NULL, NULL, NULL },
};

class CEvasImage : public CEvasObject {
public:
   CEvasImage(CEvasObject *parent, Local<Object> obj) :
       CEvasObject()
     {
        Evas *evas = evas_object_evas_get(parent->get());
        eo = evas_object_image_filled_add(evas);
        construct(eo, obj);
     }

   virtual void image_set(Handle<Value> val)
     {
       if (val->IsString())
         {
            String::Utf8Value str(val);
             if (0 > access(*str, R_OK))
               fprintf(stderr, "warning: can't read image file %s\n", *str);
            evas_object_image_file_set(eo, *str, NULL);
         }
     }

   virtual Handle<Value> image_get(void) const
     {
        const char *file = NULL, *key = NULL;
        evas_object_image_file_get(eo, &file, &key);
        if (file)
          return String::New(file);
        else
          return Null();
     }
};

class CElmBasicWindow : public CEvasObject {
public:
   CElmBasicWindow(CEvasObject *parent, Local<Object> obj) :
       CEvasObject()
     {
        eo = elm_win_add(parent ? parent->get() : NULL, "main", ELM_WIN_BASIC);
        construct(eo, obj);

        /*
         * Create elements and attach to parent so children can see siblings
         * that have already been created.  Useful to find radio button groups.
         */
        Handle<Object> elements = Object::New();
        get_object()->Set(String::New("elements"), elements);
        realize_objects(obj->Get(String::New("elements")), elements);

        evas_object_focus_set(eo, 1);
        evas_object_smart_callback_add(eo, "delete,request", &on_delete, NULL);
     }

   virtual Handle<Value> type_get(void) const
     {
        return String::New("main");
     }

   virtual Handle<Value> label_get() const
     {
        return String::New(elm_win_title_get(eo));
     }

   virtual void label_set(const char *str)
     {
        elm_win_title_set(eo, str);
     }

   ~CElmBasicWindow()
     {
     }

   static void on_delete(void *data, Evas_Object *obj, void *event_info)
     {
        elm_exit();
     }

   virtual void resize_set(Handle<Value> val)
     {
        fprintf(stderr, "warning: resize=true ignored on main window\n");
     }
};

CElmBasicWindow *main_win;

class CElmButton : public CEvasObject {
protected:
   Persistent<Value> the_icon;
   static CPropHandler<CElmButton> prop_handler;
public:
   CElmButton(CEvasObject *parent, Local<Object> obj) :
       CEvasObject()
     {
        eo = elm_button_add(parent->top_widget_get());
        construct(eo, obj);
     }

   virtual Handle<ObjectTemplate> get_template(void)
     {
        Handle<ObjectTemplate> ot = CEvasObject::get_template();
        prop_handler.fill_template(ot);
        return ot;
     }

   virtual ~CElmButton()
     {
        the_icon.Dispose();
     }

   virtual bool prop_set(const char *prop_name, Handle<Value> value)
     {
        CPropHandler<CElmButton>::prop_setter setter;

        setter = prop_handler.get_setter(prop_name);
        if (setter)
          {
             (this->*setter)(value);
             return true;
          }
        return CEvasObject::prop_set(prop_name, value);
     }

   virtual Handle<Value> prop_get(const char *prop_name) const
     {
        CPropHandler<CElmButton>::prop_getter getter;
        getter = prop_handler.get_getter(prop_name);
        if (getter)
          return (this->*getter)();
        return CEvasObject::prop_get(prop_name);
     }

   virtual Handle<Value> icon_get() const
     {
        return the_icon;
     }

   virtual void icon_set(Handle<Value> value)
     {
        the_icon.Dispose();
        CEvasObject *icon = realize_one(this, value);
        elm_button_icon_set(eo, icon->get());
        the_icon = Persistent<Value>::New(icon->get_object());
     }
};

template<> CEvasObject::CPropHandler<CElmButton>::property_list
CEvasObject::CPropHandler<CElmButton>::list[] = {
     PROP_HANDLER(CElmButton, icon),
     { NULL, NULL, NULL },
};

class CElmBackground : public CEvasObject {
protected:
   static CPropHandler<CElmBackground> prop_handler;
public:
   explicit CElmBackground(CEvasObject *parent, Local<Object> obj) :
       CEvasObject()
     {
        eo = elm_bg_add(parent->get());
        construct(eo, obj);
     }

   virtual ~CElmBackground()
     {
     }

   virtual Handle<ObjectTemplate> get_template(void)
     {
        Handle<ObjectTemplate> ot = CEvasObject::get_template();
        prop_handler.fill_template(ot);
        return ot;
     }

   virtual bool prop_set(const char *prop_name, Handle<Value> value)
     {
        CPropHandler<CElmBackground>::prop_setter setter;

        setter = prop_handler.get_setter(prop_name);
        if (setter)
          {
             (this->*setter)(value);
             return true;
          }
        return CEvasObject::prop_set(prop_name, value);
     }

   virtual Handle<Value> prop_get(const char *prop_name) const
     {
        CPropHandler<CElmBackground>::prop_getter getter;
        getter = prop_handler.get_getter(prop_name);
        if (getter)
          return (this->*getter)();
        return CEvasObject::prop_get(prop_name);
     }

   virtual void image_set(Handle<Value> val)
     {
        if (val->IsString())
          {
             String::Utf8Value str(val);
             elm_bg_file_set(eo, *str, NULL);
          }
     }

   virtual Handle<Value> image_get(void) const
     {
        const char *file = NULL, *group = NULL;
        elm_bg_file_get(eo, &file, &group);
        if (file)
          return String::New(file);
        else
          return Null();
     }

  virtual Handle<Value> red_get() const
    {
       int r, g, b;
       elm_bg_color_get(eo, &r, &g, &b);
       return Number::New(r);
    }

  virtual void red_set(Handle<Value> val)
    {
       fprintf(stderr, "Red Set\n");
       if (val->IsNumber())
         {
            int r, g, b;
            elm_bg_color_get(eo, &r, &g, &b);
        r = val->ToNumber()->Value();
            elm_bg_color_set(eo, r, g, b);
         }
    }

  virtual Handle<Value> green_get() const
    {
       int r, g, b;
       elm_bg_color_get(eo, &r, &g, &b);
       return Number::New(g);
    }

  virtual void green_set(Handle<Value> val)
    {
       if (val->IsNumber())
         {
            int r, g, b;
            elm_bg_color_get(eo, &r, &g, &b);
            g = val->ToNumber()->Value();
            elm_bg_color_set(eo, r, g, b);
        fprintf(stderr, "Set green to %d\n", g);
         }
    }
  virtual Handle<Value> blue_get() const
    {
       int r, g, b;
       elm_bg_color_get(eo, &r, &g, &b);
       return Number::New(b);
    }

  virtual void blue_set(Handle<Value> val)
    {
       if (val->IsNumber())
         {
            int r, g, b;
            elm_bg_color_get(eo, &r, &g, &b);
        b = val->ToNumber()->Value();
            elm_bg_color_set(eo, r, g, b);
         }
    }

};

template<> CEvasObject::CPropHandler<CElmBackground>::property_list
CEvasObject::CPropHandler<CElmBackground>::list[] = {
  PROP_HANDLER(CElmBackground, red),
  PROP_HANDLER(CElmBackground, green),
  PROP_HANDLER(CElmBackground, blue),
  { NULL, NULL, NULL },
};

class CElmRadio : public CEvasObject {
protected:
   static CPropHandler<CElmRadio> prop_handler;
   Persistent<Value> the_icon;
   Persistent<Value> the_group;

public:
   CElmRadio(CEvasObject *parent, Local<Object> obj) :
       CEvasObject()
     {
        eo = elm_radio_add(parent->get());
        construct(eo, obj);
     }

   virtual ~CElmRadio()
     {
        the_icon.Dispose();
        the_group.Dispose();
     }

   virtual Handle<ObjectTemplate> get_template(void)
     {
        Handle<ObjectTemplate> ot = CEvasObject::get_template();
        prop_handler.fill_template(ot);
        return ot;
     }

   virtual bool prop_set(const char *prop_name, Handle<Value> value)
     {
        CPropHandler<CElmRadio>::prop_setter setter;

        setter = prop_handler.get_setter(prop_name);
        if (setter)
          {
             (this->*setter)(value);
             return true;
          }
        return CEvasObject::prop_set(prop_name, value);
     }

   virtual Handle<Value> prop_get(const char *prop_name) const
     {
        CPropHandler<CElmRadio>::prop_getter getter;
        getter = prop_handler.get_getter(prop_name);
        if (getter)
          return (this->*getter)();
        return CEvasObject::prop_get(prop_name);
     }

   virtual Handle<Value> icon_get() const
     {
        return the_icon;
     }

   virtual void icon_set(Handle<Value> value)
     {
        the_icon.Dispose();
        CEvasObject *icon = realize_one(this, value);
        elm_radio_icon_set(eo, icon->get());
        the_icon = Persistent<Value>::New(icon->get_object());
     }

   virtual Handle<Value> group_get() const
     {
        return the_group;
     }

   virtual void group_set(Handle<Value> value)
     {
        the_group = Persistent<Value>::New(value);

        CEvasObject *parent = get_parent();
        if (parent)
          {
             CEvasObject *group = parent->get_child(value);
             if (group)
               {
                  if (dynamic_cast<CElmRadio*>(group))
                    elm_radio_group_add(eo, group->get());
                  else
                    fprintf(stderr, "%p not a radio button!\n", group);
               }
             else
               fprintf(stderr, "child %s not found!\n", *String::Utf8Value(value->ToString()));
          }
     }

   virtual Handle<Value> value_get() const
     {
        return Integer::New(elm_radio_state_value_get(eo));
     }

   virtual void value_set(Handle<Value> value)
     {
        if (value->IsNumber())
          elm_radio_state_value_set(eo, value->Int32Value());
     }
};

template<> CEvasObject::CPropHandler<CElmRadio>::property_list
CEvasObject::CPropHandler<CElmRadio>::list[] = {
  PROP_HANDLER(CElmRadio, icon),
  PROP_HANDLER(CElmRadio, group),
  PROP_HANDLER(CElmRadio, value),
  { NULL, NULL, NULL },
};

class CElmBox : public CEvasObject {
protected:
   virtual void add_child(CEvasObject *child)
     {
        elm_box_pack_end(eo, child->get());
     }

   virtual CEvasObject *get_child(Handle<Value> name)
     {
        CEvasObject *ret = NULL;

        Handle<Object> obj = get_object();
        Local<Value> elements_val = obj->Get(String::New("elements"));

        if (!elements_val->IsObject())
          {
             fprintf(stderr, "elements not an object\n");
             return ret;
          }

        Local<Object> elements = elements_val->ToObject();
        Local<Value> val = elements->Get(name);

        if (val->IsObject())
          ret = eo_from_info(val->ToObject());
        else
          fprintf(stderr, "value %s not an object\n", *String::Utf8Value(val->ToString()));

        return ret;
     }

   static CPropHandler<CElmBox> prop_handler;

public:
   CElmBox(CEvasObject *parent, Local<Object> obj) :
       CEvasObject()
     {
        eo = elm_box_add(parent->top_widget_get());
        construct(eo, obj);

        /*
         * Create elements and attach to parent so children can see siblings
         * that have already been created.  Useful to find radio button groups.
         */
        Handle<Object> elements = Object::New();
        get_object()->Set(String::New("elements"), elements);
        realize_objects(obj->Get(String::New("elements")), elements);
     }

   virtual Handle<ObjectTemplate> get_template(void)
     {
        Handle<ObjectTemplate> ot = CEvasObject::get_template();
        prop_handler.fill_template(ot);
        return ot;
     }

   virtual bool prop_set(const char *prop_name, Handle<Value> value)
     {
        CPropHandler<CElmBox>::prop_setter setter;

        setter = prop_handler.get_setter(prop_name);
        if (setter)
          {
             (this->*setter)(value);
             return true;
          }
        return CEvasObject::prop_set(prop_name, value);
     }

   virtual Handle<Value> prop_get(const char *prop_name) const
     {
        CPropHandler<CElmBox>::prop_getter getter;
        getter = prop_handler.get_getter(prop_name);
        if (getter)
          return (this->*getter)();
        return CEvasObject::prop_get(prop_name);
     }

   void horizontal_set(Handle<Value> val)
     {
        if (val->IsBoolean())
          {
             elm_box_horizontal_set(eo, val->BooleanValue());
          }
     }

   virtual Handle<Value> horizontal_get() const
     {
        return Boolean::New(elm_box_horizontal_get(eo));
     }

   void homogeneous_set(Handle<Value> val)
     {
        if (val->IsBoolean())
          elm_box_homogeneous_set(eo, val->BooleanValue());
     }

   virtual Handle<Value> homogeneous_get() const
     {
        return Boolean::New(elm_box_homogeneous_get(eo));
     }
};

template<> CEvasObject::CPropHandler<CElmBox>::property_list
CEvasObject::CPropHandler<CElmBox>::list[] = {
     PROP_HANDLER(CElmBox, horizontal),
     PROP_HANDLER(CElmBox, homogeneous),
     { NULL, NULL, NULL },
};

class CElmLabel : public CEvasObject {
protected:
   static CPropHandler<CElmLabel> prop_handler;

public:
   CElmLabel(CEvasObject *parent, Local<Object> obj) :
       CEvasObject()
     {
        eo = elm_label_add(parent->get());
        construct(eo, obj);
     }

   virtual Handle<ObjectTemplate> get_template(void)
     {
        Handle<ObjectTemplate> ot = CEvasObject::get_template();
        prop_handler.fill_template(ot);
        return ot;
     }

   virtual bool prop_set(const char *prop_name, Handle<Value> value)
     {
        CPropHandler<CElmLabel>::prop_setter setter;

        setter = prop_handler.get_setter(prop_name);
        if (setter)
          {
             (this->*setter)(value);
             return true;
          }
        return CEvasObject::prop_set(prop_name, value);
     }

   virtual Handle<Value> prop_get(const char *prop_name) const
     {
        CPropHandler<CElmLabel>::prop_getter getter;
        getter = prop_handler.get_getter(prop_name);
        if (getter)
          return (this->*getter)();
        return CEvasObject::prop_get(prop_name);
     }

   virtual void wrap_set(Handle<Value> wrap)
     {
        if (wrap->IsNumber())
          elm_label_line_wrap_set(eo, static_cast<Elm_Wrap_Type>(wrap->Int32Value()));
     }

   virtual Handle<Value> wrap_get() const
     {
        return Integer::New(elm_label_line_wrap_get(eo));
     }
};

template<> CEvasObject::CPropHandler<CElmLabel>::property_list
CEvasObject::CPropHandler<CElmLabel>::list[] = {
     PROP_HANDLER(CElmLabel, wrap),
     /* PROP_HANDLER(CElmLabel, label), - not necessary, called from CEvasObject */
     /* FIXME: add fontsize, wrap_height, wrap_width, ellipsis, slide, slide_duration */
     { NULL, NULL, NULL },
};

class CElmFlip : public CEvasObject {
public:
   static Handle<Value> do_flip(const Arguments& args)
     {
        CEvasObject *self = eo_from_info(args.This());
        CElmFlip *flipper = static_cast<CElmFlip *>(self);
        flipper->flip(ELM_FLIP_ROTATE_Y_CENTER_AXIS);
        return Undefined();
     }

   virtual void flip(Elm_Flip_Mode mode)
     {
        elm_flip_go(eo, mode);
     }

   CElmFlip(CEvasObject *parent, Local<Object> obj) :
       CEvasObject()
     {
        CEvasObject *front, *back;

        eo = elm_flip_add(parent->get());
        construct(eo, obj);

        /* realize front and back */
        front = realize_one(this, obj->Get(String::New("front")));
        elm_flip_content_front_set(eo, front->get());

        back = realize_one(this, obj->Get(String::New("back")));
        elm_flip_content_back_set(eo, back->get());

        get_object()->Set(String::New("flip"), FunctionTemplate::New(do_flip)->GetFunction());
     }
};

class CElmIcon : public CEvasObject {
public:
   static CPropHandler<CElmIcon> prop_handler;
public:
   CElmIcon(CEvasObject *parent, Local<Object> obj) :
       CEvasObject()
     {
        eo = elm_icon_add(parent->top_widget_get());
        construct(eo, obj);
     }

   virtual Handle<ObjectTemplate> get_template(void)
     {
        Handle<ObjectTemplate> ot = CEvasObject::get_template();
        prop_handler.fill_template(ot);
        return ot;
     }

   virtual bool prop_set(const char *prop_name, Handle<Value> value)
     {
        CPropHandler<CElmIcon>::prop_setter setter;

        setter = prop_handler.get_setter(prop_name);
        if (setter)
          {
             (this->*setter)(value);
             return true;
          }
        return CEvasObject::prop_set(prop_name, value);
     }

   virtual Handle<Value> prop_get(const char *prop_name) const
     {
        CPropHandler<CElmIcon>::prop_getter getter;
        getter = prop_handler.get_getter(prop_name);
        if (getter)
          return (this->*getter)();
        return CEvasObject::prop_get(prop_name);
     }

   virtual void scale_up_set(Handle<Value> val)
     {
        Eina_Bool up, down;
        if (val->IsBoolean())
          {
             elm_icon_scale_get(eo, &up, &down);
             elm_icon_scale_set(eo, val->BooleanValue(), down);
          }
     }

   virtual Handle<Value> scale_up_get() const
     {
        Eina_Bool up, down;
        elm_icon_scale_get(eo, &up, &down);
        return Boolean::New(up);
     }

   virtual void scale_down_set(Handle<Value> val)
     {
        Eina_Bool up, down;
        if (val->IsBoolean())
          {
             elm_icon_scale_get(eo, &up, &down);
             elm_icon_scale_set(eo, up, val->BooleanValue());
          }
     }

   virtual Handle<Value> prescale_get() const
     {
        int prescale=elm_icon_prescale_get(eo);
        return Integer::New(prescale);
     }

   virtual void prescale_set(Handle<Value> val)
     {
        if (val->IsNumber())
          {
             elm_icon_prescale_set(eo, val->IntegerValue());
          }
     }

   virtual Handle<Value> scale_down_get() const
     {
        Eina_Bool up, down;
        elm_icon_scale_get(eo, &up, &down);
        return Boolean::New(down);
     }

   virtual void image_set(Handle<Value> val)
     {
        if (val->IsString())
          {
             String::Utf8Value str(val);
             if (0 > access(*str, R_OK))
               fprintf(stderr, "warning: can't read icon file %s\n", *str);
             elm_icon_file_set(eo, *str, NULL);
          }
     }

   virtual Handle<Value> image_get(void) const
     {
        const char *file = NULL, *group = NULL;
        elm_icon_file_get(eo, &file, &group);
        if (file)
          return String::New(file);
        else
          return Null();
     }
};

template<> CEvasObject::CPropHandler<CElmIcon>::property_list
CEvasObject::CPropHandler<CElmIcon>::list[] = {
     PROP_HANDLER(CElmIcon, scale_up),
     PROP_HANDLER(CElmIcon, scale_down),
     PROP_HANDLER(CElmIcon, prescale),
     { NULL, NULL, NULL },
};

class CElmActionSlider : public CEvasObject {
private:
   static CPropHandler<CElmActionSlider> prop_handler;

public:
   CElmActionSlider(CEvasObject *parent, Local<Object> obj) :
       CEvasObject()
     {
        eo = elm_actionslider_add(parent->get());
        construct(eo, obj);
     }

   virtual Handle<ObjectTemplate> get_template(void)
     {
        Handle<ObjectTemplate> ot = CEvasObject::get_template();
        prop_handler.fill_template(ot);
        return ot;
     }

   virtual bool prop_set(const char *prop_name, Handle<Value> value)
     {
        CPropHandler<CElmActionSlider>::prop_setter setter;

        setter = prop_handler.get_setter(prop_name);
        if (setter)
          {
             (this->*setter)(value);
             return true;
          }
        return CEvasObject::prop_set(prop_name, value);
     }

   virtual Handle<Value> prop_get(const char *prop_name) const
     {
        CPropHandler<CElmActionSlider>::prop_getter getter;
        getter = prop_handler.get_getter(prop_name);
        if (getter)
          return (this->*getter)();
        return CEvasObject::prop_get(prop_name);
     }

   /* there's 1 indicator label and 3 position labels */
   virtual void labels_set(Handle<Value> val)
     {
        if (val->IsObject())
          {
             Local<Object> obj = val->ToObject();
             Local<Value> v[3];
             Local<String> str[3];
             const char *name[3] = { "left", "center", "right" };

             for (int i = 0; i < 3; i++)
               {
                  v[i] = obj->Get(String::New(name[i]));
                  if (v[i]->IsString())
                    str[i] = v[i]->ToString();
               }
             String::Utf8Value left(str[0]), middle(str[1]), right(str[2]);
#ifndef OLD_APIS
         elm_object_text_part_set(eo, name[0], *left);
         elm_object_text_part_set(eo,name[1], *middle);
         elm_object_text_part_set(eo,name[2], *right);
#else
             elm_actionslider_label_set(eo, ELM_ACTIONSLIDER_LABEL_LEFT, *left);
             elm_actionslider_label_set(eo, ELM_ACTIONSLIDER_LABEL_CENTER, *middle);
             elm_actionslider_label_set(eo, ELM_ACTIONSLIDER_LABEL_RIGHT, *right);
#endif
          }
     }

   virtual Handle<Value> labels_get() const
     {
        // FIXME: implement
        return Undefined();
     }

#ifndef OLD_APIS
   bool position_from_string(Handle<Value> val, Elm_Actionslider_Pos &pos)
     {
        if (!val->IsString())
          return false;

        String::Utf8Value str(val);
        if (!strcmp(*str, "left"))
          pos = ELM_ACTIONSLIDER_LEFT;
        else if (!strcmp(*str, "center"))
          pos = ELM_ACTIONSLIDER_CENTER;
        else if (!strcmp(*str, "right"))
          pos = ELM_ACTIONSLIDER_RIGHT;
        else
          {
             fprintf(stderr, "Invalid actionslider position: %s\n", *str);
             return false;
          }
        return true;
     }
#else
   bool position_from_string(Handle<Value> val, Elm_Actionslider_Magnet_Pos &pos)
     {
        if (!val->IsString())
          return false;

        String::Utf8Value str(val);
        if (!strcmp(*str, "left"))
          pos = ELM_ACTIONSLIDER_MAGNET_LEFT;
        else if (!strcmp(*str, "center"))
          pos = ELM_ACTIONSLIDER_MAGNET_CENTER;
        else if (!strcmp(*str, "right"))
          pos = ELM_ACTIONSLIDER_MAGNET_RIGHT;
        else
          {
             fprintf(stderr, "Invalid actionslider magnet position: %s\n", *str);
             return false;
          }
        return true;
     }

   bool position_from_string(Handle<Value> val, Elm_Actionslider_Indicator_Pos &pos)
     {
        if (!val->IsString())
          return false;

        String::Utf8Value str(val);
        if (!strcmp(*str, "left"))
          pos = ELM_ACTIONSLIDER_INDICATOR_LEFT;
        else if (!strcmp(*str, "center"))
          pos = ELM_ACTIONSLIDER_INDICATOR_CENTER;
        else if (!strcmp(*str, "right"))
          pos = ELM_ACTIONSLIDER_INDICATOR_RIGHT;
        else
          {
             fprintf(stderr, "Invalid actionslider indicator position: %s\n", *str);
             return false;
          }
        return true;
     }
#endif

   virtual void slider_set(Handle<Value> val)
     {
#ifndef OLD_APIS
        Elm_Actionslider_Pos pos = ELM_ACTIONSLIDER_NONE;
#else
        Elm_Actionslider_Indicator_Pos pos = ELM_ACTIONSLIDER_INDICATOR_NONE;
#endif
        if (position_from_string(val, pos))
          elm_actionslider_indicator_pos_set(eo, pos);
     }

   virtual Handle<Value> slider_get() const
     {
        return Integer::New(elm_actionslider_indicator_pos_get(eo));
     }

   virtual void magnet_set(Handle<Value> val)
     {
#ifndef OLD_APIS
        Elm_Actionslider_Pos pos = ELM_ACTIONSLIDER_NONE;
#else
        Elm_Actionslider_Magnet_Pos pos = ELM_ACTIONSLIDER_MAGNET_NONE;
#endif

        if (position_from_string(val, pos))
          elm_actionslider_magnet_pos_set(eo, pos);
     }

   virtual Handle<Value> magnet_get() const
     {
        return Integer::New(elm_actionslider_magnet_pos_get(eo));
     }
};

template<> CEvasObject::CPropHandler<CElmActionSlider>::property_list
CEvasObject::CPropHandler<CElmActionSlider>::list[] = {
     PROP_HANDLER(CElmActionSlider, magnet),
     PROP_HANDLER(CElmActionSlider, slider),
     PROP_HANDLER(CElmActionSlider, labels),
     { NULL, NULL, NULL },
};

class CElmScroller : public CEvasObject {
private:
   static CPropHandler<CElmScroller> prop_handler;

public:
   CElmScroller(CEvasObject *parent, Local<Object> obj) :
       CEvasObject()
     {
        CEvasObject *content;
        eo = elm_scroller_add(parent->get());
        construct(eo, obj);
        content = realize_one(this, obj->Get(String::New("content")));
        if (!content)
          fprintf(stderr, "scroller has no content\n");
        // FIXME: filter the object list copied in construct for more efficiency
        get_object()->Set(String::New("content"), content->get_object());
        elm_scroller_content_set(eo, content->get());
     }

   virtual Handle<ObjectTemplate> get_template(void)
     {
        Handle<ObjectTemplate> ot = CEvasObject::get_template();
        prop_handler.fill_template(ot);
        return ot;
     }

   virtual bool prop_set(const char *prop_name, Handle<Value> value)
     {
        CPropHandler<CElmScroller>::prop_setter setter;
        setter = prop_handler.get_setter(prop_name);
        if (setter)
          {
             (this->*setter)(value);
             return true;
          }
        return CEvasObject::prop_set(prop_name, value);
     }

   virtual Handle<Value> prop_get(const char *prop_name) const
     {
        CPropHandler<CElmScroller>::prop_getter getter;
        getter = prop_handler.get_getter(prop_name);
        if (getter)
          return (this->*getter)();
        return CEvasObject::prop_get(prop_name);
     }

    virtual void bounce_set(Handle<Value> val)
     {
        bool x_bounce = false, y_bounce = false;
        if (get_xy_from_object(val, x_bounce, y_bounce))
          {
             elm_scroller_bounce_set(eo, x_bounce, y_bounce);
          }
     }

   virtual Handle<Value> bounce_get() const
     {
        Eina_Bool x, y;
        elm_scroller_bounce_get(eo, &x, &y);
        Local<Object> obj = Object::New();
        obj->Set(String::New("x"), Boolean::New(x));
        obj->Set(String::New("y"), Boolean::New(y));
        return obj;
     }


   static Elm_Scroller_Policy policy_from_string(Handle<Value> val)
     {
        String::Utf8Value str(val);
        Elm_Scroller_Policy policy = ELM_SCROLLER_POLICY_AUTO;

        if (!strcmp(*str, "auto"))
          policy = ELM_SCROLLER_POLICY_AUTO;
        else if (!strcmp(*str, "on"))
          policy = ELM_SCROLLER_POLICY_ON;
        else if (!strcmp(*str, "off"))
          policy = ELM_SCROLLER_POLICY_OFF;
        else if (!strcmp(*str, "last"))
          policy = ELM_SCROLLER_POLICY_LAST;
        else
          fprintf(stderr, "unknown scroller policy %s\n", *str);

        return policy;
     }

   static Local<Value> string_from_policy(Elm_Scroller_Policy policy)
     {
        switch (policy)
          {
        case ELM_SCROLLER_POLICY_AUTO:
          return String::New("auto");
        case ELM_SCROLLER_POLICY_ON:
          return String::New("on");
        case ELM_SCROLLER_POLICY_OFF:
          return String::New("off");
        case ELM_SCROLLER_POLICY_LAST:
          return String::New("last");
        default:
          return String::New("unknown");
          }
     }

   virtual void policy_set(Handle<Value> val)
     {
        Local<Value> x_val, y_val;

        if (get_xy_from_object(val, x_val, y_val))
          {
             Elm_Scroller_Policy x_policy, y_policy;
             x_policy = policy_from_string(x_val);
             y_policy = policy_from_string(y_val);
             elm_scroller_policy_set(eo, x_policy, y_policy);
          }
     }

   virtual Handle<Value> policy_get() const
     {
        Elm_Scroller_Policy x_policy, y_policy;
        elm_scroller_policy_get(eo, &x_policy, &y_policy);
        Local<Object> obj = Object::New();
        obj->Set(String::New("x"), string_from_policy(x_policy));
        obj->Set(String::New("y"), string_from_policy(y_policy));
        return obj;
     }
};

template<> CEvasObject::CPropHandler<CElmScroller>::property_list
CEvasObject::CPropHandler<CElmScroller>::list[] = {
     PROP_HANDLER(CElmScroller, bounce),
     PROP_HANDLER(CElmScroller, policy),
};

class CElmSlider : public CEvasObject {
protected:
   Persistent<Value> the_icon;
   Persistent<Value> the_end_object;
   Persistent<Value> on_changed_val;
   static CPropHandler<CElmSlider> prop_handler;

public:
   CElmSlider(CEvasObject *parent, Local<Object> obj) :
       CEvasObject()
     {
        eo = elm_slider_add(parent->get());
        construct(eo, obj);
     }

   virtual ~CElmSlider()
     {
        the_icon.Dispose();
        the_end_object.Dispose();
        on_changed_val.Dispose();
     }

   virtual Handle<ObjectTemplate> get_template(void)
     {
        Handle<ObjectTemplate> ot = CEvasObject::get_template();
        prop_handler.fill_template(ot);
        return ot;
     }

   virtual bool prop_set(const char *prop_name, Handle<Value> value)
     {
        CPropHandler<CElmSlider>::prop_setter setter;
        setter = prop_handler.get_setter(prop_name);
        if (setter)
          {
             (this->*setter)(value);
             return true;
          }
        return CEvasObject::prop_set(prop_name, value);
     }

   virtual Handle<Value> prop_get(const char *prop_name) const
     {
        CPropHandler<CElmSlider>::prop_getter getter;
        getter = prop_handler.get_getter(prop_name);
        if (getter)
          return (this->*getter)();
        return CEvasObject::prop_get(prop_name);
     }

   virtual void units_set(Handle<Value> value)
     {
        if (value->IsString())
          {
             String::Utf8Value str(value);
             elm_slider_unit_format_set(eo, *str);
          }
     }

   virtual Handle<Value> units_get() const
     {
        return String::New(elm_slider_unit_format_get(eo));
     }

   virtual void indicator_set(Handle<Value> value)
     {
        if (value->IsString())
          {
            String::Utf8Value str(value);
            elm_slider_indicator_format_set(eo, *str);
          }
     }

   virtual Handle<Value> indicator_get() const
     {
        return String::New(elm_slider_indicator_format_get(eo));
     }

   virtual Handle<Value> span_get() const
     {
        return Integer::New(elm_slider_span_size_get(eo));
     }

   virtual void span_set(Handle<Value> value)
     {
        if (value->IsInt32())
          {
             int span = value->Int32Value();
             elm_slider_span_size_set(eo, span);
          }
     }

   virtual Handle<Value> icon_get() const
     {
        return the_icon;
     }

   virtual void icon_set(Handle<Value> value)
     {
        the_icon.Dispose();
        CEvasObject *icon = realize_one(this, value);
        elm_slider_icon_set(eo, icon->get());
        the_icon = Persistent<Value>::New(icon->get_object());
     }

   virtual Handle<Value> end_get() const
     {
        return the_end_object;
     }

   virtual void end_set(Handle<Value> value)
     {
        the_end_object.Dispose();
        CEvasObject *end_obj = realize_one(this, value);
        if (end_obj)
          {
             elm_slider_end_set(eo, end_obj->get());
             the_end_object = Persistent<Value>::New(end_obj->get_object());
          }
        else
             elm_slider_end_unset(eo);
     }

   virtual Handle<Value> value_get() const
     {
        return Number::New(elm_slider_value_get(eo));
     }

   virtual void value_set(Handle<Value> value)
     {
        if (value->IsNumber())
          elm_slider_value_set(eo, value->NumberValue());
     }

   virtual Handle<Value> min_get() const
     {
        double min, max;
        elm_slider_min_max_get(eo, &min, &max);
        return Number::New(min);
     }

   virtual void min_set(Handle<Value> value)
     {
        if (value->IsNumber())
          {
             double min, max;
             elm_slider_min_max_get(eo, &min, &max);
             min = value->NumberValue();
             elm_slider_min_max_set(eo, min, max);
          }
     }

   virtual Handle<Value> max_get() const
     {
        double min, max;
        elm_slider_min_max_get(eo, &min, &max);
        return Number::New(max);
     }

   virtual void max_set(Handle<Value> value)
     {
        if (value->IsNumber())
          {
             double min, max;
             elm_slider_min_max_get(eo, &min, &max);
             max = value->NumberValue();
             elm_slider_min_max_set(eo, min, max);
          }
     }

   virtual Handle<Value> inverted_get() const
     {
        return Boolean::New(elm_slider_inverted_get(eo));
     }

   virtual void inverted_set(Handle<Value> value)
     {
        if (value->IsBoolean())
          elm_slider_inverted_set(eo, value->BooleanValue());
     }

   virtual Handle<Value> horizontal_get() const
     {
        return Boolean::New(elm_slider_horizontal_get(eo));
     }

   virtual void horizontal_set(Handle<Value> value)
     {
        if (value->IsBoolean())
          elm_slider_horizontal_set(eo, value->BooleanValue());
     }

   static void eo_on_changed(void *data, Evas_Object *eo, void *event_info)
     {
        CElmSlider *changed = static_cast<CElmSlider*>(data);

        changed->on_changed(event_info);
     }

   virtual void on_changed(void *event_info)
     {
        Handle<Object> obj = get_object();
        HandleScope handle_scope;
        Handle<Value> val = on_changed_val;
        // FIXME: pass event_info to the callback
        // FIXME: turn the pieces below into a do_callback method
        assert(val->IsFunction());
        Handle<Function> fn(Function::Cast(*val));
        Handle<Value> args[1] = { obj };
        fn->Call(fn, 1, args);
     }

   virtual void on_changed_set(Handle<Value> val)
     {
        on_changed_val.Dispose();
        on_changed_val = Persistent<Value>::New(val);
        if (val->IsFunction())
          evas_object_smart_callback_add(eo, "changed", &eo_on_changed, this);
        else
          evas_object_smart_callback_del(eo, "changed", &eo_on_changed);
     }

   virtual Handle<Value> on_changed_get(void) const
     {
        return on_changed_val;
     }

};

template<> CEvasObject::CPropHandler<CElmSlider>::property_list
CEvasObject::CPropHandler<CElmSlider>::list[] = {
  PROP_HANDLER(CElmSlider, units),
  PROP_HANDLER(CElmSlider, indicator),
  PROP_HANDLER(CElmSlider, span),
  PROP_HANDLER(CElmSlider, icon),
  PROP_HANDLER(CElmSlider, value),
  PROP_HANDLER(CElmSlider, min),
  PROP_HANDLER(CElmSlider, max),
  PROP_HANDLER(CElmSlider, inverted),
  PROP_HANDLER(CElmSlider, end),
  PROP_HANDLER(CElmSlider, horizontal),
  PROP_HANDLER(CElmSlider, on_changed),
  { NULL, NULL, NULL },
};

class CElmGenList : public CEvasObject {
protected:
public:
   CElmGenList(CEvasObject *parent, Local<Object> obj) :
       CEvasObject()
     {
        eo = elm_genlist_add(parent->get());
        construct(eo, obj);
     }
};

class CElmList : public CEvasObject {
protected:
public:
   CElmList(CEvasObject *parent, Local<Object> obj) :
       CEvasObject()
     {
        eo = elm_list_add(parent->get());
        construct(eo, obj);
        items_set(obj->Get(String::New("items")));
     }

   Handle<Object> items_set(Handle<Value> val)
     {
        /* add an list of children */
        Local<Object> out = Object::New();

        if (!val->IsObject())
          {
             fprintf(stderr, "not an object!\n");
             return out;
          }

        Local<Object> in = val->ToObject();
        Local<Array> props = in->GetPropertyNames();

        /* iterate through elements and instantiate them */
        for (unsigned int i = 0; i < props->Length(); i++)
          {

             Local<Value> x = props->Get(Integer::New(i));
             String::Utf8Value val(x);

             Local<Value> item = in->Get(x->ToString());
             if (!item->IsObject())
               {
                  // FIXME: permit adding strings here?
                  fprintf(stderr, "list item is not an object\n");
                  continue;
               }
             Local<Value> label = item->ToObject()->Get(String::New("label"));

             String::Utf8Value str(label);
             elm_list_item_append(eo, *str, NULL, NULL, NULL, NULL);
          }

        return out;
     }
};

class CElmEntry : public CEvasObject {
protected:
   static CPropHandler<CElmEntry> prop_handler;
public:
   CElmEntry(CEvasObject *parent, Local<Object> obj) :
       CEvasObject()
     {
        eo = elm_entry_add(parent->get());
        construct(eo, obj);
     }

   virtual bool prop_set(const char *prop_name, Handle<Value> value)
     {
        CPropHandler<CElmEntry>::prop_setter setter;
        setter = prop_handler.get_setter(prop_name);
        if (setter)
          {
             (this->*setter)(value);
             return true;
          }
        return CEvasObject::prop_set(prop_name, value);
     }

   virtual Handle<Value> prop_get(const char *prop_name) const
     {
        CPropHandler<CElmEntry>::prop_getter getter;
        getter = prop_handler.get_getter(prop_name);
        if (getter)
          return (this->*getter)();
        return CEvasObject::prop_get(prop_name);
     }

   virtual Handle<Value> label_get() const
     {
        return String::New(elm_entry_entry_get(eo));
     }

   virtual void label_set(const char *str)
     {
        elm_entry_entry_set(eo, str);
     }
};

template<> CEvasObject::CPropHandler<CElmEntry>::property_list
CEvasObject::CPropHandler<CElmEntry>::list[] = {
  { NULL, NULL, NULL },
};

class CElmCheck : public CEvasObject {
protected:
   static CPropHandler<CElmCheck> prop_handler;
   Persistent<Value> the_icon;

public:
   CElmCheck(CEvasObject *parent, Local<Object> obj) :
       CEvasObject()
     {
        eo = elm_check_add(parent->get());
        construct(eo, obj);
     }

   virtual ~CElmCheck()
     {
        the_icon.Dispose();
     }

   virtual Handle<ObjectTemplate> get_template(void)
     {
        Handle<ObjectTemplate> ot = CEvasObject::get_template();
        prop_handler.fill_template(ot);
        return ot;
     }

   virtual bool prop_set(const char *prop_name, Handle<Value> value)
     {
        CPropHandler<CElmCheck>::prop_setter setter;
        setter = prop_handler.get_setter(prop_name);
        if (setter)
          {
             (this->*setter)(value);
             return true;
          }
        return CEvasObject::prop_set(prop_name, value);
     }

   virtual Handle<Value> prop_get(const char *prop_name) const
     {
        CPropHandler<CElmCheck>::prop_getter getter;
        getter = prop_handler.get_getter(prop_name);
        if (getter)
          return (this->*getter)();
        return CEvasObject::prop_get(prop_name);
     }

   virtual void state_set(Handle<Value> value)
     {
        if (value->IsBoolean())
          elm_check_state_set(eo, value->BooleanValue());
     }

   virtual Handle<Value> state_get() const
     {
        return Boolean::New(elm_check_state_get(eo));
     }

   virtual Handle<Value> icon_get() const
     {
        return the_icon;
     }

   virtual void icon_set(Handle<Value> value)
     {
        the_icon.Dispose();
        CEvasObject *icon = realize_one(this, value);
        elm_check_icon_set(eo, icon->get());
        the_icon = Persistent<Value>::New(icon->get_object());
     }
};

template<> CEvasObject::CPropHandler<CElmCheck>::property_list
CEvasObject::CPropHandler<CElmCheck>::list[] = {
  PROP_HANDLER(CElmCheck, state),
  PROP_HANDLER(CElmCheck, icon),
  { NULL, NULL, NULL },
};

class CElmClock : public CEvasObject {
protected:
  static CPropHandler<CElmClock> prop_handler;

public:
  CElmClock(CEvasObject *parent, Local<Object> obj) :
       CEvasObject()
    {
       eo = elm_clock_add(parent->top_widget_get());
       construct(eo, obj);
    }

  virtual ~CElmClock()
    {
    }

  virtual Handle<ObjectTemplate> get_template(void)
    {
       Handle<ObjectTemplate> ot = CEvasObject::get_template();
       prop_handler.fill_template(ot);
       return ot;
    }

  virtual bool prop_set(const char *prop_name, Handle<Value> value)
    {
       CPropHandler<CElmClock>::prop_setter setter;
       setter = prop_handler.get_setter(prop_name);
       if (setter)
         {
            (this->*setter)(value);
            return true;
         }
       return CEvasObject::prop_set(prop_name, value);
    }

  virtual Handle<Value> prop_get(const char *prop_name) const
    {
       CPropHandler<CElmClock>::prop_getter getter;
       getter = prop_handler.get_getter(prop_name);
       if (getter)
         return (this->*getter)();
       return CEvasObject::prop_get(prop_name);
    }

  virtual Handle<Value> show_am_pm_get() const
    {
       Eina_Bool show_am_pm = elm_clock_show_am_pm_get(eo);
       return Boolean::New(show_am_pm);
    }

  virtual void show_am_pm_set(Handle<Value> val)
    {
       if (val->IsNumber())
	 {
            int value = val->ToNumber()->Value();
            elm_clock_show_am_pm_set(eo, value);
	 }
    }

  virtual Handle<Value> show_seconds_get() const
    {
       Eina_Bool show_seconds = elm_clock_show_seconds_get(eo);
       return Boolean::New(show_seconds);
    }

  virtual void show_seconds_set(Handle<Value> val)
    {
       if (val->IsNumber())
	 {
            int value = val->ToNumber()->Value();
            elm_clock_show_seconds_set(eo, value);
	 }
    }

  virtual Handle<Value> hour_get() const
    {
       int hour = 0;
       elm_clock_time_get(eo, &hour, NULL, NULL);
       return Number::New(hour);
    }

  virtual Handle<Value> minute_get() const
    {
       int minute = 0;
       elm_clock_time_get(eo, NULL, &minute, NULL);
       return Number::New(minute);
    }

  virtual Handle<Value> second_get() const
    {
       int second = 0;
       elm_clock_time_get(eo, NULL, NULL, &second);
       return Number::New(second);
    }

  virtual void hour_set(Handle<Value> val)
    {
       if (!val->IsNumber())
         {
            fprintf(stderr, "%s: value is not a Number!\n", __FUNCTION__);
            return;
         }
       int hour = 0;
       int minute = 0;
       int second = 0;
       // use either this or the class getters (involves conversion from Value to int)
       elm_clock_time_get(eo, &hour, &minute, &second);

       Local<Object> obj = val->ToObject();
       hour = val->ToNumber()->Value();
       elm_clock_time_set(eo, hour , minute, second);
    }

  virtual void minute_set(Handle<Value> val)
    {
       if (!val->IsNumber())
         {
            fprintf(stderr, "%s: value is not a Number!\n", __FUNCTION__);
            return;
         }
       int hour = 0;
       int minute = 0;
       int second = 0;
       elm_clock_time_get(eo, &hour, &minute, &second);
       Local<Object> obj = val->ToObject();
       minute = val->ToNumber()->Value();
       elm_clock_time_set(eo, hour , minute, second);
    }

  virtual void second_set(Handle<Value> val)
    {
       if (!val->IsNumber())
         {
            fprintf(stderr, "%s: value is not a Number!\n", __FUNCTION__);
            return;
         }
       int hour = 0;
       int minute = 0;
       int second = 0;
       elm_clock_time_get(eo, &hour, &minute, &second);
       Local<Object> obj = val->ToObject();
       second = val->ToNumber()->Value();
       elm_clock_time_set(eo, hour , minute, second);
    }

  virtual Handle<Value> edit_get() const
    {
       Eina_Bool editable = elm_clock_edit_get(eo);
       return Boolean::New(editable);
    }

  virtual void edit_set(Handle<Value> val)
    {
       if (val->IsBoolean())
         {
            Eina_Bool value = val->ToBoolean()->Value();
            elm_clock_edit_set(eo, value);
         }
    }
};

template<> CEvasObject::CPropHandler<CElmClock>::property_list
CEvasObject::CPropHandler<CElmClock>::list[] = {
  PROP_HANDLER(CElmClock, edit),
  PROP_HANDLER(CElmClock, hour),
  PROP_HANDLER(CElmClock, minute),
  PROP_HANDLER(CElmClock, second),
  PROP_HANDLER(CElmClock, show_seconds),
  PROP_HANDLER(CElmClock, show_am_pm),
  { NULL, NULL, NULL },
};

class CElmProgressBar : public CEvasObject {
protected:
   static CPropHandler<CElmProgressBar> prop_handler;
   Persistent<Value> the_icon;

   static Handle<Value> do_pulse(const Arguments& args)
     {
        CEvasObject *self = eo_from_info(args.This());
        CElmProgressBar *progress = static_cast<CElmProgressBar *>(self);
	if (args[0]->IsBoolean())
          progress->pulse(args[0]->BooleanValue());
        return Undefined();
     }

public:
   CElmProgressBar(CEvasObject *parent, Local<Object> obj) :
       CEvasObject()
     {
        eo = elm_progressbar_add(parent->get());
        construct(eo, obj);
        get_object()->Set(String::New("pulse"), FunctionTemplate::New(do_pulse)->GetFunction());
     }

   virtual ~CElmProgressBar()
     {
        the_icon.Dispose();
     }

   virtual Handle<ObjectTemplate> get_template(void)
     {
        Handle<ObjectTemplate> ot = CEvasObject::get_template();
        prop_handler.fill_template(ot);
        return ot;
     }

   virtual void pulse(bool on)
     {
        elm_progressbar_pulse(eo, on);
     }

   virtual bool prop_set(const char *prop_name, Handle<Value> value)
     {
        CPropHandler<CElmProgressBar>::prop_setter setter;
        setter = prop_handler.get_setter(prop_name);
        if (setter)
          {
             (this->*setter)(value);
             return true;
          }
        return CEvasObject::prop_set(prop_name, value);
     }

   virtual Handle<Value> prop_get(const char *prop_name) const
     {
        CPropHandler<CElmProgressBar>::prop_getter getter;
        getter = prop_handler.get_getter(prop_name);
        if (getter)
          return (this->*getter)();
        return CEvasObject::prop_get(prop_name);
     }

   virtual Handle<Value> icon_get() const
     {
        return the_icon;
     }

   virtual void icon_set(Handle<Value> value)
     {
        the_icon.Dispose();
        CEvasObject *icon = realize_one(this, value);
        elm_progressbar_icon_set(eo, icon->get());
        the_icon = Persistent<Value>::New(icon->get_object());
     }

   virtual Handle<Value> inverted_get() const
     {
        return Boolean::New(elm_progressbar_inverted_get(eo));
     }

   virtual void inverted_set(Handle<Value> value)
     {
        if (value->IsBoolean())
          elm_progressbar_inverted_set(eo, value->BooleanValue());
     }

   virtual Handle<Value> horizontal_get() const
     {
        return Boolean::New(elm_progressbar_horizontal_get(eo));
     }

   virtual void horizontal_set(Handle<Value> value)
     {
        if (value->IsBoolean())
          elm_progressbar_horizontal_set(eo, value->BooleanValue());
     }

   virtual Handle<Value> units_get() const
     {
        return String::New(elm_progressbar_unit_format_get(eo));
     }

   virtual void units_set(Handle<Value> value)
     {
        if (value->IsString())
          {
             String::Utf8Value str(value);
             elm_progressbar_unit_format_set(eo, *str);
          }
     }

   virtual Handle<Value> span_get() const
     {
        return Integer::New(elm_progressbar_span_size_get(eo));
     }

   virtual void span_set(Handle<Value> value)
     {
        if (value->IsInt32())
          {
             int span = value->Int32Value();
             elm_progressbar_span_size_set(eo, span);
          }
     }

   virtual Handle<Value> pulser_get() const
     {
        return Boolean::New(elm_progressbar_pulse_get(eo));
     }

   virtual void pulser_set(Handle<Value> value)
     {
        if (value->IsBoolean())
          elm_progressbar_pulse_set(eo, value->BooleanValue());
     }

   virtual Handle<Value> value_get() const
     {
        return Number::New(elm_progressbar_value_get(eo));
     }

   virtual void value_set(Handle<Value> value)
     {
        if (value->IsNumber())
          elm_progressbar_value_set(eo, value->NumberValue());
     }
};

template<> CEvasObject::CPropHandler<CElmProgressBar>::property_list
CEvasObject::CPropHandler<CElmProgressBar>::list[] = {
  PROP_HANDLER(CElmProgressBar, icon),
  PROP_HANDLER(CElmProgressBar, inverted),
  PROP_HANDLER(CElmProgressBar, horizontal),
  PROP_HANDLER(CElmProgressBar, units),
  PROP_HANDLER(CElmProgressBar, span),
  PROP_HANDLER(CElmProgressBar, pulser),
  PROP_HANDLER(CElmProgressBar, value),
  { NULL, NULL, NULL },
};


class CElmPhoto : public CEvasObject {
protected:
  static CPropHandler<CElmPhoto> prop_handler;

public:
  CElmPhoto(CEvasObject *parent, Local<Object> obj) : CEvasObject()
    {
       eo = elm_photo_add(parent->top_widget_get());
       construct(eo, obj);
    }

  virtual ~CElmPhoto()
    {
    }

  virtual Handle<ObjectTemplate> get_template(void)
    {
       Handle<ObjectTemplate> ot = CEvasObject::get_template();
       prop_handler.fill_template(ot);
       return ot;
    }

  virtual bool prop_set(const char *prop_name, Handle<Value> value)
    {
       CPropHandler<CElmPhoto>::prop_setter setter;
       setter = prop_handler.get_setter(prop_name);
       if (setter)
         {
            (this->*setter)(value);
            return true;
         }
       return CEvasObject::prop_set(prop_name, value);
    }

  virtual Handle<Value> prop_get(const char *prop_name) const
    {
       CPropHandler<CElmPhoto>::prop_getter getter;
       getter = prop_handler.get_getter(prop_name);
       if (getter)
         return (this->*getter)();
       return CEvasObject::prop_get(prop_name);
    }

  virtual Handle<Value> image_get() const
    {
       return Undefined();
    }

  virtual void image_set(Handle<Value> val)
    {
       if (val->IsString())
         {
            String::Utf8Value str(val);

            if (0 > access(*str, R_OK))
              fprintf(stderr, "warning: can't read image file %s\n", *str);

            Eina_Bool retval = elm_photo_file_set(eo, *str);
            if (retval == EINA_FALSE)
              fprintf(stderr, "Unable to set the image\n");
         }
    }

  virtual Handle<Value> size_get() const
    {
       return Undefined();
    }

  virtual void size_set(Handle<Value> val)
    {
       if (val->IsNumber())
         {
            int size = val->ToInt32()->Value();
            elm_photo_size_set(eo, size);
         }
    }

  virtual Handle<Value> fill_get() const
    {
       return Undefined();
    }

  virtual void fill_set(Handle<Value> val)
    {
       if (val->IsBoolean())
         elm_photo_fill_inside_set(eo, val->BooleanValue());
    }
};

template<> CEvasObject::CPropHandler<CElmPhoto>::property_list
CEvasObject::CPropHandler<CElmPhoto>::list[] = {
  PROP_HANDLER(CElmPhoto, size),
  PROP_HANDLER(CElmPhoto, fill),
  { NULL, NULL, NULL },
};

class CElmSpinner : public CEvasObject {
protected:
  static CPropHandler<CElmSpinner> prop_handler;

public:
  CElmSpinner(CEvasObject *parent, Local<Object> obj) : CEvasObject()
    {
       eo = elm_spinner_add(parent->top_widget_get());
       construct(eo, obj);
    }

  virtual ~CElmSpinner()
    {
    }

  virtual Handle<ObjectTemplate> get_template(void)
    {
       Handle<ObjectTemplate> ot = CEvasObject::get_template();
       prop_handler.fill_template(ot);
       return ot;
    }

  virtual bool prop_set(const char *prop_name, Handle<Value> value)
    {
       CPropHandler<CElmSpinner>::prop_setter setter;
       setter = prop_handler.get_setter(prop_name);
       if (setter)
         {
            (this->*setter)(value);
            return true;
         }
       return CEvasObject::prop_set(prop_name, value);
    }

  virtual Handle<Value> prop_get(const char *prop_name) const
    {
       CPropHandler<CElmSpinner>::prop_getter getter;
       getter = prop_handler.get_getter(prop_name);
       if (getter)
         return (this->*getter)();
       return CEvasObject::prop_get(prop_name);
    }

  virtual Handle<Value> label_format_get() const
    {
       return Undefined();
    }

  virtual void label_format_set(Handle<Value> val)
    {
       if (val->IsString())
         {
            String::Utf8Value str(val);

            elm_spinner_label_format_set(eo, *str);
         }
    }

  virtual Handle<Value> step_get() const
    {
       return Undefined();
    }

  virtual void step_set(Handle<Value> val)
    {
       if (val->IsNumber())
         {
            int size = val->ToNumber()->Value();
            elm_spinner_step_set(eo, size);
         }
    }

   virtual Handle<Value> min_get() const
     {
        double min, max;
        elm_spinner_min_max_get(eo, &min, &max);
        return Number::New(min);
     }

   virtual void min_set(Handle<Value> value)
     {
        if (value->IsNumber())
          {
             double min, max;
             elm_spinner_min_max_get(eo, &min, &max);
             min = value->NumberValue();
             elm_spinner_min_max_set(eo, min, max);
          }
     }

   virtual Handle<Value> max_get() const
     {
        double min, max;
        elm_spinner_min_max_get(eo, &min, &max);
        return Number::New(max);
     }

   virtual void max_set(Handle<Value> value)
     {
        if (value->IsNumber())
          {
             double min, max;
             elm_spinner_min_max_get(eo, &min, &max);
             max = value->NumberValue();
             elm_spinner_min_max_set(eo, min, max);
          }
     }
  virtual Handle<Value> style_get() const
    {
       return Undefined();
    }

  virtual void style_set(Handle<Value> val)
    {
       if (val->IsString())
         {
            String::Utf8Value str(val);
            elm_object_style_set(eo, *str);
         }
    }

  virtual Handle<Value> editable_get() const
    {
       return Undefined();
    }

  virtual void editable_set(Handle<Value> val)
    {
       if (val->IsBoolean())
         {
            elm_spinner_editable_set(eo, val->BooleanValue());
         }
    }

  virtual Handle<Value> disabled_get() const
    {
       return Undefined();
    }

  virtual void disabled_set(Handle<Value> val)
    {
       if (val->IsBoolean())
         {
            elm_object_disabled_set(eo, val->BooleanValue());
         }
    }

  virtual Handle<Value> special_value_get() const
    {
       return Undefined();
    }

  virtual void special_value_set(Handle<Value> val)
    {
       if (val->IsObject())
         {
             Local<Value> value = val->ToObject()->Get(String::New("value"));
             Local<Value> label = val->ToObject()->Get(String::New("label"));
             String::Utf8Value str(label);
             int int_value = value->ToInt32()->Value();
             elm_spinner_special_value_add(eo, int_value, *str);
         }
    }
};

template<> CEvasObject::CPropHandler<CElmSpinner>::property_list
CEvasObject::CPropHandler<CElmSpinner>::list[] = {
  PROP_HANDLER(CElmSpinner, label_format),
  PROP_HANDLER(CElmSpinner, step),
  PROP_HANDLER(CElmSpinner, min),
  PROP_HANDLER(CElmSpinner, max),
  PROP_HANDLER(CElmSpinner, style),
  PROP_HANDLER(CElmSpinner, disabled),
  PROP_HANDLER(CElmSpinner, editable),
  PROP_HANDLER(CElmSpinner, special_value),
  { NULL, NULL, NULL },
};

class CElmPane : public CEvasObject {
protected:
  static CPropHandler<CElmPane> prop_handler;

public:
  CElmPane(CEvasObject *parent, Local<Object> obj) : CEvasObject()
    {
       eo = elm_panes_add(parent->top_widget_get());
       construct(eo, obj);
       CEvasObject *left, *right;
       left = realize_one(this, obj->Get(String::New("content_left")));
       if (left)
         {
            elm_panes_content_left_set(eo, left->get());
	 }

       right = realize_one(this, obj->Get(String::New("content_right")));
       if (right)
	 {
            elm_panes_content_right_set(eo, right->get());
	 }
    }

  virtual ~CElmPane()
    {
    }

  virtual Handle<ObjectTemplate> get_template(void)
    {
       Handle<ObjectTemplate> ot = CEvasObject::get_template();
       prop_handler.fill_template(ot);
       return ot;
    }

  virtual bool prop_set(const char *prop_name, Handle<Value> value)
    {
       CPropHandler<CElmPane>::prop_setter setter;
       setter = prop_handler.get_setter(prop_name);
       if (setter)
         {
            (this->*setter)(value);
            return true;
         }
       return CEvasObject::prop_set(prop_name, value);
    }

  virtual Handle<Value> prop_get(const char *prop_name) const
    {
       CPropHandler<CElmPane>::prop_getter getter;
       getter = prop_handler.get_getter(prop_name);
       if (getter)
         return (this->*getter)();
       return CEvasObject::prop_get(prop_name);
    }

  virtual Handle<Value> horizontal_get() const
    {
       return Undefined();
    }

  virtual void horizontal_set(Handle<Value> val)
    {
       if (val->IsBoolean())
         {
            elm_panes_horizontal_set(eo, val->BooleanValue());
         }
    }

   virtual void on_press_set(Handle<Value> val)
     {
	on_clicked_set(val);
     }

   virtual Handle<Value> on_press_get(void) const
     {
        return on_clicked_val;
     }

};

template<> CEvasObject::CPropHandler<CElmPane>::property_list
CEvasObject::CPropHandler<CElmPane>::list[] = {
  PROP_HANDLER(CElmPane, horizontal),
  PROP_HANDLER(CElmPane, on_press),
  { NULL, NULL, NULL },
};

class CElmBubble : public CEvasObject {
protected:
  static CPropHandler<CElmBubble> prop_handler;

public:
  CElmBubble(CEvasObject *parent, Local<Object> obj) : CEvasObject()
    {
       eo = elm_bubble_add(parent->top_widget_get());
       construct(eo, obj);
       CEvasObject *content;
       content = realize_one(this, obj->Get(String::New("content")));
       if ( content )
         {
            elm_bubble_content_set(eo, content->get());
	 }
    }

  virtual ~CElmBubble()
    {
    }

  virtual Handle<ObjectTemplate> get_template(void)
    {
       Handle<ObjectTemplate> ot = CEvasObject::get_template();
       prop_handler.fill_template(ot);
       return ot;
    }

  virtual bool prop_set(const char *prop_name, Handle<Value> value)
    {
       CPropHandler<CElmBubble>::prop_setter setter;
       setter = prop_handler.get_setter(prop_name);
       if (setter)
         {
            (this->*setter)(value);
            return true;
         }
       return CEvasObject::prop_set(prop_name, value);
    }

  virtual Handle<Value> prop_get(const char *prop_name) const
    {
       CPropHandler<CElmBubble>::prop_getter getter;
       getter = prop_handler.get_getter(prop_name);
       if (getter)
         return (this->*getter)();
       return CEvasObject::prop_get(prop_name);
    }

  virtual Handle<Value> text_part_get() const
    {
       return Undefined();
    }

  virtual void text_part_set(Handle<Value> val)
    {
       if (!val->IsObject())
	 {
            fprintf(stderr, "%s: value is not an object!\n", __FUNCTION__);
            return;
	 }
       Local<Object> obj = val->ToObject();
       Local<Value> it = obj->Get(String::New("item"));
       Local<Value> lbl = obj->Get(String::New("label"));
       String::Utf8Value item(it);
       String::Utf8Value label(lbl);
       printf("Item = %s Label = %s\n", *item, *label);
       elm_object_text_part_set(eo, *item,*label);
    }


  virtual Handle<Value> corner_get() const
    {
       return Undefined();
    }

  virtual void corner_set(Handle<Value> val)
    {
       if (val->IsString())
         {
            String::Utf8Value str(val);
            elm_bubble_corner_set(eo, *str);
         }
    }

};

template<> CEvasObject::CPropHandler<CElmBubble>::property_list
CEvasObject::CPropHandler<CElmBubble>::list[] = {
  PROP_HANDLER(CElmBubble, text_part),
  PROP_HANDLER(CElmBubble, corner),
  { NULL, NULL, NULL },
};

class CElmSegment : public CEvasObject {
protected:
  static CPropHandler<CElmSegment> prop_handler;

public:
   CElmSegment(CEvasObject *parent, Local<Object> obj) :
       CEvasObject()
     {
        eo = elm_segment_control_add(parent->get());
        construct(eo, obj);
        items_set(obj->Get(String::New("items")));
     }

   Handle<Object> items_set(Handle<Value> val)
     {
        /* add an list of children */
        Local<Object> out = Object::New();

        if (!val->IsObject())
          {
             fprintf(stderr, "not an object!\n");
             return out;
          }

        Local<Object> in = val->ToObject();
        Local<Array> props = in->GetPropertyNames();

        /* iterate through elements and instantiate them */
        for (unsigned int i = 0; i < props->Length(); i++)
          {

             Local<Value> x = props->Get(Integer::New(i));
             String::Utf8Value val(x);

             Local<Value> item = in->Get(x->ToString());
             if (!item->IsObject())
               {
                  // FIXME: permit adding strings here?
                  fprintf(stderr, "list item is not an object\n");
                  continue;
               }
             Local<Value> label = item->ToObject()->Get(String::New("label"));

             String::Utf8Value str(label);
             elm_segment_control_item_add(eo, NULL, *str);
          }

        return out;
     }

   virtual ~CElmSegment()
     {
     }

  virtual bool prop_set(const char *prop_name, Handle<Value> value)
    {
       CPropHandler<CElmSegment>::prop_setter setter;
       setter = prop_handler.get_setter(prop_name);
       if (setter)
         {
            (this->*setter)(value);
            return true;
         }
       return CEvasObject::prop_set(prop_name, value);
    }

  virtual Handle<Value> prop_get(const char *prop_name) const
    {
       CPropHandler<CElmSegment>::prop_getter getter;
       getter = prop_handler.get_getter(prop_name);
       if (getter)
         return (this->*getter)();
       return CEvasObject::prop_get(prop_name);
    }
};

template<> CEvasObject::CPropHandler<CElmSegment>::property_list
CEvasObject::CPropHandler<CElmSegment>::list[] = {
  { NULL, NULL, NULL },
};

class CElmMenu : public CEvasObject {
protected:
  static CPropHandler<CElmMenu> prop_handler;

public:
  CElmMenu(CEvasObject *parent, Local<Object> obj) : CEvasObject()
    {
       eo = elm_menu_add(parent->top_widget_get());
       construct(eo, obj);
       items_set(obj->Get(String::New("items")), NULL);
    }

  virtual ~CElmMenu()
    {
    }

  Elm_Menu_Item * items_set(Handle<Value> val, Elm_Menu_Item *parent)
    {
       /* add a list of children */
       Local<Object> out = Object::New();

       if (!val->IsObject())
         {
            fprintf(stderr, "not an object!\n");
            return NULL;
         }

       Local<Object> in = val->ToObject();
       Local<Array> props = in->GetPropertyNames();
       Elm_Menu_Item *child = NULL;

       /* iterate through elements and instantiate them */
       for (unsigned int i = 0; i < props->Length(); i++)
         {
            Local<Value> x = props->Get(Integer::New(i));
            String::Utf8Value val(x);

            Local<Value> item = in->Get(x->ToString());
            if (!item->IsObject())
              {
                 fprintf(stderr, "List item is not an object!\n");
                 continue;
              }

            Local<Value> val1 = item->ToObject()->Get(String::New("label"));
            Local<Value> val2 = item->ToObject()->Get(String::New("icon"));

            if (!val1->IsString() && !val2->IsString())
              {
                 fprintf(stderr, "Empty menu item?\n");
                 continue;
              }

            String::Utf8Value label(val1->ToString());
            String::Utf8Value icon(val2->ToString());

            child = elm_menu_item_add(eo, parent, *icon, *label, NULL, NULL);

            Local<Value> val3 = item->ToObject()->Get(String::New("disabled"));
            if (val3->IsBoolean())
              elm_menu_item_disabled_set(child, val3->ToBoolean()->Value());

            Local<Value> val4 = item->ToObject()->Get(String::New("items"));
            if (val4->IsObject())
              items_set(val4, child);
         }
       return child;
    }

  virtual Handle<ObjectTemplate> get_template(void)
    {
       Handle<ObjectTemplate> ot = CEvasObject::get_template();
       prop_handler.fill_template(ot);
       return ot;
    }

  virtual bool prop_set(const char *prop_name, Handle<Value> value)
    {
       CPropHandler<CElmMenu>::prop_setter setter;
       setter = prop_handler.get_setter(prop_name);
       if (setter)
         {
            (this->*setter)(value);
            return true;
         }
       return CEvasObject::prop_set(prop_name, value);
    }

  virtual Handle<Value> prop_get(const char *prop_name) const
    {
       CPropHandler<CElmMenu>::prop_getter getter;
       getter = prop_handler.get_getter(prop_name);
       if (getter)
         return (this->*getter)();
       return CEvasObject::prop_get(prop_name);
    }

  virtual Handle<Value> move_get() const
    {
       return Undefined();
    }

  virtual void move_set(Handle<Value> val)
    {
        if (!val->IsObject())
          return;
        Local<Object> obj = val->ToObject();
        Local<Value> x = obj->Get(String::New("x"));
        Local<Value> y = obj->Get(String::New("y"));
        if (!x->IsNumber() || !y->IsNumber())
          return;
        Evas_Coord x_out = x->NumberValue();
        Evas_Coord y_out = y->NumberValue();
	elm_menu_move (eo, x_out, y_out);
    }
};

template<> CEvasObject::CPropHandler<CElmMenu>::property_list
CEvasObject::CPropHandler<CElmMenu>::list[] = {
  PROP_HANDLER(CElmMenu, move),
  { NULL, NULL, NULL },
};

class CElmColorSelector : public CEvasObject {
protected:
  static CPropHandler<CElmColorSelector> prop_handler;
   /* the on_clicked function */
   Persistent<Value> on_changed_val;

public:
  CElmColorSelector(CEvasObject *parent, Local<Object> obj) : CEvasObject()
    {
       eo = elm_colorselector_add(parent->top_widget_get());
       construct(eo, obj);
    }

  virtual ~CElmColorSelector()
    {
    }

  virtual Handle<ObjectTemplate> get_template(void)
    {
       Handle<ObjectTemplate> ot = CEvasObject::get_template();
       prop_handler.fill_template(ot);
       return ot;
    }

  virtual bool prop_set(const char *prop_name, Handle<Value> value)
    {
       CPropHandler<CElmColorSelector>::prop_setter setter;
       setter = prop_handler.get_setter(prop_name);
       if (setter)
         {
            (this->*setter)(value);
            return true;
         }
       return CEvasObject::prop_set(prop_name, value);
    }

  virtual Handle<Value> prop_get(const char *prop_name) const
    {
       CPropHandler<CElmColorSelector>::prop_getter getter;
       getter = prop_handler.get_getter(prop_name);
       if (getter)
         return (this->*getter)();
       return CEvasObject::prop_get(prop_name);
    }

  virtual Handle<Value> red_get() const
    {
       int r, g, b, a;
       elm_colorselector_color_get(eo, &r, &g, &b, &a);
       return Number::New(r);
    }

  virtual void red_set(Handle<Value> val)
    {
       if (val->IsNumber())
         {
            int r, g, b, a;
            elm_colorselector_color_get(eo, &r, &g, &b, &a);
	    r = val->ToNumber()->Value();
            elm_colorselector_color_set(eo, r, g, b, a);
         }
    }

  virtual Handle<Value> green_get() const
    {
       int r, g, b, a;
       elm_colorselector_color_get(eo, &r, &g, &b, &a);
       return Number::New(g);
    }

  virtual void green_set(Handle<Value> val)
    {
       if (val->IsNumber())
         {
            int r, g, b, a;
            elm_colorselector_color_get(eo, &r, &g, &b, &a);
            g = val->ToNumber()->Value();
            elm_colorselector_color_set(eo, r, g, b, a);
         }
    }
  virtual Handle<Value> blue_get() const
    {
       int r, g, b, a;
       elm_colorselector_color_get(eo, &r, &g, &b, &a);
       return Number::New(b);
    }

  virtual void blue_set(Handle<Value> val)
    {
       if (val->IsNumber())
         {
            int r, g, b, a;
            elm_colorselector_color_get(eo, &r, &g, &b, &a);
	    b = val->ToNumber()->Value();
            elm_colorselector_color_set(eo, r, g, b, a);
         }
    }
  virtual Handle<Value> alpha_get() const
    {
       int r, g, b, a;
       elm_colorselector_color_get(eo, &r, &g, &b, &a);
       return Number::New(a);
    }

  virtual void alpha_set(Handle<Value> val)
    {
       if (val->IsNumber())
         {
            int r, g, b, a;
            elm_colorselector_color_get(eo, &r, &g, &b, &a);
	    a = val->ToNumber()->Value();
            elm_colorselector_color_set(eo, r, g, b, a);
         }
    }
   virtual void on_changed(void *event_info)
     {
        Handle<Object> obj = get_object();
        HandleScope handle_scope;
        Handle<Value> val = on_changed_val;
        assert(val->IsFunction());
        Handle<Function> fn(Function::Cast(*val));
        Handle<Value> args[1] = { obj };
        fn->Call(fn, 1, args);
     }

   static void eo_on_changed(void *data, Evas_Object *eo, void *event_info)
     {
        CElmColorSelector *changed = static_cast<CElmColorSelector*>(data);
        changed->on_changed(event_info);
     }

   virtual void on_changed_set(Handle<Value> val)
     {
        on_changed_val.Dispose();
        on_changed_val = Persistent<Value>::New(val);
        if (val->IsFunction())
          evas_object_smart_callback_add(eo, "changed", &eo_on_changed, this);
        else
          evas_object_smart_callback_del(eo, "changed", &eo_on_changed);
     }

   virtual Handle<Value> on_changed_get(void) const
     {
        return on_changed_val;
     }

};

template<> CEvasObject::CPropHandler<CElmColorSelector>::property_list
CEvasObject::CPropHandler<CElmColorSelector>::list[] = {
  PROP_HANDLER(CElmColorSelector, red),
  PROP_HANDLER(CElmColorSelector, green),
  PROP_HANDLER(CElmColorSelector, blue),
  PROP_HANDLER(CElmColorSelector, alpha),
  PROP_HANDLER(CElmColorSelector, on_changed),
  { NULL, NULL, NULL },
};

class CElmCalendar : public CEvasObject {
protected:
  static CPropHandler<CElmCalendar> prop_handler;
  Persistent<Value> on_changed_val;

public:
  CElmCalendar(CEvasObject *parent, Local<Object> obj) : CEvasObject()
    {
       eo = elm_calendar_add(parent->top_widget_get());
       construct(eo, obj);
       marks_set(obj->Get(String::New("marks")));
    }

   Handle<Object> marks_set(Handle<Value> val)
     {
        /* add an list of children */
        Local<Object> out = Object::New();

        if (!val->IsObject())
          {
             fprintf(stderr, "not an object!\n");
             return out;
          }

        Local<Object> in = val->ToObject();
        Local<Array> props = in->GetPropertyNames();
        struct tm mark_time;

        /* iterate through elements and instantiate them */
        for (unsigned int i = 0; i < props->Length(); i++)
          {

             Local<Value> x = props->Get(Integer::New(i));
             String::Utf8Value val(x);

             Local<Value> item = in->Get(x->ToString());
             if (!item->IsObject())
               {
                  String::Utf8Value xval(x->ToString());
                  fprintf(stderr, "item is not an object %s\n", *xval);
                  continue;
               }

             Local<Value> type = item->ToObject()->Get(String::New("mark_type"));
             String::Utf8Value mark_type(type);
             Local<Value> date = item->ToObject()->Get(String::New("mark_date"));
             mark_time.tm_mday = date->ToNumber()->Value();
             Local<Value> mon = item->ToObject()->Get(String::New("mark_mon"));
             mark_time.tm_mon = mon->ToNumber()->Value();
             Local<Value> year = item->ToObject()->Get(String::New("mark_year"));
             mark_time.tm_year = year->ToNumber()->Value() - 1900;
             Local<Value> repeat = item->ToObject()->Get(String::New("mark_repeat"));
             String::Utf8Value mark_repeat(repeat);
             Elm_Calendar_Mark_Repeat intRepeat = ELM_CALENDAR_UNIQUE;

             if ( !strcasecmp(*mark_repeat, "annually"))
               intRepeat = ELM_CALENDAR_ANNUALLY;
             else if ( !strcasecmp(*mark_repeat, "monthly"))
               intRepeat = ELM_CALENDAR_MONTHLY;
             else if ( !strcasecmp(*mark_repeat, "weekly"))
               intRepeat = ELM_CALENDAR_WEEKLY;
             else if ( !strcasecmp(*mark_repeat, "daily"))
               intRepeat = ELM_CALENDAR_DAILY;

             elm_calendar_mark_add(eo, *mark_type, &mark_time, intRepeat);
             elm_calendar_marks_draw (eo);
             fprintf(stderr, "Showing marks %d %d %d\n", intRepeat, mark_time.tm_year, mark_time.tm_mon);
          }
        return out;
     }

   virtual Handle<ObjectTemplate> get_template(void)
     {
        Handle<ObjectTemplate> ot = CEvasObject::get_template();
        prop_handler.fill_template(ot);
        return ot;
     }

  virtual bool prop_set(const char *prop_name, Handle<Value> value)
    {
       CPropHandler<CElmCalendar>::prop_setter setter;
       setter = prop_handler.get_setter(prop_name);
       if (setter)
         {
            (this->*setter)(value);
            return true;
         }
       return CEvasObject::prop_set(prop_name, value);
    }

  virtual Handle<Value> prop_get(const char *prop_name) const
    {
       CPropHandler<CElmCalendar>::prop_getter getter;
       getter = prop_handler.get_getter(prop_name);
       if (getter)
         return (this->*getter)();
       return CEvasObject::prop_get(prop_name);
    }

   virtual void on_changed(void *event_info)
     {
        Handle<Object> obj = get_object();
        HandleScope handle_scope;
        Handle<Value> val = on_changed_val;
        assert(val->IsFunction());
        Handle<Function> fn(Function::Cast(*val));
        Handle<Value> args[1] = { obj };
        fn->Call(fn, 1, args);
     }

   static void eo_on_changed(void *data, Evas_Object *eo, void *event_info)
     {
        CElmCalendar *changed = static_cast<CElmCalendar*>(data);
        changed->on_changed(event_info);
     }

   virtual void on_changed_set(Handle<Value> val)
     {
        on_changed_val.Dispose();
        on_changed_val = Persistent<Value>::New(val);
        if (val->IsFunction())
          {
             evas_object_smart_callback_add(eo, "changed", &eo_on_changed, this);
             fprintf(stderr, "Setting callback\n");
          }
        else
          evas_object_smart_callback_del(eo, "changed", &eo_on_changed);
     }

   virtual Handle<Value> on_changed_get(void) const
     {
        return on_changed_val;
     }

   virtual Handle<Value> weekday_names_get(void) const
     {
        Local<Object> obj = Object::New();

        const char **wds = elm_calendar_weekdays_names_get(eo);
        obj->Set(String::New("0"), String::New(wds[0]));
        obj->Set(String::New("1"), String::New(wds[1]));
        obj->Set(String::New("2"), String::New(wds[2]));
        obj->Set(String::New("3"), String::New(wds[3]));
        obj->Set(String::New("4"), String::New(wds[4]));
        obj->Set(String::New("5"), String::New(wds[5]));
        obj->Set(String::New("6"), String::New(wds[6]));

        return obj;
     }

   virtual void weekday_names_set(Handle<Value> val)
     {
        if ( val->IsObject() )
          {
             const char *weekdays[7];
             Local<Object> obj = val->ToObject();

             for (int i = 0; i < 7 ; i++)
               {
                  char fill[2];
                  sprintf(fill, "%d", i);
                  Handle<Value> value = obj->Get(String::New(fill));
                  if ( value->IsString())
                    {
                       String::Utf8Value str(value);
                       weekdays[i] = strdup(*str);
                       fprintf(stderr, "%s Day of Week = %s\n",fill, weekdays[i]);
                    }
               }
             elm_calendar_weekdays_names_set(eo,weekdays);
          }
     }

   virtual Handle<Value> min_year_get(void) const
     {
        int year_min, year_max;
        elm_calendar_min_max_year_get(eo, &year_min, &year_max);
        return Number::New(year_min);
     }

   virtual void min_year_set(Handle<Value> val)
     {
        if ( val->IsNumber() )
          {
             int year_min, year_max;
             elm_calendar_min_max_year_get(eo, &year_min, &year_max);
             year_min = val->ToNumber()->Value();
             fprintf(stderr,"Setting min year = %d\n", year_min);
             elm_calendar_min_max_year_set(eo, year_min, year_max);
          }
     }

   virtual Handle<Value> max_year_get(void) const
     {
        int year_min, year_max;
        elm_calendar_min_max_year_get(eo, &year_min, &year_max);
        return Number::New(year_max);
     }

   virtual void max_year_set(Handle<Value> val)
     {
        if ( val->IsNumber() )
          {
             int year_min, year_max;
             elm_calendar_min_max_year_get(eo, &year_min, &year_max);
             year_max = val->ToNumber()->Value();
             fprintf(stderr,"Setting max year = %d\n", year_max);
             elm_calendar_min_max_year_set(eo, year_min, year_max);
          }
     }

   virtual Handle<Value> day_selection_enabled_get(void) const
     {
        Eina_Bool day_select = elm_calendar_day_selection_enabled_get(eo);
        return Boolean::New(day_select);
     }

   virtual void day_selection_enabled_set(Handle<Value> val)
     {
        if ( val->IsBoolean() )
          {
             int year_min, year_max;
             Eina_Bool day_select = val->ToBoolean()->Value();
             elm_calendar_day_selection_enabled_set(eo, day_select);
          }
     }

   virtual Handle<Value> selected_date_get(void) const
     {
        struct tm selected_time;
        elm_calendar_selected_time_get (eo,&selected_time);
        return Number::New(selected_time.tm_mday);
     }

   virtual void selected_date_set(Handle<Value> val)
     {
        if ( val->IsNumber() )
          {
             struct tm selected_time;
             elm_calendar_selected_time_get (eo,&selected_time);
             selected_time.tm_mday = val->ToNumber()->Value();
             fprintf(stderr,"Setting day = %d\n", selected_time.tm_mday);
             elm_calendar_selected_time_set (eo,&selected_time);
          }
     }

   virtual Handle<Value> selected_month_get(void) const
     {
        struct tm selected_time;
        elm_calendar_selected_time_get (eo,&selected_time);
        return Number::New(selected_time.tm_mon);
     }

   virtual void selected_month_set(Handle<Value> val)
     {
        if ( val->IsNumber() )
          {
             struct tm selected_time;
             elm_calendar_selected_time_get (eo,&selected_time);
             selected_time.tm_mon = val->ToNumber()->Value();
             //tm_mon is zero based - but hide that from user.
             //let them give a normal number
             selected_time.tm_mon = selected_time.tm_mon - 1;
             fprintf(stderr,"Setting month = %d\n", selected_time.tm_mon);
             elm_calendar_selected_time_set (eo,&selected_time);
          }
     }

   virtual Handle<Value> selected_year_get(void) const
     {
        struct tm selected_time;
        elm_calendar_selected_time_get (eo,&selected_time);
        return Number::New(selected_time.tm_year);
     }

   virtual void selected_year_set(Handle<Value> val)
     {
        if ( val->IsNumber() )
          {
             struct tm selected_time;
             elm_calendar_selected_time_get (eo,&selected_time);
             fprintf(stderr,"Setting year = %d\n", selected_time.tm_year);
             selected_time.tm_year = val->ToNumber()->Value();
             fprintf(stderr,"Setting year = %d\n", selected_time.tm_year);
             //tm_year is years since 1900 - but hide that from user.
             //let them give a normal year
             selected_time.tm_year = selected_time.tm_year - 1900;
             elm_calendar_selected_time_set (eo,&selected_time);
          }
     }

   virtual Handle<Value> calendar_interval_get(void) const
     {
        return Number::New(elm_calendar_interval_get(eo));
     }

   virtual void calendar_interval_set(Handle<Value> val)
     {
        if ( val->IsNumber() )
          {
             double interval = val->ToNumber()->Value();
             fprintf(stderr, "Set Interval = %f\n", interval);
             elm_calendar_interval_set(eo, interval);
          }
     }
};

template<> CEvasObject::CPropHandler<CElmCalendar>::property_list
CEvasObject::CPropHandler<CElmCalendar>::list[] = {
     PROP_HANDLER(CElmCalendar, weekday_names),
     PROP_HANDLER(CElmCalendar, min_year),
     PROP_HANDLER(CElmCalendar, max_year),
     PROP_HANDLER(CElmCalendar, day_selection_enabled),
     PROP_HANDLER(CElmCalendar, selected_date),
     PROP_HANDLER(CElmCalendar, selected_month),
     PROP_HANDLER(CElmCalendar, selected_year),
     PROP_HANDLER(CElmCalendar, calendar_interval),
     PROP_HANDLER(CElmCalendar, on_changed),
     { NULL, NULL, NULL },
};

class CElmTable : public CEvasObject {
protected:
  static CPropHandler<CElmTable> prop_handler;
   /* the on_clicked function */
   Persistent<Value> on_changed_val;

public:
   CElmTable(CEvasObject *parent, Local<Object> obj) : CEvasObject()
     {
        eo = elm_table_add(parent->top_widget_get());
        construct(eo, obj);
        items_set(obj->Get(String::New("subobjects")));
     }

   virtual void items_set(Handle<Value> val)
     {
        if (!val->IsObject())
          {
             fprintf(stderr, "not an object!\n");
             return;
          }

        Local<Object> in = val->ToObject();
        Local<Array> props = in->GetPropertyNames();

        /* iterate through elements and instantiate them */
        for (unsigned int i = 0; i < props->Length(); i++)
          {
             Local<Value> x = props->Get(Integer::New(i));
             String::Utf8Value val(x);

             Local<Value> item = in->Get(x->ToString());

             new_item_set(item);
          }
     }

   virtual void new_item_set(Handle<Value> item)
     {
        fprintf(stderr, "Add New Table SubObject\n");
        CEvasObject *child = NULL;
        if (!item->IsObject())
          {
             // FIXME: permit adding strings here?
             fprintf(stderr, "list item is not an object\n");
             return;
          }
        Local<Value> xpos = item->ToObject()->Get(String::New("x"));
        Local<Value> ypos = item->ToObject()->Get(String::New("y"));
        Local<Value> width = item->ToObject()->Get(String::New("w"));
        Local<Value> height = item->ToObject()->Get(String::New("h"));
        Local<Value> subobj = item->ToObject()->Get(String::New("subobject"));

        if ( subobj->IsObject())
          {
             child = realize_one(this, subobj);
             if(!child)
                return;
          }
        else
          {
             return;
          }

        int x,y,w,h;

        if (xpos->IsNumber())
          {
             x = xpos->IntegerValue();
             fprintf(stderr, "X = %d\n", x);
          }
        if (ypos->IsNumber())
          {
             y = ypos->IntegerValue();
             fprintf(stderr, "y = %d\n", y);
          }
        if (width->IsNumber())
          {
             w = width->IntegerValue();
             fprintf(stderr, "W = %d\n", w);
          }
        if (height->IsNumber())
          {
             h = height->IntegerValue();
             fprintf(stderr, "H = %d\n", h);
          }

        if ( ( x + y + w + h ) > 0 )
          {
             elm_table_pack (this->get(), child->get(), x, y, w, h);
          }
     }

   virtual ~CElmTable()
     {
     }

   virtual Handle<ObjectTemplate> get_template(void)
     {
        Handle<ObjectTemplate> ot = CEvasObject::get_template();
        prop_handler.fill_template(ot);
        return ot;
     }

   virtual bool prop_set(const char *prop_name, Handle<Value> value)
     {
        CPropHandler<CElmTable>::prop_setter setter;
        setter = prop_handler.get_setter(prop_name);
        if (setter)
          {
             (this->*setter)(value);
             return true;
          }
        return CEvasObject::prop_set(prop_name, value);
     }

   virtual Handle<Value> prop_get(const char *prop_name) const
     {
        CPropHandler<CElmTable>::prop_getter getter;
        getter = prop_handler.get_getter(prop_name);
        if (getter)
          return (this->*getter)();
        return CEvasObject::prop_get(prop_name);
     }

    void homogeneous_set(Handle<Value> val)
      {
         if (val->IsBoolean())
           elm_table_homogeneous_set(eo, val->BooleanValue());
      }

    virtual Handle<Value> homogeneous_get() const
      {
         return Boolean::New(elm_table_homogeneous_get(eo));
      }
};

template<> CEvasObject::CPropHandler<CElmTable>::property_list
CEvasObject::CPropHandler<CElmTable>::list[] = {
  { NULL, NULL, NULL },
};

CEvasObject *
realize_one(CEvasObject *parent, Handle<Value> object_val)
{
   if (!object_val->IsObject())
     {
        fprintf(stderr, "%s: value is not an object!\n", __FUNCTION__);
        return NULL;
     }

   Local<Object> obj = object_val->ToObject();

   Local<Value> val = obj->Get(String::New("type"));
   String::Utf8Value str(val);

   /* create the evas object */
   // FIXME: make a list here
   CEvasObject *eo = NULL;
   if (!strcmp(*str, "actionslider"))
     eo = new CElmActionSlider(parent, obj);
   else if (!strcmp(*str, "button"))
     eo = new CElmButton(parent, obj);
   else if (!strcmp(*str, "background"))
     eo = new CElmBackground(parent, obj);
   else if (!strcmp(*str, "check"))
     eo = new CElmCheck(parent, obj);
   else if (!strcmp(*str, "clock"))
     eo = new CElmClock(parent, obj);
   else if (!strcmp(*str, "entry"))
     eo = new CElmEntry(parent, obj);
   else if (!strcmp(*str, "flip"))
     eo = new CElmFlip(parent, obj);
   else if (!strcmp(*str, "list"))
     eo = new CElmList(parent, obj);
   else if (!strcmp(*str, "genlist"))
     eo = new CElmGenList(parent, obj);
   else if (!strcmp(*str, "icon"))
     eo = new CElmIcon(parent, obj);
   else if (!strcmp(*str, "label"))
     eo = new CElmLabel(parent, obj);
   else if (!strcmp(*str, "radio"))
     eo = new CElmRadio(parent, obj);
   else if (!strcmp(*str, "box"))
     eo = new CElmBox(parent, obj);
   else if (!strcmp(*str, "progressbar"))
     eo = new CElmProgressBar(parent, obj);
   else if (!strcmp(*str, "scroller"))
     eo = new CElmScroller(parent, obj);
   else if (!strcmp(*str, "segment"))
     eo = new CElmSegment(parent, obj);
   else if (!strcmp(*str, "image"))
     eo = new CEvasImage(parent, obj);
   else if (!strcmp(*str, "slider"))
     eo = new CElmSlider(parent, obj);
   else if (!strcmp(*str, "photo"))
     eo = new CElmPhoto(parent,obj);
   else if (!strcmp(*str, "spinner"))
     eo = new CElmSpinner(parent,obj);
   else if (!strcmp(*str, "pane"))
     eo = new CElmPane(parent,obj);
   else if (!strcmp(*str, "bubble"))
     eo = new CElmBubble(parent,obj);
   else if (!strcmp(*str, "menu"))
     eo = new CElmMenu(parent,obj);
   else if (!strcmp(*str, "colorselector"))
     eo = new CElmColorSelector(parent,obj);
   else if (!strcmp(*str, "calendar"))
     eo = new CElmCalendar(parent,obj);
   else if (!strcmp(*str, "table"))
     eo = new CElmTable(parent,obj);

   if (!eo)
     {
        fprintf(stderr, "Bad object type %s\n", *str);
        return eo;
     }

   return eo;
}

Handle<Value>
elm_main_window(const Arguments& args)
{
   if (args.Length() != 1)
     return ThrowException(Exception::Error(String::New("Bad parameters")));

   if (!args[0]->IsObject())
     return Undefined();

   main_win = new CElmBasicWindow(NULL, args[0]->ToObject());
   if (!main_win)
     return Undefined();

   return main_win->get_object();
}

Handle<Value>
elm_loop_time(const Arguments& args)
{
   return Number::New(ecore_loop_time_get());
}

Handle<Value>
elm_exit(const Arguments& args)
{
   elm_exit();
   return Undefined();
}

Persistent<Value> the_datadir;

Handle<Value>
datadir_getter(Local<String> property, const AccessorInfo& info)
{
   return the_datadir;
}

void
datadir_setter(Local<String> property, Local<Value> value,
               const AccessorInfo& info)
{
   the_datadir.Dispose();
   the_datadir = Persistent<Value>::New(value);
}

void
elm_v8_setup(Handle<ObjectTemplate> global)
{
   Handle<ObjectTemplate> elm = ObjectTemplate::New();
   global->Set(String::New("elm"), elm);

   elm->Set(String::New("window"), FunctionTemplate::New(elm_main_window));
   elm->Set(String::New("loop_time"), FunctionTemplate::New(elm_loop_time));
   elm->Set(String::New("exit"), FunctionTemplate::New(elm_exit));
   elm->SetAccessor(String::New("datadir"), datadir_getter, datadir_setter);

   /* setup data directory */
   the_datadir = Persistent<String>::New(String::New(PACKAGE_DATA_DIR "/" ));
}

void
elm_v8_shutdown(void)
{
   the_datadir.Dispose();
}
