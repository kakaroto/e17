#ifndef C_ELM_BUTTON_H
#define C_ELM_BUTTON_H

#include "elm.h"
#include "CElmLayout.h"

namespace elm {

class CElmButton : public CElmLayout {
private:
   static Persistent<FunctionTemplate> tmpl;

protected:
   CElmButton(Local<Object> _jsObject, CElmObject *parent);
   CElmButton(Local<Object> _jsObject, Evas_Object *child);
   virtual ~CElmButton();

   struct {
      Persistent<Value> icon;
   } cached;

   static Handle<FunctionTemplate> GetTemplate();

public:
   static void Initialize(Handle<Object> val);

   Handle<Value> icon_get() const;
   void icon_set(Handle<Value> value);

   Handle<Value> autorepeat_get() const;
   void autorepeat_set(Handle<Value> value);

   Handle<Value> autorepeat_initial_timeout_get() const;
   void autorepeat_initial_timeout_set(Handle<Value> value);

   Handle<Value> autorepeat_gap_timeout_get() const;
   void autorepeat_gap_timeout_set(Handle<Value> value);

   friend Handle<Value> CElmObject::New<CElmButton>(const Arguments& args);
};

}

#endif
