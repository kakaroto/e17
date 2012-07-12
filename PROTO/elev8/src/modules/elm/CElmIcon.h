#ifndef C_ELM_ICON_H
#define C_ELM_ICON_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmIcon : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmIcon(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> target);
   virtual void resizable_up_set(Handle<Value> val);
   virtual Handle<Value> resizable_up_get() const;
   virtual void resizable_down_set(Handle<Value> val);
   virtual Handle<Value> prescale_get() const;
   virtual void prescale_set(Handle<Value> val);
   virtual Handle<Value> resizable_down_get() const;
   virtual void image_set(Handle<Value> val);
   virtual Handle<Value> image_get(void) const;

   Handle<Value> lookup_order_get() const;
   void lookup_order_set(Handle<Value> val);

   friend Handle<Value> CElmObject::New<CElmIcon>(const Arguments& args);
};

}

#endif // C_ELM_ICON_H
