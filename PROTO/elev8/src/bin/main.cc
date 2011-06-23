/*
 * elev8 - javascript for EFL
 *
 * The script's job is to prepare for the main loop to run
 * then exit
 */

#include "main.h"

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

/* CEvasObject is a virtual class, representing an evas object */
class CEvasObject;
CEvasObject *realize_one(CEvasObject *parent, v8::Handle<v8::Value> obj);

class CEvasObject {
   /* realize_one is a factory for our class */
   friend CEvasObject *realize_one(CEvasObject *parent, v8::Handle<v8::Value> obj);
protected:
   Evas_Object *eo;
   v8::Persistent<v8::ObjectTemplate> the_template;
   v8::Persistent<v8::Object> the_object;

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
   v8::Persistent<v8::Value> on_clicked_val;

   /* the animator function and its hook into ecore */
   v8::Persistent<v8::Value> on_animate_val;
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
     typedef v8::Handle<v8::Value> (X::*prop_getter)(void) const;
     typedef void (X::*prop_setter)(v8::Handle<v8::Value> val);

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
     void fill_template(v8::Handle<v8::ObjectTemplate> &ot)
       {
          for (property_list *prop = list; prop->name; prop++)
            ot->SetAccessor(v8::String::New(prop->name), &eo_getter, &eo_setter);
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
   void construct(Evas_Object *_eo, v8::Local<v8::Object> obj)
     {
        eo = _eo;
        assert(eo != NULL);

        v8::Handle<v8::Object> out = get_object();

        /* copy properties, one by one */
        v8::Handle<v8::Array> props = obj->GetPropertyNames();
        for (unsigned int i = 0; i < props->Length(); i++)
          {
             v8::Local<v8::Value> name = props->Get(v8::Integer::New(i));
             v8::String::Utf8Value name_str(name);

             /* skip the type property */
             if (!strcmp(*name_str, "type"))
               continue;

             v8::Local<v8::Value> value = obj->Get(name->ToString());
             init_property(out, name, value);
          }

        /* show the object, maybe */
        v8::Local<v8::Value> hidden = obj->Get(v8::String::New("hidden"));
        if (!hidden->IsTrue())
          show();
     }

   virtual void add_child(CEvasObject *child)
     {
     }

   void object_set_eo(v8::Handle<v8::Object> obj, CEvasObject *eo)
     {
        obj->Set(v8::String::New("_eo"), v8::External::Wrap(eo));
     }

   static CEvasObject *eo_from_info(v8::Handle<v8::Object> obj)
     {
        v8::Handle<v8::Value> val = obj->Get(v8::String::New("_eo"));
        return static_cast<CEvasObject *>(v8::External::Unwrap(val));
     }

   static void eo_setter(v8::Local<v8::String> property,
                         v8::Local<v8::Value> value,
                         const v8::AccessorInfo& info)
     {
        CEvasObject *eo = eo_from_info(info.This());
        v8::String::Utf8Value prop_name(property);
        eo->prop_set(*prop_name, value);
        v8::String::Utf8Value val(value->ToString());
     }

   static v8::Handle<v8::Value> eo_getter(v8::Local<v8::String> property,
                                          const v8::AccessorInfo& info)
     {
        CEvasObject *eo = eo_from_info(info.This());
        v8::String::Utf8Value prop_name(property);
        return eo->prop_get(*prop_name);
     }

   /* setup the property on construction */
   virtual void init_property(v8::Handle<v8::Object> out,
			 v8::Handle<v8::Value> name,
			 v8::Handle<v8::Value> value)
     {
        v8::String::Utf8Value name_str(name);

        /* set or copy the property */
        if (!prop_set(*name_str, value))
          out->Set(name, value);
     }

public:
   virtual v8::Handle<v8::ObjectTemplate> get_template(void)
     {
        /* FIXME: only need to create one template per object class */
        the_template = v8::Persistent<v8::ObjectTemplate>::New(v8::ObjectTemplate::New());

        prop_handler.fill_template(the_template);

        return the_template;
     }

   virtual v8::Handle<v8::Object> get_object(void)
     {
        if (the_object.IsEmpty())
          {
             v8::Handle<v8::ObjectTemplate> ot = get_template();
             the_object = v8::Persistent<v8::Object>::New(ot->NewInstance());
             object_set_eo(the_object, this);

             /* FIXME: make handle a weak handle, and detect destruction */
          }
        return the_object;
     }

   virtual v8::Handle<v8::Value> type_get(void) const
     {
        fprintf(stderr, "undefined object type!\n");
        return v8::Undefined();
     }

   virtual void type_set(v8::Handle<v8::Value> value)
     {
        fprintf(stderr, "type cannot be set!\n");
     }

   Evas_Object *get() const
     {
        return eo;
     }

   /*
    * set a property
    * return true if successful, false if not
    */
   virtual bool prop_set(const char *prop_name, v8::Handle<v8::Value> value)
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

   virtual v8::Handle<v8::Value> prop_get(const char *prop_name) const
     {
        CPropHandler<CEvasObject>::prop_getter getter;
        getter = prop_handler.get_getter(prop_name);
        if (getter)
          return (this->*getter)();
        return v8::Undefined();
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
        eo = NULL;
     }

   virtual void x_set(v8::Handle<v8::Value> val)
     {
       if (val->IsNumber())
         {
            Evas_Coord x, y, width, height;
            evas_object_geometry_get(eo, &x, &y, &width, &height);
            x = val->ToInt32()->Value();
            evas_object_move(eo, x, y);
         }
     }

   virtual v8::Handle<v8::Value> x_get(void) const
     {
       Evas_Coord x, y, width, height;
       evas_object_geometry_get(eo, &x, &y, &width, &height);
       return v8::Number::New(x);
     }

   virtual void y_set(v8::Handle<v8::Value> val)
     {
       if (val->IsNumber())
         {
            Evas_Coord x, y, width, height;
            evas_object_geometry_get(eo, &x, &y, &width, &height);
            y = val->ToInt32()->Value();
            evas_object_move(eo, x, y);
         }
     }

   virtual v8::Handle<v8::Value> y_get(void) const
     {
       Evas_Coord x, y, width, height;
       evas_object_geometry_get(eo, &x, &y, &width, &height);
       return v8::Number::New(y);
     }

   virtual void height_set(v8::Handle<v8::Value> val)
     {
       if (val->IsNumber())
         {
           Evas_Coord x, y, width, height;
           evas_object_geometry_get(eo, &x, &y, &width, &height);
           height = val->ToInt32()->Value();
           evas_object_resize(eo, width, height);
         }
     }

   virtual v8::Handle<v8::Value> height_get(void) const
     {
       Evas_Coord x, y, width, height;
       evas_object_geometry_get(eo, &x, &y, &width, &height);
       return v8::Number::New(height);
     }

   virtual void width_set(v8::Handle<v8::Value> val)
     {
       if (val->IsNumber())
         {
           Evas_Coord x, y, width, height;
           evas_object_geometry_get(eo, &x, &y, &width, &height);
           width = val->ToInt32()->Value();
           evas_object_resize(eo, width, height);
         }
     }

   virtual v8::Handle<v8::Value> width_get(void) const
     {
       Evas_Coord x, y, width, height;
       evas_object_geometry_get(eo, &x, &y, &width, &height);
       return v8::Number::New(width);
     }

   void move(v8::Local<v8::Value> x, v8::Local<v8::Value> y)
     {
        if (x->IsNumber() && y->IsNumber())
          evas_object_move(eo, x->Int32Value(), y->Int32Value());
     }

   virtual void on_click(void *event_info)
     {
        v8::Handle<v8::Object> obj = get_object();
        v8::HandleScope handle_scope;
        v8::Handle<v8::Value> val = on_clicked_val;
        // FIXME: pass event_info to the callback
        // FIXME: turn the pieces below into a do_callback method
        assert(val->IsFunction());
        v8::Handle<v8::Function> fn(v8::Function::Cast(*val));
        v8::Handle<v8::Value> args[1] = { obj };
        fn->Call(fn, 1, args);
     }

   static void eo_on_click(void *data, Evas_Object *eo, void *event_info)
     {
        CEvasObject *clicked = static_cast<CEvasObject*>(data);

        clicked->on_click(event_info);
     }

   virtual void on_animate_set(v8::Handle<v8::Value> val)
     {
        on_animate_val.Dispose();
        on_animate_val = v8::Persistent<v8::Value>::New(val);
        if (val->IsFunction())
          current_animator = ecore_animator_add(&eo_on_animate, this);
        else if (current_animator)
          {
             ecore_animator_del(current_animator);
             current_animator = NULL;
          }
     }

   virtual v8::Handle<v8::Value> on_animate_get(void) const
     {
        return on_animate_val;
     }

   virtual void on_clicked_set(v8::Handle<v8::Value> val)
     {
        on_clicked_val.Dispose();
        on_clicked_val = v8::Persistent<v8::Value>::New(val);
        if (val->IsFunction())
          evas_object_smart_callback_add(eo, "clicked", &eo_on_click, this);
        else
          evas_object_smart_callback_del(eo, "clicked", &eo_on_click);
     }

   virtual v8::Handle<v8::Value> on_clicked_get(void) const
     {
        return on_animate_val;
     }

   virtual void on_animate(void)
     {
        v8::Handle<v8::Object> obj = get_object();
        v8::HandleScope handle_scope;
        v8::Handle<v8::Value> val = on_animate_val;
        assert(val->IsFunction());
        v8::Handle<v8::Function> fn(v8::Function::Cast(*val));
        v8::Handle<v8::Value> args[1] = { obj };
        fn->Call(fn, 1, args);
     }

   static Eina_Bool eo_on_animate(void *data)
     {
        CEvasObject *clicked = static_cast<CEvasObject*>(data);

        clicked->on_animate();

        return ECORE_CALLBACK_RENEW;
     }

   virtual v8::Handle<v8::Value> label_get() const
     {
       return v8::Undefined();
     }

   virtual void label_set(v8::Handle<v8::Value> val)
     {
        if (val->IsString() || val->IsNumber())
          {
             v8::String::Utf8Value str(val);
             label_set(*str);
          }
     }

   virtual void label_set(const char *str)
     {
        fprintf(stderr, "setting label on non-button\n");
     }

   virtual v8::Handle<v8::Value> disabled_get() const
     {
        return v8::Boolean::New(elm_object_disabled_get(eo));
     }

   virtual void disabled_set(v8::Handle<v8::Value> val)
     {
        if (val->IsBoolean())
          elm_object_disabled_set(eo, val->BooleanValue());
     }

   virtual v8::Handle<v8::Value> scale_get() const
     {
        return v8::Number::New(elm_object_scale_get(eo));
     }

   virtual void scale_set(v8::Handle<v8::Value> val)
     {
        if (val->IsNumber())
          elm_object_scale_set(eo, val->NumberValue());
     }

   bool get_xy_from_object(v8::Handle<v8::Value> val, double &x_out, double &y_out)
     {
        v8::HandleScope handle_scope;
        if (!val->IsObject())
          return false;
        v8::Local<v8::Object> obj = val->ToObject();
        v8::Local<v8::Value> x = obj->Get(v8::String::New("x"));
        v8::Local<v8::Value> y = obj->Get(v8::String::New("y"));
        if (!x->IsNumber() || !y->IsNumber())
          return false;
        x_out = x->NumberValue();
        y_out = y->NumberValue();
        return true;
     }

   bool get_xy_from_object(v8::Handle<v8::Value> val, bool &x_out, bool &y_out)
     {
        v8::HandleScope handle_scope;
        if (!val->IsObject())
          return false;
        v8::Local<v8::Object> obj = val->ToObject();
        v8::Local<v8::Value> x = obj->Get(v8::String::New("x"));
        v8::Local<v8::Value> y = obj->Get(v8::String::New("y"));
        if (!x->IsBoolean() || !y->IsBoolean())
          return false;
        x_out = x->BooleanValue();
        y_out = y->BooleanValue();
        return true;
     }

   bool get_xy_from_object(v8::Handle<v8::Value> val, int &x_out, int &y_out)
     {
        v8::HandleScope handle_scope;
        if (!val->IsObject())
          return false;
        v8::Local<v8::Object> obj = val->ToObject();
        v8::Local<v8::Value> x = obj->Get(v8::String::New("x"));
        v8::Local<v8::Value> y = obj->Get(v8::String::New("y"));
        if (!x->IsInt32() || !y->IsInt32())
          return false;
        x_out = x->Int32Value();
        y_out = y->Int32Value();
        return true;
     }

    bool get_xy_from_object(v8::Handle<v8::Value> val,
			   v8::Handle<v8::Value> &x_val,
			   v8::Handle<v8::Value> &y_val)
     {
        v8::HandleScope handle_scope;
        if (!val->IsObject())
          return false;
        v8::Local<v8::Object> obj = val->ToObject();
        x_val = obj->Get(v8::String::New("x"));
        y_val = obj->Get(v8::String::New("y"));
        return true;
     }

   virtual void weight_set(v8::Handle<v8::Value> weight)
     {
        double x, y;
        if (get_xy_from_object(weight, x, y))
          evas_object_size_hint_weight_set(eo, x, y);
     }

   virtual v8::Handle<v8::Value> weight_get(void) const
     {
       double x = 0.0, y = 0.0;
       evas_object_size_hint_weight_get(eo, &x, &y);
       v8::Local<v8::Object> obj = v8::Object::New();
       obj->Set(v8::String::New("x"), v8::Number::New(x));
       obj->Set(v8::String::New("y"), v8::Number::New(y));
       return obj;
     }

   virtual void align_set(v8::Handle<v8::Value> align)
     {
        double x, y;
        if (get_xy_from_object(align, x, y))
          {
             evas_object_size_hint_align_set(eo, x, y);
          }
     }

   virtual v8::Handle<v8::Value> align_get(void) const
     {
       double x, y;
       evas_object_size_hint_align_get(eo, &x, &y);
       v8::Local<v8::Object> obj = v8::Object::New();
       obj->Set(v8::String::New("x"), v8::Number::New(x));
       obj->Set(v8::String::New("y"), v8::Number::New(y));
       return obj;
     }

   virtual void image_set(v8::Handle<v8::Value> val)
     {
        if (val->IsString())
          fprintf(stderr, "no image set\n");
     }

   virtual v8::Handle<v8::Value> image_get(void) const
     {
        return v8::Undefined();
     }

   virtual void show()
     {
        evas_object_show(eo);
     }

   /* returns a list of children in an object */
   v8::Handle<v8::Object> realize_objects(v8::Handle<v8::Value> val)
     {
        /* add an list of children */
        v8::Local<v8::Object> out = v8::Object::New();

        if (!val->IsObject())
          {
             fprintf(stderr, "not an object!\n");
             return out;
          }

        v8::Handle<v8::Object> in = val->ToObject();
        v8::Handle<v8::Array> props = in->GetPropertyNames();

        /* iterate through elements and instantiate them */
        for (unsigned int i = 0; i < props->Length(); i++)
          {

             v8::Handle<v8::Value> x = props->Get(v8::Integer::New(i));
             v8::String::Utf8Value val(x);

             CEvasObject *child = realize_one(this, in->Get(x->ToString()));
             if (!child)
               continue;
             add_child(child);

             v8::Handle<v8::Object> child_obj = child->get_object();
             out->Set(x, child_obj);
          }

        return out;
     }

   /* resize this object when the parent resizes? */
   virtual void resize_set(v8::Handle<v8::Value> val)
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

   virtual v8::Handle<v8::Value> resize_get(void) const
     {
        return v8::Boolean::New(is_resize);
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
     PROP_HANDLER(CEvasObject, type),
     PROP_HANDLER(CEvasObject, resize),
     PROP_HANDLER(CEvasObject, align),
     PROP_HANDLER(CEvasObject, weight),
     PROP_HANDLER(CEvasObject, on_animate),
     PROP_HANDLER(CEvasObject, on_clicked),
     PROP_HANDLER(CEvasObject, scale),
     { NULL, NULL, NULL },
};

class CEvasImage : public CEvasObject {
public:
   CEvasImage(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
        Evas *evas = evas_object_evas_get(parent->get());
        eo = evas_object_image_filled_add(evas);
        construct(eo, obj);
     }

   virtual void image_set(v8::Handle<v8::Value> val)
     {
       if (val->IsString())
         {
            v8::String::Utf8Value str(val);
             if (0 > access(*str, R_OK))
               fprintf(stderr, "warning: can't read image file %s\n", *str);
            evas_object_image_file_set(eo, *str, NULL);
         }
     }

   virtual v8::Handle<v8::Value> image_get(void) const
     {
        const char *file = NULL, *key = NULL;
        evas_object_image_file_get(eo, &file, &key);
        if (file)
          return v8::String::New(file);
        else
          return v8::Null();
     }
};

class CElmBasicWindow : public CEvasObject {
public:
   CElmBasicWindow(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
        eo = elm_win_add(parent ? parent->get() : NULL, "main", ELM_WIN_BASIC);
        construct(eo, obj);
        v8::Handle<v8::Object> elements;
        elements = realize_objects(obj->Get(v8::String::New("elements")));
        get_object()->Set(v8::String::New("elements"), elements);
        evas_object_focus_set(eo, 1);
        evas_object_smart_callback_add(eo, "delete,request", &on_delete, NULL);
     }

   virtual v8::Handle<v8::Value> type_get(void) const
     {
        return v8::String::New("main");
     }

   virtual v8::Handle<v8::Value> label_get() const
     {
        return v8::String::New(elm_win_title_get(eo));
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

   virtual void resize_set(v8::Handle<v8::Value> val)
     {
        fprintf(stderr, "warning: resize=true ignored on main window\n");
     }
};

CElmBasicWindow *main_win;

class CElmButton : public CEvasObject {
public:
   CElmButton(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
        eo = elm_button_add(parent->top_widget_get());
        construct(eo, obj);
     }

   virtual ~CElmButton()
     {
     }

   virtual void label_set(const char *str)
     {
        elm_button_label_set(eo, str);
     }

   virtual v8::Handle<v8::Value> label_get() const
     {
       return v8::String::New(elm_button_label_get(eo));
     }
};

class CElmBackground : public CEvasObject {
public:
   CElmBackground(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
        eo = elm_bg_add(parent->top_widget_get());
        construct(eo, obj);
     }

   virtual ~CElmBackground()
     {
     }

   virtual void image_set(v8::Handle<v8::Value> val)
     {
        if (val->IsString())
          {
             v8::String::Utf8Value str(val);
             elm_bg_file_set(eo, *str, NULL);
          }
     }

   virtual v8::Handle<v8::Value> image_get(void) const
     {
        const char *file = NULL, *group = NULL;
        elm_bg_file_get(eo, &file, &group);
        if (file)
          return v8::String::New(file);
        else
          return v8::Null();
     }

};

class CElmRadio : public CEvasObject {
public:
   CElmRadio(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
        eo = elm_radio_add(parent->top_widget_get());
        construct(eo, obj);
     }

   virtual void label_set(const char *str)
     {
        elm_radio_label_set(eo, str);
     }

   virtual ~CElmRadio()
     {
     }
};

class CElmBox : public CEvasObject {
protected:
   virtual void add_child(CEvasObject *child)
     {
        elm_box_pack_end(eo, child->get());
     }

   static CPropHandler<CElmBox> prop_handler;

public:
   CElmBox(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
        eo = elm_box_add(parent->top_widget_get());
        construct(eo, obj);
        v8::Handle<v8::Object> elements;
        elements = realize_objects(obj->Get(v8::String::New("elements")));
        get_object()->Set(v8::String::New("elements"), elements);
     }

   virtual v8::Handle<v8::ObjectTemplate> get_template(void)
     {
        v8::Handle<v8::ObjectTemplate> ot = CEvasObject::get_template();
        prop_handler.fill_template(ot);
        return ot;
     }

   virtual bool prop_set(const char *prop_name, v8::Handle<v8::Value> value)
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

   virtual v8::Handle<v8::Value> prop_get(const char *prop_name) const
     {
        CPropHandler<CElmBox>::prop_getter getter;
        getter = prop_handler.get_getter(prop_name);
        if (getter)
          return (this->*getter)();
        return CEvasObject::prop_get(prop_name);
     }

   void horizontal_set(v8::Handle<v8::Value> val)
     {
        if (val->IsBoolean())
          {
             elm_box_horizontal_set(eo, val->BooleanValue());
          }
     }

   virtual v8::Handle<v8::Value> horizontal_get() const
     {
        return v8::Boolean::New(elm_box_horizontal_get(eo));
     }

   void homogeneous_set(v8::Handle<v8::Value> val)
     {
        if (val->IsBoolean())
          elm_box_homogeneous_set(eo, val->BooleanValue());
     }

   virtual v8::Handle<v8::Value> homogeneous_get() const
     {
        return v8::Boolean::New(elm_box_homogeneous_get(eo));
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
   CElmLabel(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
        eo = elm_label_add(parent->top_widget_get());
        construct(eo, obj);
     }

   virtual v8::Handle<v8::ObjectTemplate> get_template(void)
     {
        v8::Handle<v8::ObjectTemplate> ot = CEvasObject::get_template();
        prop_handler.fill_template(ot);
        return ot;
     }

   virtual bool prop_set(const char *prop_name, v8::Handle<v8::Value> value)
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

   virtual v8::Handle<v8::Value> prop_get(const char *prop_name) const
     {
        CPropHandler<CElmLabel>::prop_getter getter;
        getter = prop_handler.get_getter(prop_name);
        if (getter)
          return (this->*getter)();
        return CEvasObject::prop_get(prop_name);
     }

   virtual void wrap_set(v8::Handle<v8::Value> wrap)
     {
        if (wrap->IsNumber())
          elm_label_line_wrap_set(eo, static_cast<Elm_Wrap_Type>(wrap->Int32Value()));
     }

   virtual v8::Handle<v8::Value> wrap_get() const
     {
        return v8::Integer::New(elm_label_line_wrap_get(eo));
     }

   virtual void label_set(const char *str)
     {
        elm_label_label_set(eo, str);
     }

   virtual v8::Handle<v8::Value> label_get() const
     {
        return v8::String::New(elm_label_label_get(eo));
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
   static v8::Handle<v8::Value> do_flip(const v8::Arguments& args)
     {
        CEvasObject *self = eo_from_info(args.This());
        CElmFlip *flipper = static_cast<CElmFlip *>(self);
        flipper->flip(ELM_FLIP_ROTATE_Y_CENTER_AXIS);
        return v8::Undefined();
     }

   virtual void flip(Elm_Flip_Mode mode)
     {
        elm_flip_go(eo, mode);
     }

   CElmFlip(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
        CEvasObject *front, *back;

        eo = elm_flip_add(parent->top_widget_get());
        construct(eo, obj);

        /* realize front and back */
        front = realize_one(this, obj->Get(v8::String::New("front")));
        elm_flip_content_front_set(eo, front->get());

        back = realize_one(this, obj->Get(v8::String::New("back")));
        elm_flip_content_back_set(eo, back->get());

        get_object()->Set(v8::String::New("flip"), v8::FunctionTemplate::New(do_flip)->GetFunction());
     }
};

class CElmIcon : public CEvasObject {
public:
   CElmIcon(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
        eo = elm_icon_add(parent->top_widget_get());
        construct(eo, obj);
     }

   virtual v8::Handle<v8::ObjectTemplate> get_template(void)
     {
        v8::Handle<v8::ObjectTemplate> ot = CEvasObject::get_template();
        ot->SetAccessor(v8::String::New("scale"), &eo_getter, &eo_setter);
        return ot;
     }

   virtual bool prop_set(const char *prop_name, v8::Handle<v8::Value> value)
     {
        if (!strcmp(prop_name, "scale"))
          scale_set(value);
        else
          return CEvasObject::prop_set(prop_name, value);
        return true;
     }

   virtual v8::Handle<v8::Value> prop_get(const char *prop_name) const
     {
        if (!strcmp(prop_name, "scale"))
          return scale_get();
        return CEvasObject::prop_get(prop_name);
     }

   virtual void scale_set(v8::Handle<v8::Value> align)
     {
        bool x, y;
        if (get_xy_from_object(align, x, y))
          {
             elm_icon_scale_set(eo, x, y);
          }
     }

   virtual v8::Handle<v8::Value> scale_get() const
     {
        Eina_Bool x, y;
        elm_icon_scale_get(eo, &x, &y);
        v8::Local<v8::Object> obj = v8::Object::New();
        obj->Set(v8::String::New("x"), v8::Boolean::New(x));
        obj->Set(v8::String::New("y"), v8::Boolean::New(y));
        return obj;
     }

   virtual void image_set(v8::Handle<v8::Value> val)
     {
        if (val->IsString())
          {
             v8::String::Utf8Value str(val);
             if (0 > access(*str, R_OK))
               fprintf(stderr, "warning: can't read icon file %s\n", *str);
             elm_icon_file_set(eo, *str, NULL);
          }
     }

   virtual v8::Handle<v8::Value> image_get(void) const
     {
        const char *file = NULL, *group = NULL;
        elm_icon_file_get(eo, &file, &group);
        if (file)
          return v8::String::New(file);
        else
          return v8::Null();
     }
};

class CElmActionSlider : public CEvasObject {
private:
   static CPropHandler<CElmActionSlider> prop_handler;

public:
   CElmActionSlider(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
        eo = elm_actionslider_add(parent->top_widget_get());
        construct(eo, obj);
     }

   virtual v8::Handle<v8::ObjectTemplate> get_template(void)
     {
        v8::Handle<v8::ObjectTemplate> ot = CEvasObject::get_template();
        prop_handler.fill_template(ot);
        return ot;
     }

   virtual bool prop_set(const char *prop_name, v8::Handle<v8::Value> value)
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

   virtual v8::Handle<v8::Value> prop_get(const char *prop_name) const
     {
        CPropHandler<CElmActionSlider>::prop_getter getter;
        getter = prop_handler.get_getter(prop_name);
        if (getter)
          return (this->*getter)();
        return CEvasObject::prop_get(prop_name);
     }

   /* there's 1 indicator label and 3 position labels */
   virtual void label_set(const char *str)
     {
        elm_actionslider_indicator_label_set(eo, str);
     }

   virtual v8::Handle<v8::Value> label_get(v8::Handle<v8::Value> val) const
     {
        return v8::String::New(elm_actionslider_indicator_label_get(eo));
     }

   virtual void labels_set(v8::Handle<v8::Value> val)
     {
        if (val->IsObject())
          {
             v8::Local<v8::Object> obj = val->ToObject();
             v8::Local<v8::Value> v[3];
             v8::Local<v8::String> str[3];
             const char *name[3] = { "left", "center", "right" };

             for (int i = 0; i < 3; i++)
               {
                  v[i] = obj->Get(v8::String::New(name[i]));
                  if (v[i]->IsString())
                    str[i] = v[i]->ToString();
               }
             v8::String::Utf8Value left(str[0]), middle(str[1]), right(str[2]);
             elm_actionslider_labels_set(eo, *left, *middle, *right);
          }
     }

   virtual v8::Handle<v8::Value> labels_get() const
     {
        // FIXME: implement
        return v8::Undefined();
     }

   bool position_from_string(v8::Handle<v8::Value> val, Elm_Actionslider_Pos &pos)
     {
        if (!val->IsString())
          return false;

        v8::String::Utf8Value str(val);
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

   virtual void slider_set(v8::Handle<v8::Value> val)
     {
        Elm_Actionslider_Pos pos = ELM_ACTIONSLIDER_NONE;

        if (position_from_string(val, pos))
          elm_actionslider_indicator_pos_set(eo, pos);
     }

   virtual v8::Handle<v8::Value> slider_get() const
     {
        return v8::Integer::New(elm_actionslider_indicator_pos_get(eo));
     }

   virtual void magnet_set(v8::Handle<v8::Value> val)
     {
        Elm_Actionslider_Pos pos = ELM_ACTIONSLIDER_NONE;

        if (position_from_string(val, pos))
          elm_actionslider_magnet_pos_set(eo, pos);
     }

   virtual v8::Handle<v8::Value> magnet_get() const
     {
        return v8::Integer::New(elm_actionslider_magnet_pos_get(eo));
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
   CElmScroller(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
        CEvasObject *content;
        eo = elm_scroller_add(parent->top_widget_get());
        construct(eo, obj);
        content = realize_one(this, obj->Get(v8::String::New("content")));
        if (!content)
          fprintf(stderr, "scroller has no content\n");
        // FIXME: filter the object list copied in construct for more efficiency
        get_object()->Set(v8::String::New("content"), content->get_object());
        elm_scroller_content_set(eo, content->get());
     }

   virtual v8::Handle<v8::ObjectTemplate> get_template(void)
     {
        v8::Handle<v8::ObjectTemplate> ot = CEvasObject::get_template();
        prop_handler.fill_template(ot);
        return ot;
     }

   virtual bool prop_set(const char *prop_name, v8::Handle<v8::Value> value)
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

   virtual v8::Handle<v8::Value> prop_get(const char *prop_name) const
     {
        CPropHandler<CElmScroller>::prop_getter getter;
        getter = prop_handler.get_getter(prop_name);
        if (getter)
          return (this->*getter)();
        return CEvasObject::prop_get(prop_name);
     }

    virtual void bounce_set(v8::Handle<v8::Value> val)
     {
        bool x_bounce = false, y_bounce = false;
        if (get_xy_from_object(val, x_bounce, y_bounce))
          {
             elm_scroller_bounce_set(eo, x_bounce, y_bounce);
          }
     }

   virtual v8::Handle<v8::Value> bounce_get() const
     {
        Eina_Bool x, y;
        elm_scroller_bounce_get(eo, &x, &y);
        v8::Local<v8::Object> obj = v8::Object::New();
        obj->Set(v8::String::New("x"), v8::Boolean::New(x));
        obj->Set(v8::String::New("y"), v8::Boolean::New(y));
        return obj;
     }


   static Elm_Scroller_Policy policy_from_string(v8::Handle<v8::Value> val)
     {
        v8::String::Utf8Value str(val);
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

   static v8::Local<v8::Value> string_from_policy(Elm_Scroller_Policy policy)
     {
        switch (policy)
          {
        case ELM_SCROLLER_POLICY_AUTO:
          return v8::String::New("auto");
        case ELM_SCROLLER_POLICY_ON:
          return v8::String::New("on");
        case ELM_SCROLLER_POLICY_OFF:
          return v8::String::New("off");
        case ELM_SCROLLER_POLICY_LAST:
          return v8::String::New("last");
        default:
          return v8::String::New("unknown");
          }
     }

   virtual void policy_set(v8::Handle<v8::Value> val)
     {
        v8::Local<v8::Value> x_val, y_val;

        if (get_xy_from_object(val, x_val, y_val))
          {
             Elm_Scroller_Policy x_policy, y_policy;
             x_policy = policy_from_string(x_val);
             y_policy = policy_from_string(y_val);
             elm_scroller_policy_set(eo, x_policy, y_policy);
          }
     }

   virtual v8::Handle<v8::Value> policy_get() const
     {
        Elm_Scroller_Policy x_policy, y_policy;
        elm_scroller_policy_get(eo, &x_policy, &y_policy);
        v8::Local<v8::Object> obj = v8::Object::New();
        obj->Set(v8::String::New("x"), string_from_policy(x_policy));
        obj->Set(v8::String::New("y"), string_from_policy(y_policy));
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
   v8::Persistent<v8::Value> the_icon;
   v8::Persistent<v8::Value> the_end_object;
   v8::Persistent<v8::Value> on_changed_val;

public:
   CElmSlider(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
        eo = elm_slider_add(parent->top_widget_get());
        construct(eo, obj);
     }

   virtual ~CElmSlider()
     {
        the_icon.Dispose();
        the_end_object.Dispose();
        on_changed_val.Dispose();
     }

   virtual v8::Handle<v8::ObjectTemplate> get_template(void)
     {
        v8::Handle<v8::ObjectTemplate> ot = CEvasObject::get_template();
        ot->SetAccessor(v8::String::New("units"), &eo_getter, &eo_setter);
        ot->SetAccessor(v8::String::New("indicator"), &eo_getter, &eo_setter);
        ot->SetAccessor(v8::String::New("span"), &eo_getter, &eo_setter);
        ot->SetAccessor(v8::String::New("icon"), &eo_getter, &eo_setter);
        ot->SetAccessor(v8::String::New("value"), &eo_getter, &eo_setter);
        ot->SetAccessor(v8::String::New("min"), &eo_getter, &eo_setter);
        ot->SetAccessor(v8::String::New("max"), &eo_getter, &eo_setter);
        ot->SetAccessor(v8::String::New("inverted"), &eo_getter, &eo_setter);
        ot->SetAccessor(v8::String::New("end"), &eo_getter, &eo_setter);
        ot->SetAccessor(v8::String::New("on_changed"), &eo_getter, &eo_setter);
        return ot;
     }

   virtual bool prop_set(const char *prop_name, v8::Handle<v8::Value> value)
     {
        if (!strcmp(prop_name, "units"))
          units_set(value);
        else if (!strcmp(prop_name, "indicator"))
          indicator_set(value);
        else if (!strcmp(prop_name, "span"))
          span_set(value);
        else if (!strcmp(prop_name, "icon"))
          icon_set(value);
        else if (!strcmp(prop_name, "value"))
          value_set(value);
        else if (!strcmp(prop_name, "min"))
          min_set(value);
        else if (!strcmp(prop_name, "max"))
          max_set(value);
        else if (!strcmp(prop_name, "inverted"))
          inverted_set(value);
        else if (!strcmp(prop_name, "end"))
          end_set(value);
        else if (!strcmp(prop_name, "horizontal"))
          horizontal_set(value);
        else if (!strcmp(prop_name, "on_changed"))
          on_changed_set(value);
        else
          return CEvasObject::prop_set(prop_name, value);
        return true;
     }

   virtual v8::Handle<v8::Value> prop_get(const char *prop_name) const
     {
        if (!strcmp(prop_name, "units"))
          return units_get();
        else if (!strcmp(prop_name, "indicator"))
          return indicator_get();
        else if (!strcmp(prop_name, "span"))
          return span_get();
        else if (!strcmp(prop_name, "icon"))
          return icon_get();
        else if (!strcmp(prop_name, "value"))
          return value_get();
        else if (!strcmp(prop_name, "min"))
          return min_get();
        else if (!strcmp(prop_name, "max"))
          return max_get();
        else if (!strcmp(prop_name, "inverted"))
          return inverted_get();
        else if (!strcmp(prop_name, "end"))
          return end_get();
        else if (!strcmp(prop_name, "horizontal"))
          return horizontal_get();
        else if (!strcmp(prop_name, "on_changed"))
          return on_changed_get();
        return CEvasObject::prop_get(prop_name);
     }

   virtual void units_set(v8::Handle<v8::Value> value)
     {
        if (value->IsString())
          {
             v8::String::Utf8Value str(value);
             elm_slider_unit_format_set(eo, *str);
          }
     }

   virtual v8::Local<v8::Value> units_get() const
     {
        return v8::String::New(elm_slider_unit_format_get(eo));
     }

   virtual void indicator_set(v8::Handle<v8::Value> value)
     {
        if (value->IsString())
          {
            v8::String::Utf8Value str(value);
            elm_slider_indicator_format_set(eo, *str);
          }
     }

   virtual v8::Local<v8::Value> indicator_get() const
     {
        return v8::String::New(elm_slider_indicator_format_get(eo));
     }

   virtual void label_set(const char *str)
     {
        elm_slider_label_set(eo, str);
     }

   virtual v8::Handle<v8::Value> label_get() const
     {
        return v8::String::New(elm_slider_label_get(eo));
     }

   virtual v8::Local<v8::Value> span_get() const
     {
        return v8::Integer::New(elm_slider_span_size_get(eo));
     }

   virtual void span_set(v8::Handle<v8::Value> value)
     {
        if (value->IsInt32())
          {
             int span = value->Int32Value();
             elm_slider_span_size_set(eo, span);
          }
     }

   virtual v8::Handle<v8::Value> icon_get() const
     {
        return the_icon;
     }

   virtual void icon_set(v8::Handle<v8::Value> value)
     {
        the_icon.Dispose();
        CEvasObject *icon = realize_one(this, value);
        elm_slider_icon_set(eo, icon->get());
        the_icon = v8::Persistent<v8::Value>::New(icon->get_object());
     }

   virtual v8::Handle<v8::Value> end_get() const
     {
        return the_end_object;
     }

   virtual void end_set(v8::Handle<v8::Value> value)
     {
        the_end_object.Dispose();
        CEvasObject *end_obj = realize_one(this, value);
        if (end_obj)
          {
             elm_slider_end_set(eo, end_obj->get());
             the_end_object = v8::Persistent<v8::Value>::New(end_obj->get_object());
          }
        else
             elm_slider_end_unset(eo);
     }

   virtual v8::Handle<v8::Value> value_get() const
     {
        return v8::Number::New(elm_slider_value_get(eo));
     }

   virtual void value_set(v8::Handle<v8::Value> value)
     {
        if (value->IsNumber())
          elm_slider_value_set(eo, value->NumberValue());
     }

   virtual v8::Handle<v8::Value> min_get() const
     {
        double min, max;
        elm_slider_min_max_get(eo, &min, &max);
        return v8::Number::New(min);
     }

   virtual void min_set(v8::Handle<v8::Value> value)
     {
        if (value->IsNumber())
          {
             double min, max;
             elm_slider_min_max_get(eo, &min, &max);
             min = value->NumberValue();
             elm_slider_min_max_set(eo, min, max);
          }
     }

   virtual v8::Handle<v8::Value> max_get() const
     {
        double min, max;
        elm_slider_min_max_get(eo, &min, &max);
        return v8::Number::New(max);
     }

   virtual void max_set(v8::Handle<v8::Value> value)
     {
        if (value->IsNumber())
          {
             double min, max;
             elm_slider_min_max_get(eo, &min, &max);
             max = value->NumberValue();
             elm_slider_min_max_set(eo, min, max);
          }
     }

   virtual v8::Handle<v8::Value> inverted_get() const
     {
        return v8::Boolean::New(elm_slider_inverted_get(eo));
     }

   virtual void inverted_set(v8::Handle<v8::Value> value)
     {
        if (value->IsBoolean())
          elm_slider_inverted_set(eo, value->BooleanValue());
     }

   virtual v8::Handle<v8::Value> horizontal_get() const
     {
        return v8::Boolean::New(elm_slider_horizontal_get(eo));
     }

   virtual void horizontal_set(v8::Handle<v8::Value> value)
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
        v8::Handle<v8::Object> obj = get_object();
        v8::HandleScope handle_scope;
        v8::Handle<v8::Value> val = on_changed_val;
        // FIXME: pass event_info to the callback
        // FIXME: turn the pieces below into a do_callback method
        assert(val->IsFunction());
        v8::Handle<v8::Function> fn(v8::Function::Cast(*val));
        v8::Handle<v8::Value> args[1] = { obj };
        fn->Call(fn, 1, args);
     }

   virtual void on_changed_set(v8::Handle<v8::Value> val)
     {
        on_changed_val.Dispose();
        on_changed_val = v8::Persistent<v8::Value>::New(val);
        if (val->IsFunction())
          evas_object_smart_callback_add(eo, "changed", &eo_on_changed, this);
        else
          evas_object_smart_callback_del(eo, "changed", &eo_on_changed);
     }

   virtual v8::Handle<v8::Value> on_changed_get(void) const
     {
        return on_changed_val;
     }

};

class CElmGenList : public CEvasObject {
protected:
public:
   CElmGenList(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
        eo = elm_genlist_add(parent->top_widget_get());
        construct(eo, obj);
     }
};

class CElmList : public CEvasObject {
protected:
public:
   CElmList(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
        eo = elm_list_add(parent->top_widget_get());
        construct(eo, obj);
        items_set(obj->Get(v8::String::New("items")));
     }

   v8::Handle<v8::Object> items_set(v8::Handle<v8::Value> val)
     {
        /* add an list of children */
        v8::Local<v8::Object> out = v8::Object::New();

        if (!val->IsObject())
          {
             fprintf(stderr, "not an object!\n");
             return out;
          }

        v8::Local<v8::Object> in = val->ToObject();
        v8::Local<v8::Array> props = in->GetPropertyNames();

        /* iterate through elements and instantiate them */
        for (unsigned int i = 0; i < props->Length(); i++)
          {

             v8::Local<v8::Value> x = props->Get(v8::Integer::New(i));
             v8::String::Utf8Value val(x);

             v8::Local<v8::Value> item = in->Get(x->ToString());
             if (!item->IsObject())
               {
                  // FIXME: permit adding strings here?
                 fprintf(stderr, "list item is not an object\n");
                 continue;
               }
             v8::Local<v8::Value> label = item->ToObject()->Get(v8::String::New("label"));

             v8::String::Utf8Value str(label);
             elm_list_item_append(eo, *str, NULL, NULL, NULL, NULL);
          }

        return out;
     }
};

CEvasObject *
realize_one(CEvasObject *parent, v8::Handle<v8::Value> object_val)
{
   if (!object_val->IsObject())
     {
        fprintf(stderr, "%s: value is not an object!\n", __FUNCTION__);
        return NULL;
     }

   v8::Local<v8::Object> obj = object_val->ToObject();

   v8::Local<v8::Value> val = obj->Get(v8::String::New("type"));
   v8::String::Utf8Value str(val);

   /* create the evas object */
   // FIXME: make a list here
   CEvasObject *eo = NULL;
   if (!strcmp(*str, "actionslider"))
      {
         eo = new CElmActionSlider(parent, obj);
      }
   else if (!strcmp(*str, "button"))
      {
         eo = new CElmButton(parent, obj);
      }
   else if (!strcmp(*str, "background"))
      {
         eo = new CElmBackground(parent, obj);
      }
   else if (!strcmp(*str, "flip"))
      {
         eo = new CElmFlip(parent, obj);
      }
   else if (!strcmp(*str, "list"))
      {
         eo = new CElmList(parent, obj);
      }
   else if (!strcmp(*str, "genlist"))
      {
         eo = new CElmGenList(parent, obj);
      }
   else if (!strcmp(*str, "icon"))
      {
         eo = new CElmIcon(parent, obj);
      }
   else if (!strcmp(*str, "label"))
      {
         eo = new CElmLabel(parent, obj);
      }
   else if (!strcmp(*str, "radio"))
      {
         eo = new CElmRadio(parent, obj);
      }
   else if (!strcmp(*str, "box"))
      {
         eo = new CElmBox(parent, obj);
      }
   else if (!strcmp(*str, "scroller"))
      {
         eo = new CElmScroller(parent, obj);
      }
   else if (!strcmp(*str, "image"))
      {
         eo = new CEvasImage(parent, obj);
      }
   else if (!strcmp(*str, "slider"))
      {
         eo = new CElmSlider(parent, obj);
      }

   if (!eo)
      {
         fprintf(stderr, "Bad object type %s\n", *str);
         return eo;
      }

   return eo;
}

v8::Handle<v8::Value>
Realize(const v8::Arguments& args)
{
   if (args.Length() != 1)
     return v8::ThrowException(v8::String::New("Bad parameters"));
   realize_one(main_win, args[0]);
   return v8::Undefined();
}

v8::Handle<v8::Value>
Print(const v8::Arguments& args)
{
   for (int i = 0; i < args.Length(); i++)
     {
        v8::HandleScope handle_scope;
        v8::String::Utf8Value str(args[i]);
        printf("%s%s", i ? " ":"", *str);
     }
   printf("\n");
   fflush(stdout);
   return v8::Undefined();
}

int shebang_length(char *p, int len)
{
   int i = 0;

   if ((len > 2) && (p[0] == '#') && (p[1] == '!'))
     {
        for (i = 2; i < len && p[i] != '\n'; i++)
          ;
        /* leave first newline in so line numbers are correct */
     }

   return i;
}

v8::Local<v8::String>
string_from_file(const char *filename)
{
   v8::Local<v8::String> ret;
   int fd, len = 0;
   char *bad_ret = reinterpret_cast<char*>(MAP_FAILED);
   char *p = bad_ret;
   int n;

   fd = open(filename, O_RDONLY);
   if (fd < 0)
     goto fail;

   len = lseek(fd, 0, SEEK_END);
   if (len <= 0)
     goto fail;

   p = reinterpret_cast<char*>(mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0));
   if (p == bad_ret)
     goto fail;

   n = shebang_length(p, len);

   ret = v8::String::New(&p[n], len - n);

fail:
   if (p == bad_ret)
     munmap(p, len);

   if (fd >= 0)
     close(fd);

   return ret;
}

void
boom(v8::TryCatch &try_catch)
{
   v8::Handle<v8::Message> msg = try_catch.Message();
   v8::String::Utf8Value error(try_catch.Exception());

   if (msg.IsEmpty())
     {
        fprintf(stderr, "%s\n", *error);
     }
   else
     {
        v8::String::Utf8Value file(msg->GetScriptResourceName());
        int line = msg->GetLineNumber();

        fprintf(stderr, "%s:%d %s\n", *file, line, *error);
     }
   exit(1);
}

void
run_script(const char *filename)
{
   v8::HandleScope handle_scope;
   v8::TryCatch try_catch;

   /* load the script and run it */
   v8::Handle<v8::String> origin = v8::String::New(filename);
   v8::Handle<v8::String> source = string_from_file(filename);
   if (source.IsEmpty())
     {
        fprintf(stderr, "Failed to read source %s\n", filename);
        return;
     }

   /* compile */
   v8::Handle<v8::Script> script = v8::Script::Compile(source, origin);
   if (try_catch.HasCaught())
     boom(try_catch);

   /* run */
   v8::Handle<v8::Value> result = script->Run();
   if (try_catch.HasCaught())
     boom(try_catch);
}

v8::Handle<v8::Value>
elm_main_window(const v8::Arguments& args)
{
   if (args.Length() != 1)
     return v8::ThrowException(v8::String::New("Bad parameters"));

   if (!args[0]->IsObject())
     return v8::Undefined();

   main_win = new CElmBasicWindow(NULL, args[0]->ToObject());
   if (!main_win)
     return v8::Undefined();

   return main_win->get_object();
}

v8::Handle<v8::Value>
elm_loop_time(const v8::Arguments& args)
{
   return v8::Number::New(ecore_loop_time_get());
}

void
elev8_run(const char *script)
{
   v8::HandleScope handle_scope;
   v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();

   global->Set(v8::String::New("realize"), v8::FunctionTemplate::New(Realize));
   global->Set(v8::String::New("print"), v8::FunctionTemplate::New(Print));
   v8::Handle<v8::ObjectTemplate> elm = v8::ObjectTemplate::New();
   global->Set(v8::String::New("elm"), elm);

   elm->Set(v8::String::New("main"), v8::FunctionTemplate::New(elm_main_window));
   elm->Set(v8::String::New("loop_time"), v8::FunctionTemplate::New(elm_loop_time));

   /* setup V8 */
   v8::Persistent<v8::Context> context = v8::Context::New(NULL, global);
   v8::Context::Scope context_scope(context);
   run_script(script);

   elm_run();

   context.Dispose();
}

static void
main_help(const char *progname)
{
   printf("Usage:\n"
       "\t%s input_file.js\n"
       "\te.g. %s ../../data/javascript/button.js\n",
	progname, progname);
}

int
elm_main(int argc, char **argv)
{
   if (argc < 2)
     {
        fprintf(stderr, "%s: Error: no input file specified.\n", argv[0]);
        main_help(argv[0]);
        exit(-1);
     }
   v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
   elev8_run(argv[1]);
   v8::V8::Dispose();

   return 0;
}

ELM_MAIN()
