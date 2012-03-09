#ifndef C_ELM_CHECK_H
#define C_ELM_CHECK_H

#include <v8.h>
#include "CEvasObject.h"

class CElmCheck : public CEvasObject {
   FACTORY(CElmCheck)
protected:
   CPropHandler<CElmCheck> prop_handler;
   Persistent<Value> the_icon;

public:
   CElmCheck(CEvasObject *parent, Local<Object> obj); 

   virtual ~CElmCheck();

   virtual void state_set(Handle<Value> value);

   virtual Handle<Value> state_get() const;

   virtual Handle<Value> icon_get() const;

   virtual void icon_set(Handle<Value> value);
};

#endif
