#ifndef C_EVAS_OBJECT_H
#define C_EVAS_OBJECT_H

#include <v8.h>

#include "elev8_elm.h"

using namespace v8;

// FIXME: split CElmObject from CEvasObject
class CEvasObject {
   /* make_or_get is a factory for our class */
   friend CEvasObject *make_or_get(CEvasObject *parent, Handle<Value> obj);

private:
   typedef CEvasObject *(*WidgetConstructor)(CEvasObject *parent, Local<Object> description);
   static Eina_Hash *constructor_map;

public:
   static void init_factory();
   static void register_widget(const char *type, WidgetConstructor constructor);
   static CEvasObject *make(const char *type, CEvasObject *parent, Local<Object> description);

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
   class CPropHandlerBase {
   public:
      virtual bool set(CEvasObject *eo, const char *prop_name, Handle<Value> value) = 0;
      virtual Handle<Value> get(const CEvasObject *eo, const char *prop_name) const = 0;
      virtual void fill_template(Handle<ObjectTemplate> &ot) = 0;
      virtual ~CPropHandlerBase() { }
   };

   /* property list for this class */
   CPropHandlerBase *property_list_base;

   template<class T> class CPropHandler : CPropHandlerBase {
   public:
     typedef Handle<Value> (T::*prop_getter)(void) const;
     typedef void (T::*prop_setter)(Handle<Value> val);

   private:
     struct CPropertyList {
       const char *name;
       prop_getter get;
       prop_setter set;
     };

     /* base class property list, setup in constructor */
     CPropHandlerBase *prev_list;

     static CPropertyList list[];
   public:
     explicit CPropHandler(CPropHandlerBase *&prev)
       {
          /* build linked list with base classes' properties */
          prev_list = prev;
          prev = this;
       }

     virtual ~CPropHandler() { }

     /*
      * the set method to set a property we know about
      */
     virtual bool set(CEvasObject *eo, const char *prop_name, Handle<Value> value)
       {
          T *self = static_cast<T*>(eo);

          for (CPropertyList *prop = list; prop->name; prop++)
            {
               if (!strcmp(prop->name, prop_name))
                 {
                    prop_setter set = prop->set;
                    (self->*set)(value);
                    return true;
                 }
            }

          /* traverse into base classes */
          if (!prev_list)
            return false;
          return prev_list->set(eo, prop_name, value);
       }

     /*
      * the get method to set a property we know about
      */
     virtual Handle<Value> get(const CEvasObject *eo, const char *prop_name) const
       {
          const T *self = static_cast<const T*>(eo);

          for (CPropertyList *prop = list; prop->name; prop++)
            {
               if (!strcmp(prop->name, prop_name))
                 {
                    prop_getter get = prop->get;
                    return (self->*get)();
                 }
            }
          /* traverse into base classes */
          if (!prev_list)
            return Undefined();
          return prev_list->get(eo, prop_name);
       }

     /*
      * Add an interceptor on a property on the given V8 object template
      */
     virtual void fill_template(Handle<ObjectTemplate> &ot)
       {
          for (CPropertyList *prop = list; prop->name; prop++)
            ot->SetAccessor(String::New(prop->name), &eo_getter, &eo_setter);
          /* traverse into base classes */
          if (!prev_list)
            return;
          return prev_list->fill_template(ot);
       }
   };

#define PROP_HANDLER(cls, foo) { #foo, &cls::foo##_get, &cls::foo##_set }
   CPropHandler<CEvasObject> prop_handler;

protected:
   explicit CEvasObject();

   /*
    * Two phase constructor required because Evas_Object type needs
    * to be known to be created.
    */
   void construct(Evas_Object *_eo, Local<Object> obj);

   virtual void add_child(CEvasObject *) { }

   CEvasObject *get_parent() const;

   void object_set_eo(Handle<Object> obj, CEvasObject *eo);

   static CEvasObject *eo_from_info(Handle<Object> obj);

   static void eo_setter(Local<String> property, Local<Value> value, const AccessorInfo& info);

   static Handle<Value> eo_getter(Local<String> property, const AccessorInfo& info);

   /* setup the property on construction */
   virtual void init_property(Handle<Object> out, Handle<Value> name, Handle<Value> value);

public:
   virtual Handle<Object> get_object(void);

   virtual Handle<ObjectTemplate> get_template(void);

   virtual Handle<Value> type_get(void) const
     {
        ELM_ERR( "undefined object type!");
        return Undefined();
     }

   virtual void type_set(Handle<Value>)
     {
        ELM_ERR( "type cannot be set!");
     }

   Evas_Object *get() const
     {
        return eo;
     }

   virtual CEvasObject *get_child(Handle<Value>)
     {
        ELM_ERR( "get_child undefined");
        return NULL;
     }

   /*
    * set a property
    * return true if successful, false if not
    */
   virtual bool prop_set(const char *prop_name, Handle<Value> value)
     {
        return property_list_base->set(this, prop_name, value);
     }

   virtual Handle<Value> prop_get(const char *prop_name) const
     {
        return property_list_base->get(this, prop_name);
     }

   // FIXME: could add to the parent here... raster to figure out
   Evas_Object *top_widget_get() const
     {
        return elm_object_top_widget_get(eo);
     }

   virtual ~CEvasObject();

   virtual void x_set(Handle<Value> val);

   virtual Handle<Value> x_get(void) const;

   virtual void y_set(Handle<Value> val);

   virtual Handle<Value> y_get(void) const;

   virtual void height_set(Handle<Value> val);

   virtual Handle<Value> height_get(void) const;

   virtual void width_set(Handle<Value> val);

   virtual Handle<Value> width_get(void) const;

   void move(Local<Value> x, Local<Value> y);

   virtual void on_click(void *event_info);

   static void eo_on_click(void *data, Evas_Object *, void *event_info);

   virtual void on_animate_set(Handle<Value> val);

   virtual Handle<Value> on_animate_get(void) const;

   virtual void on_clicked_set(Handle<Value> val);

   virtual Handle<Value> on_clicked_get(void) const;

   virtual void on_keydown(Evas_Event_Key_Down *info);

   static void eo_on_keydown(void *data, Evas *, Evas_Object *, void *event_info);

   virtual void on_keydown_set(Handle<Value> val);

   virtual Handle<Value> on_keydown_get(void) const;

   virtual void on_animate(void);

   static Eina_Bool eo_on_animate(void *data);

   virtual Handle<Value> label_get() const;

   virtual Handle<Value> text_get() const;

   virtual void label_set(Handle<Value> val);

   virtual void text_set(Handle<Value> val);

   virtual void label_set(const char *str);

   virtual Handle<Value> disabled_get() const;

   virtual void disabled_set(Handle<Value> val);

   virtual Handle<Value> scale_get() const;

   virtual void scale_set(Handle<Value> val);

   bool get_xy_from_object(Handle<Value> val, double &x_out, double &y_out);

   bool get_xy_from_object(Handle<Value> val, bool &x_out, bool &y_out);

   bool get_xy_from_object(Handle<Value> val, int &x_out, int &y_out);

   bool get_xy_from_object(Handle<Value> val, Handle<Value> &x_val, Handle<Value> &y_val);

   virtual void weight_set(Handle<Value> weight);

   virtual Handle<Value> weight_get(void) const;

   virtual void align_set(Handle<Value> align);

   virtual Handle<Value> align_get(void) const;

   virtual void image_set(Handle<Value> val);

   virtual Handle<Value> image_get(void) const;

   virtual void show(bool show);

   /* returns a list of children in an object */
   Handle<Object> realize_objects(Handle<Value> val, Handle<Object> &out);

   /* resize this object when the parent resizes? */
   virtual void resize_set(Handle<Value> val);

   virtual Handle<Value> resize_get(void) const;

   virtual void pointer_set(Handle<Value>)
     {
        // FIXME: ignore this, or move the pointer?
     }

   virtual Handle<Value> pointer_get(void) const;

   virtual void style_set(Handle<Value> val);

   virtual Handle<Value> style_get(void) const;

   virtual void visible_set(Handle<Value> val);

   virtual Handle<Value> visible_get(void) const;

   virtual void hint_min_set(Handle<Value> val);

   virtual Handle<Value> hint_min_get(void) const;

   virtual void hint_max_set(Handle<Value> val);

   virtual Handle<Value> hint_max_get(void) const;

   virtual void focus_set(Handle<Value> val);

   virtual Handle<Value> focus_get(void) const;

   virtual Handle<Value> layer_get() const;

   virtual void layer_set(Handle<Value> val);

   virtual void name_set(Handle<Value> val);

   virtual Handle<Value> name_get(void) const;

   virtual void hint_req_set(Handle<Value> val);

   virtual Handle<Value> hint_req_get(void) const;

   virtual void padding_set(Handle<Value> val);

   virtual Handle<Value> padding_get(void) const;

   virtual Handle<Value> pointer_mode_get() const;

   virtual void pointer_mode_set(Handle<Value> val);

   virtual void antialias_set(Handle<Value> val);

   virtual Handle<Value> antialias_get(void) const;

   virtual void static_clip_set(Handle<Value> val);

   virtual Handle<Value> static_clip_get(void) const;

   virtual void size_hint_aspect_set(Handle<Value> val);

   virtual Handle<Value> size_hint_aspect_get(void) const;
};

#define FACTORY(type_) \
   public: \
      static CEvasObject* make(CEvasObject *parent, Local<Object> description) { \
         return new type_(parent, description); \
      }

#define PROPERTIES_OF(type_) \
   template<> CEvasObject::CPropHandler<type_>::CPropertyList CEvasObject::CPropHandler<type_>::list[]

#define NO_PROPERTIES { { NULL } }

#endif
