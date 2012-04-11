#ifndef _CELM_OBJECT_H
#define _CELM_OBJECT_H

#include "elm.h"

namespace elm {

using namespace v8;

class CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   Evas_Object *eo;
   Persistent<Object> jsObject;

   struct {
      Persistent<Value> animate;
      Persistent<Value> click;
      Persistent<Value> key_down;
   } cb;

   struct {
      bool isResize;
   } cached;

   Ecore_Animator *current_animator;

   CElmObject(Local<Object> _jsObject, Evas_Object *_eo);
   virtual ~CElmObject();

   static Handle<FunctionTemplate> GetTemplate();

   void ApplyProperties(Handle<Object> obj);

   template <class T>
   static Handle<Value> New(const Arguments& args)
     {
        HandleScope scope;

        if (!args.IsConstructCall())
          {
             args[0]->ToObject()->SetHiddenValue(String::New("type"), T::GetTemplate()->GetFunction());
             return args[0];
          }

        CElmObject *parent = (args[1] == Undefined()) ? NULL :
           static_cast<CElmObject *>(args[1]->ToObject()->GetPointerFromInternalField(0));

        T *obj = new T(args.This(), parent);
        obj->jsObject.MakeWeak(obj, T::Delete);

        return Undefined();
     }

   static void Delete(Persistent<Value>, void *parameter);

public:
   static void Initialize(Handle<Object> target);

   Evas_Object *GetEvasObject() const { return eo; }
   virtual void DidRealiseElement(Local<Value>) {}

   Handle<Value> x_get() const;
   void x_set(Handle<Value> value);

   Handle<Value> y_get() const;
   void y_set(Handle<Value> val);

   Handle<Value> width_get() const;
   void width_set(Handle<Value> val);

   Handle<Value> height_get() const;
   void height_set(Handle<Value> val);

   Handle<Value> text_get() const;
   void text_set(Handle<Value> val);

   Handle<Value> scale_get() const;
   void scale_set(Handle<Value> val);

   Handle<Value> style_get() const;
   void style_set(Handle<Value> val);

   Handle<Value> align_get() const;
   void align_set(Handle<Value> align);

   Handle<Value> weight_get() const;
   void weight_set(Handle<Value> align);

   Handle<Value> visible_get() const;
   void visible_set(Handle<Value> val);

   Handle<Value> enabled_get() const;
   void enabled_set(Handle<Value> val);

   Handle<Value> hint_min_get() const;
   void hint_min_set(Handle<Value> val);

   Handle<Value> hint_max_get() const;
   void hint_max_set(Handle<Value> val);

   Handle<Value> hint_req_get() const;
   void hint_req_set(Handle<Value> val);

   Handle<Value> focus_get() const;
   void focus_set(Handle<Value> val);

   Handle<Value> layer_get() const;
   void layer_set(Handle<Value> val);

   Handle<Value> padding_get() const;
   void padding_set(Handle<Value> val);

   void pointer_mode_set(Handle<Value> val);
   Handle<Value> pointer_mode_get() const;

   Handle<Value> antialias_get() const;
   void antialias_set(Handle<Value> val);

   Handle<Value> static_clip_get() const;
   void static_clip_set(Handle<Value> val);

   Handle<Value> size_hint_aspect_get() const;
   void size_hint_aspect_set(Handle<Value> val);

   Handle<Value> name_get() const;
   void name_set(Handle<Value> val);

   Handle<Value> resize_get() const;
   void resize_set(Handle<Value>);

   Handle<Value> pointer_get() const;
   void pointer_set(Handle<Value>);

   void OnAnimate();
   static Eina_Bool OnAnimateWrapper(void *data);

   Handle<Value> on_animate_get() const;
   void on_animate_set(Handle<Value> val);

   void OnClick(void *event_info);
   static void OnClickWrapper(void *data, Evas_Object *, void *event_info);

   Handle<Value> on_click_get() const;
   void on_click_set(Handle<Value> val);

   void OnKeyDown(Evas_Event_Key_Down *event);
   static void OnKeyDownWrapper(void *data, Evas *, Evas_Object *, void *event_info);

   Handle<Value> on_key_down_get() const;
   void on_key_down_set(Handle<Value> val);

   Handle<Value> elements_get() const;
   void elements_set(Handle<Value> val);

   static Handle<Value> Realise(const Arguments& args);

   static Local<Value> Realise(Handle<Value> desc, Handle<Value> parent);
};

}

#endif
