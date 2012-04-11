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

   Ecore_Animator *current_animator;

   CElmObject(Local<Object> _jsObject, Evas_Object *_eo);
   virtual ~CElmObject();

   static Handle<FunctionTemplate> GetTemplate();

   void ApplyProperties(Handle<Object> obj);

public:
   Evas_Object *GetEvasObject() const { return eo; }

   Handle<Value> Getx() const;
   void Setx(Handle<Value> value);

   Handle<Value> Gety() const;
   void Sety(Handle<Value> val);

   Handle<Value> Getwidth() const;
   void Setwidth(Handle<Value> val);

   Handle<Value> Getheight() const;
   void Setheight(Handle<Value> val);

   Handle<Value> Getalign() const;
   void Setalign(Handle<Value> align);

   Handle<Value> Getweight() const;
   void Setweight(Handle<Value> align);

   Handle<Value> Getvisible() const;
   void Setvisible(Handle<Value> val);

   Handle<Value> Getenabled() const;
   void Setenabled(Handle<Value> val);

   Handle<Value> Gethint_min() const;
   void Sethint_min(Handle<Value> val);

   Handle<Value> Gethint_max() const;
   void Sethint_max(Handle<Value> val);

   Handle<Value> Gethint_req() const;
   void Sethint_req(Handle<Value> val);

   Handle<Value> Getfocus() const;
   void Setfocus(Handle<Value> val);

   Handle<Value> Getlayer() const;
   void Setlayer(Handle<Value> val);

   Handle<Value> Getpadding() const;
   void Setpadding(Handle<Value> val);

   void Setpointer_mode(Handle<Value> val);
   Handle<Value> Getpointer_mode() const;

   Handle<Value> Getantialias() const;
   void Setantialias(Handle<Value> val);

   Handle<Value> Getstatic_clip() const;
   void Setstatic_clip(Handle<Value> val);

   Handle<Value> Getsize_hint_aspect() const;
   void Setsize_hint_aspect(Handle<Value> val);

   Handle<Value> Getname() const;
   void Setname(Handle<Value> val);

   Handle<Value> Getpointer() const;
   void Setpointer(Handle<Value>);

   void OnAnimate();
   static Eina_Bool OnAnimateWrapper(void *data);

   Handle<Value> Geton_animate() const;
   void Seton_animate(Handle<Value> val);

   void OnClick(void *event_info);
   static void OnClickWrapper(void *data, Evas_Object *, void *event_info);

   Handle<Value> Geton_click() const;
   void Seton_click(Handle<Value> val);

   void OnKeyDown(Evas_Event_Key_Down *event);
   static void OnKeyDownWrapper(void *data, Evas *, Evas_Object *, void *event_info);

   Handle<Value> Geton_key_down() const;
   void Seton_key_down(Handle<Value> val);
};

}

#endif
