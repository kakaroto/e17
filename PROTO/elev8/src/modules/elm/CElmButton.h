#ifndef C_ELM_BUTTON_H
#define C_ELM_BUTTON_H

#include "elm.h"
#include "CElmObject.h"

namespace elm {

class CElmButton : public CElmObject {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmButton(Local<Object> _jsObject, CElmObject *parent);
   virtual ~CElmButton();

   struct {
      Persistent<Value> icon;
   } cached;

   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> val);

   Handle<Value> icon_get() const;
   void icon_set(Handle<Value> value);

   friend Handle<Value> CElmObject::New<CElmButton>(const Arguments& args);
};

}

#endif
