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
CEvasObject *realize_one(CEvasObject *parent, v8::Local<v8::Value> obj);

class CEvasObject {
   /* realize_one is a factory for our class */
   friend CEvasObject *realize_one(CEvasObject *parent, v8::Local<v8::Value> obj);
protected:
   Evas_Object *eo;
   v8::Persistent<v8::Object> the_object;
   bool is_resize;
protected:
   explicit CEvasObject() :
       eo(NULL),
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
        callback_set(obj->Get(v8::String::New("on_clicked")));
        animator_set(obj->Get(v8::String::New("on_animate")));
        weight_set(obj->Get(v8::String::New("weight")));

        v8::Handle<v8::Object> out = get_object();

        /* copy properties, one by one */
        v8::Handle<v8::Array> props = obj->GetPropertyNames();
        for (unsigned int i = 0; i < props->Length(); i++)
          {
             v8::Handle<v8::Value> name = props->Get(v8::Integer::New(i));
             v8::String::Utf8Value name_str(name);

             /* skip the type property */
             if (!strcmp(*name_str, "type"))
               continue;

             v8::Handle<v8::Value> value = obj->Get(name->ToString());
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

   static CEvasObject *eo_from_info(const v8::AccessorInfo& info)
     {
        v8::Handle<v8::Value> val = info.This()->Get(v8::String::New("_eo"));
        return static_cast<CEvasObject *>(v8::External::Unwrap(val));
     }

   static void eo_setter(v8::Local<v8::String> property,
                         v8::Local<v8::Value> value,
                         const v8::AccessorInfo& info)
     {
        CEvasObject *eo = eo_from_info(info);
        v8::String::Utf8Value prop_name(property);
        eo->prop_set(*prop_name, value);
        v8::String::Utf8Value val(value->ToString());
     }

   static v8::Handle<v8::Value> eo_getter(v8::Local<v8::String> property,
                                          const v8::AccessorInfo& info)
     {
        CEvasObject *eo = eo_from_info(info);
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
   virtual v8::Local<v8::ObjectTemplate> get_template(void)
     {
        /* FIXME: only need to create one template per object class */
        v8::Local<v8::ObjectTemplate> ot = v8::ObjectTemplate::New();
        ot->SetAccessor(v8::String::New("x"), &eo_getter, &eo_setter);
        ot->SetAccessor(v8::String::New("y"), &eo_getter, &eo_setter);
        ot->SetAccessor(v8::String::New("width"), &eo_getter, &eo_setter);
        ot->SetAccessor(v8::String::New("height"), &eo_getter, &eo_setter);
        ot->SetAccessor(v8::String::New("image"), &eo_getter, &eo_setter);
        ot->SetAccessor(v8::String::New("label"), &eo_getter, &eo_setter);
        ot->SetAccessor(v8::String::New("type"), &eo_getter, &eo_setter);
        ot->SetAccessor(v8::String::New("resize"), &eo_getter, &eo_setter);
        ot->SetAccessor(v8::String::New("align"), &eo_getter, &eo_setter);

        return ot;
     }

   virtual v8::Handle<v8::Object> get_object(void)
     {
        if (the_object.IsEmpty())
          {
             v8::Local<v8::ObjectTemplate> ot = get_template();
             the_object = v8::Persistent<v8::Object>((ot->NewInstance()));
             object_set_eo(the_object, this);

             /* FIXME: make handle a weak handle, and detect destruction */
          }
        return the_object;
     }

   virtual v8::Handle<v8::Value> type_get(void)
     {
        fprintf(stderr, "undefined object type!\n");
        return v8::Undefined();
     }

   Evas_Object *get()
     {
        return eo;
     }

   virtual bool prop_set(const char *prop_name, v8::Handle<v8::Value> value)
     {
        // FIXME: use a table
        if (!strcmp(prop_name, "type"))
          {
             fprintf(stderr, "object type cannot be changed");
             return false;
          }
        if (!strcmp(prop_name, "label"))
          label_set(value);
        else if (!strcmp(prop_name, "align"))
          align_set(value);
        else if (!strcmp(prop_name, "x"))
          x_set(value);
        else if (!strcmp(prop_name, "y"))
          y_set(value);
        else if (!strcmp(prop_name, "height"))
          height_set(value);
        else if (!strcmp(prop_name, "width"))
          width_set(value);
        else if (!strcmp(prop_name, "image"))
          image_set(value);
        else if (!strcmp(prop_name, "resize"))
          resize_set(value);
        else if (!strcmp(prop_name, "width"))
          width_set(value);
        else
          {
             return false;
          }
        return true;
     }

   virtual v8::Handle<v8::Value> prop_get(const char *prop_name)
     {
        // FIXME: use a table
        if (!strcmp(prop_name, "type"))
          return type_get();
        else if (!strcmp(prop_name, "align"))
          return align_get();
        else if (!strcmp(prop_name, "x"))
          return x_get();
        else if (!strcmp(prop_name, "y"))
          return y_get();
        else if (!strcmp(prop_name, "height"))
          return height_get();
        else if (!strcmp(prop_name, "width"))
          return width_get();
        else if (!strcmp(prop_name, "label"))
          return label_get();
        else if (!strcmp(prop_name, "image"))
          return image_get();
        else if (!strcmp(prop_name, "resize"))
          return resize_get();
        return v8::Undefined();
     }

   // FIXME: could add to the parent here... raster to figure out
   Evas_Object *top_widget_get()
     {
        return elm_object_top_widget_get(eo);
     }

   virtual ~CEvasObject()
     {
        evas_object_unref(eo);
        the_object.Dispose();
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

   virtual v8::Handle<v8::Value> x_get(void)
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

   virtual v8::Handle<v8::Value> y_get(void)
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

   virtual v8::Handle<v8::Value> height_get(void)
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

   virtual v8::Handle<v8::Value> width_get(void)
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
        v8::HandleScope handle_scope;
        v8::Local<v8::Object> obj(*get_object());
        v8::Handle<v8::Value> val = obj->Get(v8::String::New("on_clicked"));
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

   void callback_set(v8::Local<v8::Value> val)
     {
        if (val->IsFunction())
          evas_object_smart_callback_add(eo, "clicked", &eo_on_click, this);
        else
          evas_object_smart_callback_del(eo, "clicked", &eo_on_click);
     }

   virtual void on_animate(void)
     {
        v8::HandleScope handle_scope;
        v8::Local<v8::Object> obj(*get_object());
        v8::Handle<v8::Value> val = obj->Get(v8::String::New("on_animate"));
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

   void animator_set(v8::Local<v8::Value> val)
     {
        if (val->IsFunction())
          {
             ecore_animator_add(&eo_on_animate, this);
          }
     }

   virtual v8::Handle<v8::Value> label_get()
     {
       return v8::Undefined();
     }

   virtual void label_set(v8::Handle<v8::Value> val)
     {
        fprintf(stderr, "setting label on non-button\n");
     }

   bool get_xy_from_object(v8::Handle<v8::Value> val, double &x_out, double &y_out)
     {
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
        if (!val->IsObject())
          return false;
        v8::Local<v8::Object> obj = val->ToObject();
        x_val = obj->Get(v8::String::New("x"));
        y_val = obj->Get(v8::String::New("y"));
        return true;
     }

   virtual void weight_set(v8::Local<v8::Value> weight)
     {
        double x, y;
        if (get_xy_from_object(weight, x, y))
          evas_object_size_hint_weight_set(eo, x, y);
     }

   virtual void align_set(v8::Handle<v8::Value> align)
     {
        double x, y;
        if (get_xy_from_object(align, x, y))
          {
             evas_object_size_hint_align_set(eo, x, y);
          }
     }

   virtual v8::Handle<v8::Value> align_get(void)
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

   virtual v8::Handle<v8::Value> image_get(void)
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

   virtual v8::Handle<v8::Value> resize_get(void)
     {
        return v8::Boolean::New(is_resize);
     }
};

class CEvasImage : public CEvasObject {
public:
   CEvasImage(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
fprintf(stderr, "%d %s\n", __LINE__, __FUNCTION__);
        Evas *evas = evas_object_evas_get(parent->get());
        eo = evas_object_image_filled_add(evas);
        construct(eo, obj);
     }

   virtual void image_set(v8::Handle<v8::Value> val)
     {
fprintf(stderr, "%d %s\n", __LINE__, __FUNCTION__);
       if (val->IsString())
         {
            v8::String::Utf8Value str(val);
             if (0 > access(*str, R_OK))
               fprintf(stderr, "warning: can't read image file %s\n", *str);
            evas_object_image_file_set(eo, *str, NULL);
         }
     }

   virtual v8::Handle<v8::Value> image_get(void)
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

   virtual v8::Handle<v8::Value> type_get(void)
     {
        return v8::String::New("main");
     }

   virtual v8::Handle<v8::Value> label_get()
     {
        return v8::String::New(elm_win_title_get(eo));
     }

   virtual void label_set(v8::Handle<v8::Value> val)
     {
        if (val->IsString())
          {
             v8::String::Utf8Value str(val);
             elm_win_title_set(eo, *str);
          }
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

   virtual void label_set(v8::Handle<v8::Value> val)
     {
        if (val->IsString())
          {
             v8::String::Utf8Value str(val);
             elm_button_label_set(eo, *str);
          }
     }

   virtual v8::Handle<v8::Value> label_get()
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

   virtual v8::Handle<v8::Value> image_get(void)
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

   virtual void label_set(v8::Handle<v8::Value> val)
     {
        if (val->IsString())
          {
             v8::String::Utf8Value str(val);
             elm_radio_label_set(eo, *str);
          }
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

public:
   CElmBox(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
        eo = elm_box_add(parent->top_widget_get());
        construct(eo, obj);
        v8::Handle<v8::Object> elements;
        elements = realize_objects(obj->Get(v8::String::New("elements")));
        get_object()->Set(v8::String::New("elements"), elements);
        horizontal_set(obj->Get(v8::String::New("horizontal")));
     }

   void horizontal_set(v8::Local<v8::Value> val)
     {
        if (val->IsBoolean())
          {
             elm_box_horizontal_set(eo, val->BooleanValue());
          }
     }
};

class CElmLabel : public CEvasObject {
public:
   CElmLabel(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
        eo = elm_label_add(parent->top_widget_get());
        wrap_set(obj->Get(v8::String::New("wrap")));
        construct(eo, obj);
     }

   void wrap_set(v8::Local<v8::Value> wrap)
     {
        if (wrap->IsNumber())
          elm_label_line_wrap_set(eo, static_cast<Elm_Wrap_Type>(wrap->Int32Value()));
     }

   virtual void label_set(v8::Handle<v8::Value> val)
     {
        if (val->IsString())
          {
             v8::String::Utf8Value str(val);
             elm_label_label_set(eo, *str);
          }
     }
};

class CElmFlip : public CEvasObject {
public:
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
     }
};

class CElmIcon : public CEvasObject {
public:
   CElmIcon(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
        eo = elm_icon_add(parent->top_widget_get());
        construct(eo, obj);
        scale_set(obj->Get(v8::String::New("scale")));
     }

   virtual void scale_set(v8::Local<v8::Value> align)
     {
        bool x, y;
        if (get_xy_from_object(align, x, y))
          {
             elm_icon_scale_set(eo, x, y);
          }
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

   virtual v8::Handle<v8::Value> image_get(void)
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
public:
   CElmActionSlider(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
        eo = elm_actionslider_add(parent->top_widget_get());
        construct(eo, obj);
        magnet_set(obj->Get(v8::String::New("magnet")));
        slider_set(obj->Get(v8::String::New("slider")));
        labels_set(obj->Get(v8::String::New("labels")));
     }

   /* there's 1 indicator label and 3 position labels */
   virtual void label_set(v8::Handle<v8::Value> val)
     {
        if (val->IsString())
          {
             v8::String::Utf8Value str(val);
             elm_actionslider_indicator_label_set(eo, *str);
          }
     }

   virtual void labels_set(v8::Local<v8::Value> val)
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

   bool position_from_string(v8::Local<v8::Value> val, Elm_Actionslider_Pos &pos)
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

   void slider_set(v8::Local<v8::Value> val)
     {
        Elm_Actionslider_Pos pos = ELM_ACTIONSLIDER_NONE;

        if (position_from_string(val, pos))
          elm_actionslider_indicator_pos_set(eo, pos);
     }

   void magnet_set(v8::Local<v8::Value> val)
     {
        Elm_Actionslider_Pos pos = ELM_ACTIONSLIDER_NONE;

        if (position_from_string(val, pos))
          elm_actionslider_magnet_pos_set(eo, pos);
     }
};

class CElmScroller : public CEvasObject {
public:
   CElmScroller(CEvasObject *parent, v8::Local<v8::Object> obj) :
       CEvasObject()
     {
        CEvasObject *content;
        eo = elm_scroller_add(parent->top_widget_get());
        construct(eo, obj);
        bounce_set(obj->Get(v8::String::New("bounce")));
        policy_set(obj->Get(v8::String::New("policy")));
        content = realize_one(this, obj->Get(v8::String::New("content")));
        if (!content)
          fprintf(stderr, "scroller has no content\n");
        // FIXME: filter the object list copied in construct for more efficiency
        get_object()->Set(v8::String::New("content"), content->get_object());
        elm_scroller_content_set(eo, content->get());
     }

   void bounce_set(v8::Local<v8::Value> val)
     {
        bool x_bounce, y_bounce;
        if (get_xy_from_object(val, x_bounce, y_bounce))
          {
             elm_scroller_bounce_set(eo, x_bounce, y_bounce);
          }
     }

   Elm_Scroller_Policy policy_from_string(v8::Local<v8::Value> val)
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

   void policy_set(v8::Local<v8::Value> val)
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
};

CEvasObject *
realize_one(CEvasObject *parent, v8::Local<v8::Value> object_val)
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
   else if (!strcmp(*str, "pack"))
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

v8::Handle<v8::String>
string_from_file(const char *filename)
{
   v8::Handle<v8::String> ret = v8::Handle<v8::String>();
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
