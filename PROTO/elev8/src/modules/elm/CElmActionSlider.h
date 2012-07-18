#ifndef C_ELM_ACTION_SLIDER_H
#define C_ELM_ACTION_SLIDER_H

#include "elm.h"
#include "CElmObject.h"
#include "CElmLayout.h"

namespace elm {

using namespace v8;

class CElmActionSlider : public CElmLayout {
private:
   static Persistent<FunctionTemplate> tmpl;

   bool position_from_string(Handle<Value> val, Elm_Actionslider_Pos &pos);
protected:
   CElmActionSlider(Local<Object> _jsObject, CElmObject *parent);
   ~CElmActionSlider();

   static Handle<FunctionTemplate> GetTemplate();

   struct {
      Persistent<Value> select;
   } cb;

public:
   static void Initialize(Handle<Object> target);

   void labels_set(Handle<Value> val);
   Handle<Value> labels_get() const;

   void slider_set(Handle<Value> val);
   Handle<Value> slider_get() const;

   void magnet_set(Handle<Value> val);
   Handle<Value> magnet_get() const;

   void OnSelect(void *event_info);
   static void OnSelectWrapper(void *data, Evas_Object *, void *event_info);

   Handle<Value> on_select_get() const;
   void on_select_set(Handle<Value> val);

   void enabled_pos_set(Handle<Value> val);
   Handle<Value> enabled_pos_get() const;

   friend Handle<Value> CElmObject::New<CElmActionSlider>(const Arguments& args);
};

}

#endif
