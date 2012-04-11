#ifndef C_ELM_ACTION_SLIDER_H
#define C_ELM_ACTION_SLIDER_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmActionSlider : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

   bool position_from_string(Handle<Value> val, Elm_Actionslider_Pos &pos);
protected:
   CElmActionSlider(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> target);

   void labels_set(Handle<Value> val);
   Handle<Value> labels_get() const;

   void slider_set(Handle<Value> val);
   Handle<Value> slider_get() const;

   void magnet_set(Handle<Value> val);
   Handle<Value> magnet_get() const;

   friend Handle<Value> CElmObject::New<CElmActionSlider>(const Arguments& args);
};

}

#endif
