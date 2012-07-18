#ifndef C_ELM_RADIO_H
#define C_ELM_RADIO_H

#include "elm.h"
#include "CElmLayout.h"
#include "CElmObject.h"

namespace elm {

using namespace v8;

class CElmRadio : public CElmLayout {
private:
   CElmObject *parent;
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmRadio(Local<Object> _jsObject, CElmObject *parent);
   static Handle<FunctionTemplate> GetTemplate();
   virtual ~CElmRadio();

   struct {
        Persistent<Value> icon;
        Persistent<Value> group;
   } cached;

public:
   static void Initialize(Handle<Object> target);

   Handle<Value> icon_get() const;
   void icon_set(Handle<Value> value);

   Handle<Value> group_get() const;
   void group_set(Handle<Value> value);

   Handle<Value> value_get() const;
   void value_set(Handle<Value> value);

   Handle<Value> group_value_get() const;
   void group_value_set(Handle<Value> value);

   Handle<Value> group_selected_get() const;

   friend Handle<Value> CElmObject::New<CElmRadio>(const Arguments& args);
};

}

#endif // C_ELM_RADIO_H
